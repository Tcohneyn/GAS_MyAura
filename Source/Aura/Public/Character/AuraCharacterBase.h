// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraCharacterBase.generated.h"

class UDebuffNiagaraComponent;
class UNiagaraSystem;
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
    virtual void Die(const FVector& DeathImpulse) override;	
    virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
    virtual bool IsDead_Implementation() const override;
    virtual AActor* GetAvatar_Implementation() override;
    virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
    virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
    virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
    virtual int32 GetMinionCount_Implementation() override;
    virtual void IncremenetMinionCount_Implementation(int32 Amount) override;
    virtual ECharacterClass GetCharacterClass_Implementation() override;
    virtual FOnASCRegistered GetOnASCRegisteredDelegate() override;
    /** end Combat Interface */

    FOnASCRegistered OnAscRegistered;
    
    UFUNCTION(NetMulticast, Reliable)
    virtual void MulticastHandleDeath(const FVector& DeathImpulse);

    UPROPERTY(EditAnywhere,Category="Combat")
    TArray<FTaggedMontage> AttackMontages;
protected:
	virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="Combat")
    TObjectPtr<USkeletalMeshComponent> Weapon;

    UPROPERTY(EditAnywhere, Category = "Combat")
    FName WeaponTipSocketName;

    UPROPERTY(EditAnywhere, Category = "Combat")
    FName LeftHandSocketName;

    UPROPERTY(EditAnywhere, Category = "Combat")
    FName RightHandSocketName;

    UPROPERTY(EditAnywhere, Category = "Combat")
    FName TailSocketName;
    
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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    UNiagaraSystem* BloodEffect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    USoundBase* DeathSound;
    /* Minions */
	
    int32 MinionCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
    ECharacterClass CharacterClass = ECharacterClass::Warrior;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;
private:

    UPROPERTY(EditAnywhere, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

    UPROPERTY(EditAnywhere, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;
    
    UPROPERTY(EditAnywhere, Category = "Combat")
    TObjectPtr<UAnimMontage> HitReactMontage;
};
