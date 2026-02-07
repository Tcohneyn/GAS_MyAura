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
    
    const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ScaledDamage);

    // 5. 将配置好的伤害效果应用到目标身上
    // GetAbilitySystemComponentFromActorInfo(): 获取施法者（自己）的 ASC
    // *DamageSpecHandle.Data.Get(): 获取 Spec 的指针并解引用，获取内部数据
    // UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor): 获取受击者的 ASC
    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
        *DamageSpecHandle.Data.Get(), 
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor)
    );
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
    FDamageEffectParams Params;
    Params.WorldContextObject = GetAvatarActorFromActorInfo();
    Params.DamageGameplayEffectClass = DamageEffectClass;
    Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
    Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
    Params.AbilityLevel = GetAbilityLevel();
    Params.DamageType = DamageType;
    Params.DebuffChance = DebuffChance;
    Params.DebuffDamage = DebuffDamage;
    Params.DebuffDuration = DebuffDuration;
    Params.DebuffFrequency = DebuffFrequency;
    Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
    Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
    Params.KnockbackChance = KnockbackChance;
    if (IsValid(TargetActor))
    {
        FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
        Rotation.Pitch = 45.f;
        const FVector ToTarget = Rotation.Vector();
        Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
        Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
    }
    return Params;
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

