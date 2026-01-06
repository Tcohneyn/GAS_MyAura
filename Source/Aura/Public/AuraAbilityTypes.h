#pragma once
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct  FAuraGameplayEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()
public:
    bool IsCriticalHit() const { return bIsCriticalHit; }
    bool IsBlockedHit () const { return bIsBlockedHit; }

    void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
    void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
    
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