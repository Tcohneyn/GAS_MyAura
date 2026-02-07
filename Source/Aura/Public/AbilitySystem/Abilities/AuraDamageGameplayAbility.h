// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    void CauseDamage(AActor* TargetActor);
   //工厂函数：自动读取当前能力的配置（如伤害类型、等级、减益默认值）并填充到 `FDamageEffectParams` 结构体中
    UFUNCTION(BlueprintPure)
    FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;
protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta = (DisplayName = "伤害效果类"))
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, Category = "伤害",meta = (DisplayName = "伤害类型"))
    FGameplayTag DamageType;

    UPROPERTY(EditDefaultsOnly, Category = "伤害",meta = (DisplayName = "伤害"))
    FScalableFloat Damage;

    UPROPERTY(EditDefaultsOnly, Category = "伤害",meta = (DisplayName = "负面效果机率"))
    float DebuffChance = 20.f;

    UPROPERTY(EditDefaultsOnly, Category = "伤害",meta = (DisplayName = "负面效果伤害"))
    float DebuffDamage = 5.f;

    UPROPERTY(EditDefaultsOnly, Category = "伤害",meta = (DisplayName = "负面效果触发频率"))
    float DebuffFrequency = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "伤害",meta = (DisplayName = "负面效果持续时间"))
    float DebuffDuration = 5.f;

    UPROPERTY(EditDefaultsOnly, Category = "伤害", meta = (DisplayName = "死亡冲力强度"))
    float DeathImpulseMagnitude = 1000.f;

    UPROPERTY(EditDefaultsOnly, Category = "伤害", meta = (DisplayName = "击退力的强度"))
    float KnockbackForceMagnitude = 1000.f;

    UPROPERTY(EditDefaultsOnly, Category = "伤害", meta = (DisplayName = "击退机率"))
    float KnockbackChance = 0.f;
    UFUNCTION(BlueprintPure)
    FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;
    
};
