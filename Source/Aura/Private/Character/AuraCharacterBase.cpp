// Tcohneyn All Rights Reserved


#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Engine/ContentEncryptionConfig.h"

AAuraCharacterBase::AAuraCharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;
    Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
    Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AAuraCharacterBase::BeginPlay()
{
    Super::BeginPlay();

}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}
//初始化主要属性
void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
    check(IsValid(GetAbilitySystemComponent()));
    check(GameplayEffectClass);
    // 创建 GameplayEffect 的上下文（Context），
    // 用来描述施加者、来源对象等信息（这里施加者就是角色自己）
    FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    ContextHandle.AddSourceObject(this);  // 关键：将当前角色对象(this)设置为效果来源对象
    // 基于  GameplayEffectClass 类生成一个 GameplayEffectSpec（效果说明书）
    const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
    // 将这个 GameplayEffectSpec 应用到目标（这里目标是自己）。
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
    //主要属性
    ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
    //派生属性
    ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
    //核心属性
    ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}