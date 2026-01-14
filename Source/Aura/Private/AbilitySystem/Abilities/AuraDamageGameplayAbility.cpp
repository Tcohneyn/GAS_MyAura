// Tcohneyn All Rights Reserved


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

/**
 * UAuraDamageGameplayAbility::CauseDamage
 * 作用：计算伤害数值并将其应用到目标 Actor 身上。
 * @param TargetActor 受到伤害的目标对象
 */
void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
    // 1. 创建一个外发的 Gameplay Effect Spec (规格句柄)
    // DamageEffectClass: 在蓝图中设置的 GE 类（通常是瞬时伤害效果）
    // 1.f: 这里暂时设置基础等级为 1，具体的伤害缩放会在后面通过 SetByCaller 处理
    FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);

    // 2. 遍历该技能定义的所有伤害类型（例如：物理、火、冰等）
    // DamageTypes 是一个 TMap<FGameplayTag, FScalableFloat>
    for (TTuple<FGameplayTag, FScalableFloat> Pair : DamageTypes)
    {
        // 3. 根据当前技能等级（GetAbilityLevel）从曲线表中获取对应的伤害数值
        // 例如：1级时伤害是 5，40级时通过曲线计算可能是 50
        const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());

        // 4. 使用 SetByCaller 机制将计算后的数值注入到 Spec 中
        // Pair.Key: 伤害类型的标签（如 Damage.Physical）
        // ScaledDamage: 刚才算出的具体数值
        // 这样后续的 ExecCalc_Damage (伤害计算类) 就能通过这些标签读取到具体伤害
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
    }

    // 5. 将配置好的伤害效果应用到目标身上
    // GetAbilitySystemComponentFromActorInfo(): 获取施法者（自己）的 ASC
    // *DamageSpecHandle.Data.Get(): 获取 Spec 的指针并解引用，获取内部数据
    // UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor): 获取受击者的 ASC
    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
        *DamageSpecHandle.Data.Get(), 
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor)
    );
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
    if (TaggedMontages.Num() > 0)
    {
        const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
        return TaggedMontages[Selection];
    }

    return FTaggedMontage();
}
