// Tcohneyn All Rights Reserved


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"


// 静态工厂函数：用于创建并返回一个UTargetDataUnderMouse任务实例。
// 这是UE中AbilityTask的标准创建模式，方便在蓝图或C++中调用。
UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
    // 使用模板函数NewAbilityTask来创建新的任务对象。
    // OwningAbility参数表示拥有此任务的那个GameplayAbility（游戏技能）。
    UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
    // 返回新创建的任务对象。
    return MyObj;
}

// 重写自UAbilityTask。当此任务被激活时，GAS会自动调用此函数。
// 这是整个异步任务的逻辑起点，负责决定数据流的方向（客户端发送还是服务器监听）。
void UTargetDataUnderMouse::Activate()
{
    // 检查当前执行此能力的Actor是否由本地玩家控制。
    // 这是关键的分支判断，决定了后续操作是在客户端执行还是服务器端执行。
    const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
    
    if (bIsLocallyControlled)
    {
        // 情况1：在本地控制的客户端上执行。
        // 因为客户端能直接获取到鼠标输入，所以由此客户端负责采集鼠标位置数据并发送给服务器。
        SendMouseCursorData();
    }
    else
    {
        // 情况2：在服务器上执行（或由非本地控制的客户端执行，如网络游戏中的旁观者）。
        // 服务器需要监听并等待客户端发送过来的目标数据[1,6](@ref)。
        
        // 获取当前技能的唯一标识符（句柄），用于在多个技能实例中精确识别当前技能[1](@ref)
        const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
        // 获取当前技能激活的预测键，用于匹配客户端预测操作和服务器确认[1](@ref)
        const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
        
        // 注册一个委托（回调函数），当服务器收到客户端发送的目标数据时自动触发[1,6](@ref)
        // 这里绑定到OnTargetDataReplicatedCallback函数，该函数负责处理接收到的数据
        AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
        
        // 尝试立即调用已设置的委托（如果数据已经到达）[1](@ref)
        // 这在数据可能先于委托注册到达的情况下很有用，避免错过已到达的数据[6](@ref)
        const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
        
        // 如果数据尚未到达（即委托没有被立即调用）
        if (!bCalledDelegate)
        {
            // 设置任务状态为"等待远程玩家数据"[1](@ref)
            // 这通常会暂停任务的执行，直到数据到达后通过委托恢复[6](@ref)
            SetWaitingOnRemotePlayerData();
        }
    }
}
// 此函数在本地控制的客户端上执行，负责获取鼠标光标下的碰撞点，并将数据同步到服务器。
void UTargetDataUnderMouse::SendMouseCursorData()
{
    // 创建一个客户端预测作用域窗口。
    // 在此对象生命周期内，客户端的操作可以进行预测执行，无需等待服务器确认，极大提升操作响应速度。
    FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
    
    // 获取拥有此技能的Actor的玩家控制器，以便进行输入和射线检测。
    APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
    
    // 声明一个碰撞检测结果变量。
    FHitResult CursorHit;
    // 从鼠标光标当前位置向游戏世界发射一条射线（检测可见性通道），并获取碰撞结果。
    // 这决定了技能将要施放的目标位置或目标物体。
    PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

    // 创建一个目标数据句柄（Handle），用于在网络间安全地传递和引用目标数据。
    FGameplayAbilityTargetDataHandle DataHandle;
    // 在堆上创建一个新的单目标命中数据对象，并填入射线检测的结果。
    FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
    Data->HitResult = CursorHit;
    // 将数据对象添加到句柄中。句柄会管理其内存生命周期。
    DataHandle.Add(Data);
    
    // 关键的RPC（远程过程调用）：客户端调用此函数，将封装好的目标数据发送到服务器。
    // 这是实现网络同步的核心步骤，确保服务器和客户端对技能目标有一致的认知。
    AbilitySystemComponent->ServerSetReplicatedTargetData(
        GetAbilitySpecHandle(),        // 当前技能的唯一句柄，用于标识是哪个技能的数据。
        GetActivationPredictionKey(),  // 预测键，用于匹配客户端的预测操作和服务器的最终确认，处理回滚。
        DataHandle,                    // 要同步的目标数据（即刚才获取的鼠标命中点）。
        FGameplayTag(),                // 与此数据关联的游戏标签（本例中未使用）。
        AbilitySystemComponent->ScopedPredictionKey // 当前预测作用域的键，进一步确保预测同步的准确性。
    );

    // 检查是否应该广播任务委托（例如，任务是否已被取消）。
    // 这个检查很重要，可以避免在任务已经无效的情况下还去广播数据，导致意外行为。
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        // 广播有效数据，通知所有绑定了此委托的蓝图或C++函数。
        // 例如，在蓝图中，连接到此ValidData委托的节点将会被触发，并接收到DataHandle中的数据。
        ValidData.Broadcast(DataHandle);
    }
}

// 当客户端发送的目标数据成功复制到服务器时，系统会自动调用此回调函数
void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
    // 关键步骤：消费（确认接收）已复制的目标数据
    // 这会清除GAS系统内部为此次预测所缓存的临时数据，避免重复处理，并标记该预测键（Prediction Key）对应的事件已在服务器确认完成[5](@ref)
    AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
    
    // 安全检查：判断当前是否应该广播任务委托（例如，任务未被强制取消）
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        // 广播有效数据：通知所有绑定了此委托的蓝图或C++函数
        // 例如，在蓝图中连接了此委托的节点会接收到 DataHandle，从而执行后续技能逻辑[5](@ref)
        ValidData.Broadcast(DataHandle);
    }
}