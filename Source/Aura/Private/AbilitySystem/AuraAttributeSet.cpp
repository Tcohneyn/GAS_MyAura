// Tcohneyn All Rights Reserved


#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
    InitHealth(50.f);
    InitMaxHealth(100.f);
    InitMana(25.f);
    InitMaxMana(50.f);
}
// 网络同步属性时需要重写这个函数
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    // 使用 DOREPLIFETIME_CONDITION_NOTIFY 宏注册属性的同步规则
    // 参数依次是：类名、属性名、同步条件、是否始终触发 OnRep
    // REPNOTIFY_Always 表示即使值没变也会触发 OnRep_XXX 
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Health,COND_None,REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Mana,COND_None,REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxMana,COND_None,REPNOTIFY_Always);
}
// 在属性发生改变之前调用
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    // 调用父类逻辑，保持基础行为
    Super::PreAttributeChange(Attribute, NewValue);
    // 如果修改的是 Health 属性
    if (Attribute == GetHealthAttribute())
    {
        // 将 NewValue 限制在 [0, MaxHealth] 区间
        // 防止血量小于0 或 超过最大血量
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
    // 如果修改的是 Mana 属性
    if (Attribute == GetManaAttribute())
    {
        // 将 NewValue 限制在 [0, MaxMana] 区间
        // 防止法力值小于0 或 超过最大法力
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
    }
}
// 从 GameplayEffect 回调数据中提取 Source（效果施加者）和 Target（效果接受者）的信息
void UAuraAttributeSet::GetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
    // Source = causer of the effect, Target = target of the effect (owner of this AS)
    // Source = 效果来源（施加者），Target = 效果目标（这个 AttributeSet 的拥有者）

    // 获取 Effect Context（包含效果来源的上下文信息，比如施法者、技能来源等）
    Props.EffectContextHandle = Data.EffectSpec.GetContext();
    // 从 Context 中获取 Source 的 AbilitySystemComponent
    Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
    // 检查 Source ASC 是否有效，以及它的 Actor 信息是否可用
    if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
    {
        // 获取 Source Avatar（施加效果的实际 Actor，例如角色）
        Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
        // 获取 Source Controller（通常是 PlayerController 或 AIController）
        Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
        // 如果 Controller 为空，但 AvatarActor 是 Pawn
        if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
        {
            if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
            {
                // 从 Pawn 上获取 Controller
                Props.SourceController = Pawn->GetController();
            }
        }
        // 如果 Controller 存在，则从 Controller 的 Pawn 获取 Source Character
        if (Props.SourceController)
        {
            Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
        }
    }
    // 获取 Target（效果接受者）的信息
    if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
    {
        // 获取 Target Avatar（受效果影响的角色/对象）
        Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
        // 获取 Target Controller（可能是 PlayerController 或 AIController）
        Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
        // 获取 Target Character（受效果影响的角色）
        Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
        // 保存 Target 的 ASC
        Props.TargetASC = &Data.Target;
    }
}
// 在 GameplayEffect 执行并修改属性之后调用
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
	
    // 定义一个 Props 结构体来保存 Source / Target 信息
    FEffectProperties Props;

    // 调用上面封装好的函数，提取数据并填充到 Props 中
    GetEffectProperties(Data, Props);
    //Health 与 Mana 超过最大值时被限制
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
    }
    if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
    }
}
// ---------------------- OnRep 回调 ----------------------
// 当属性在客户端被复制（replicate）时会调用这些函数
// OldXXX 参数表示同步前的旧值
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
    // GAMEPLAYATTRIBUTE_REPNOTIFY 宏：
    // 通知 AbilitySystemComponent 该属性发生变化，触发相关回调（比如 UI 更新）
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Health,OldHealth);
}


void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxHealth,OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Mana,OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxMana,OldMaxMana);
}