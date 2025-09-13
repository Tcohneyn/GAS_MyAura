// Tcohneyn All Rights Reserved


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
    PrimaryActorTick.bCanEverTick = false;
    SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}


void AAuraEffectActor::BeginPlay()
{
    Super::BeginPlay();
}
// ===============================
// 核心函数：向目标施加效果
// ===============================
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
    // 从目标 Actor 获取 AbilitySystemComponent（核心能力系统组件）
    // 如果目标不具备 ASC（比如不是可受效果的角色），直接返回
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    if (TargetASC == nullptr) return;

    // 确保传入的 GameplayEffectClass 合法（非空），否则会触发断言错误
    check(GameplayEffectClass);

    // 创建一个 GameplayEffectContextHandle（效果上下文），
    // 保存效果应用时的额外信息（来源对象、施法者等）
    FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();

    // 把当前 AuraEffectActor 作为 SourceObject 添加到上下文中，
    // 方便之后追踪效果来源（例如谁制造的伤害/光环）
    EffectContextHandle.AddSourceObject(this);

    // 创建一个 GameplayEffectSpecHandle（效果说明书），
    // 指定要应用的 GameplayEffect 类型、等级（这里固定为 1.0f）、和上下文
    const FGameplayEffectSpecHandle EffectSpecHandle = 
        TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);

    // 将效果应用到目标自身（Self），传入的是 Spec 数据（展开后的效果）
    //TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

    // 将效果应用到目标自身，返回 ActiveEffectHandle 用于后续管理
    const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

    // 检查效果是否是无限持续
    const bool bIsInfinite =  EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;

    // 如果是无限效果，且策略是离开范围时移除，则记录 Handle 以便后续移除
    if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
    {
        ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
    }
}

// ===============================
// 当另一个 Actor 与当前 AuraEffectActor 重叠时调用
// ===============================
void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
    // 如果瞬时效果的应用策略是 "重叠时应用"
    if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
        // 对目标施加瞬时 GameplayEffect（例如瞬间伤害或治疗）
        ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }

    // 如果持续效果的应用策略是 "重叠时应用"
    if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
        // 对目标施加持续 GameplayEffect（有持续时间，例如 5 秒回血）
        ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }

    // 如果无限效果策略是重叠时应用
    if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
        ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
    }
}
// ===============================
// 当另一个 Actor 与当前 AuraEffectActor 结束重叠时调用
// ===============================
void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
    // 瞬时效果策略：结束重叠时应用
    if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
        ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }

    // 持续效果策略：结束重叠时应用
    if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
        ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }

    // 无限效果策略：结束重叠时应用
    if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
        ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
    }

    // 无限效果移除策略：离开重叠时移除
    if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
    {
        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
        if (!IsValid(TargetASC)) return;

        // 找出属于该目标的所有 ActiveEffectHandles
        TArray<FActiveGameplayEffectHandle> HandlesToRemove;
        for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
        {
            if (TargetASC == HandlePair.Value)
            {
                // 移除效果
                TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
                HandlesToRemove.Add(HandlePair.Key);
            }
        }

        // 从记录中删除已移除的 Handle
        for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
        {
            ActiveEffectHandles.FindAndRemoveChecked(Handle);
        }
    }
}