#pragma once
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
    GENERATED_BODY()

    FDamageEffectParams(){}

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UObject> WorldContextObject = nullptr;

    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

    UPROPERTY(BlueprintReadWrite)
    float BaseDamage = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float AbilityLevel = 1.f;

    UPROPERTY(BlueprintReadWrite)
    FGameplayTag DamageType = FGameplayTag();

    UPROPERTY(BlueprintReadWrite)
    float DebuffChance = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float DebuffDamage = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float DebuffDuration = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float DebuffFrequency = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float DeathImpulseMagnitude = 0.f;

    UPROPERTY(BlueprintReadWrite)
    FVector DeathImpulse = FVector::ZeroVector;
    
    UPROPERTY(BlueprintReadWrite)
    float KnockbackForceMagnitude = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float KnockbackChance = 0.f;

    UPROPERTY(BlueprintReadWrite)
    FVector KnockbackForce = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct  FAuraGameplayEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()
public:
    bool IsCriticalHit() const { return bIsCriticalHit; }
    bool IsBlockedHit () const { return bIsBlockedHit; }
    bool IsSuccessfulDebuff() const { return bIsSuccessfulDebuff; }
    float GetDebuffDamage() const { return DebuffDamage; }
    float GetDebuffDuration() const { return DebuffDuration; }
    float GetDebuffFrequency() const { return DebuffFrequency; }
    TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }
    FVector GetDeathImpulse() const { return DeathImpulse; }
    FVector GetKnockbackForce() const { return KnockbackForce; }
    
    void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
    void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
    void SetIsSuccessfulDebuff(bool bInIsDebuff) { bIsSuccessfulDebuff = bInIsDebuff; }
    void SetDebuffDamage(float InDamage) { DebuffDamage = InDamage; }
    void SetDebuffDuration(float InDuration) { DebuffDuration = InDuration; }
    void SetDebuffFrequency(float InFrequency) { DebuffFrequency = InFrequency; }
    void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }
    void SetDeathImpulse(const FVector& InImpulse) { DeathImpulse = InImpulse; }
    void SetKnockbackForce(const FVector& InForce) { KnockbackForce = InForce; }
    /**
     * 返回用于序列化的实际结构体，子类必须重写此方法！
     * 此方法定义了在网络复制和序列化时使用的数据结构类型。
     * 子类需要返回自定义的结构体类型以确保正确的序列化行为。
     */
    virtual UScriptStruct* GetScriptStruct() const
    {
        return FGameplayEffectContext::StaticStruct();
    }


    /** 创建此上下文的拷贝，用于复制以便后续修改 */
    virtual FGameplayEffectContext* Duplicate() const
    {
        FGameplayEffectContext* NewContext = new FGameplayEffectContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
    
    /**
     * 自定义网络序列化，子类必须重写此方法
     * 实现自定义数据的网络同步逻辑，确保客户端和服务器数据一致。
     * @param Ar 序列化存档（输入/输出）
     * @param Map 包映射表，用于对象引用解析
     * @param bOutSuccess 输出参数，指示序列化是否成功
     * @return 序列化是否完成
     */
    virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
protected:
    UPROPERTY()
    bool bIsBlockedHit = false;
	
    UPROPERTY()
    bool bIsCriticalHit = false;

    UPROPERTY()
    bool bIsSuccessfulDebuff = false;

    UPROPERTY()
    float DebuffDamage = 0.f;

    UPROPERTY()
    float DebuffDuration = 0.f;

    UPROPERTY()
    float DebuffFrequency = 0.f;

    TSharedPtr<FGameplayTag> DamageType;
    
    UPROPERTY()
    FVector DeathImpulse = FVector::ZeroVector;

    UPROPERTY()
    FVector KnockbackForce = FVector::ZeroVector;
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
    enum
    {
        WithNetSerializer = true,
        WithCopy = true
    };
};