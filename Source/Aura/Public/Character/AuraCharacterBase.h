// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UTimelineComponent;
class UGameplayAbility;
class UAttributeSet;
class UAuraAbilitySystemComponent;
class USkeletalMeshComponent;
class UGameplayEffect;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter,public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UAttributeSet* GetAttributeSet() const {return AttributeSet;}
    /** Combat Interface */
    virtual UAnimMontage* GetHitReactMontage_Implementation() override;
    virtual void Die() override;
    virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
    virtual bool IsDead_Implementation() const override;
    virtual AActor* GetAvatar_Implementation() override;
    virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
    /** end Combat Interface */
    UFUNCTION(NetMulticast, Reliable)
    virtual void MulticastHandleDeath();

    UPROPERTY(EditAnywhere,Category="Combat")
    TArray<FTaggedMontage> AttackMontages;
protected:
	virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly,Category="Combat")
    TObjectPtr<USkeletalMeshComponent> Weapon;

    UPROPERTY(EditAnywhere, Category = "Combat")
    FName WeaponTipSocketName;

    UPROPERTY(EditAnywhere, Category = "Combat")
    FName LeftHandSocketName;

    UPROPERTY(EditAnywhere, Category = "Combat")
    FName RightHandSocketName;
    
    bool bDead = false;
    UPROPERTY()
    
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UAttributeSet> AttributeSet;

    virtual void InitAbilityActorInfo();
    
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
    TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
    TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
    TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
    TSubclassOf<UGameplayEffect> DefaultResistanceAttributes;
    
    void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
    virtual void InitializeDefaultAttributes() const;

    void AddCharacterAbilities();

    /* 溶解效果 */
#pragma region DissolveCurve
    UPROPERTY(EditDefaultsOnly, Category = "Dissolve")
    UCurveFloat* DissolveCurve;  // 控制溶解程度的曲线（0~1）
    
    UPROPERTY(BlueprintReadOnly, Category = "Dissolve")
    UTimelineComponent* BodyDissolveTimeline;
    
    UPROPERTY(BlueprintReadOnly, Category = "Dissolve")
    UTimelineComponent* WeaponDissolveTimeline;
    
    UPROPERTY(BlueprintReadOnly, Category = "Dissolve")
    FOnTimelineFloat BodyDissolveUpdate;
    
    UPROPERTY(BlueprintReadOnly, Category = "Dissolve")
    FOnTimelineFloat WeaponDissolveUpdate;
    void Dissolve();

    UFUNCTION(BlueprintImplementableEvent)
    void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

    UFUNCTION(BlueprintImplementableEvent)
    void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;
#pragma endregion

private:

    UPROPERTY(EditAnywhere, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

    UPROPERTY(EditAnywhere, Category = "Combat")
    TObjectPtr<UAnimMontage> HitReactMontage;
};
