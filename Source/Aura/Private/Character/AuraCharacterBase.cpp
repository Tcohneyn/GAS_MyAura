// Tcohneyn All Rights Reserved


#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Engine/ContentEncryptionConfig.h"

AAuraCharacterBase::AAuraCharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;
    
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetCapsuleComponent()->SetGenerateOverlapEvents(false);
    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    // 设置网格体与“抛射物”碰撞通道的交互方式为“重叠”
    // 目的：当一颗子弹（其碰撞体被设置为ECC_Projectile通道）飞向这个角色时，我们希望子弹能穿透网格体（而不是被物理弹开），
    //       但同时游戏逻辑要能感知到这次“击中”，以便计算伤害、播放音效等。
    GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
    // 启用网格体的重叠事件生成功能
    // 目的：这是触发重叠事件的必要前提。只有打开这个开关，当重叠实际发生时，引擎才会去检测并触发相应的回调函数（如OnComponentBeginOverlap）。
    //       如果没有这行代码，即使上一行设置为Overlap，也不会有任何事件被触发。
    GetMesh()->SetGenerateOverlapEvents(true);
    
    Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
    Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

   BodyDissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("BodyDissolveTimeline"));
   WeaponDissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("WeaponDissolveTimeline"));
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
    return HitReactMontage;
}

void AAuraCharacterBase::Die()
{
    Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
    MulticastHandleDeath();
}

// 这是一个在服务器端调用，但在所有客户端上执行的远程过程调用(RPC)函数
// 用于处理角色死亡时的物理效果和碰撞设置
void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
    // 设置武器的物理模拟
    Weapon->SetSimulatePhysics(true);        // 启用武器的物理模拟，使其受重力影响
    Weapon->SetEnableGravity(true);          // 启用武器的重力效果
    Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly); // 只启用物理碰撞

    // 设置角色网格体的物理模拟
    GetMesh()->SetSimulatePhysics(true);     // 启用角色网格体的物理模拟
    GetMesh()->SetEnableGravity(true);       // 启用角色网格体的重力效果
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly); // 只启用物理碰撞
    GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 设置对静态物体的碰撞响应为阻挡

    // 禁用胶囊体组件的碰撞
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 完全禁用胶囊体碰撞
    Dissolve();
}

void AAuraCharacterBase::BeginPlay()
{
    Super::BeginPlay();

}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
    check(Weapon);
    return Weapon->GetSocketLocation(WeaponTipSocketName);
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

void AAuraCharacterBase::AddCharacterAbilities()
{
    UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
    if (!HasAuthority()) return; //检查是否拥有网络权限

    AuraASC->AddCharacterAbilities(StartupAbilities);
}

void AAuraCharacterBase::Dissolve()
{
    if (IsValid(DissolveMaterialInstance))
    {
        UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
        GetMesh()->SetMaterial(0, DynamicMatInst);
        StartDissolveTimeline(DynamicMatInst);
    }
    if (IsValid(WeaponDissolveMaterialInstance))
    {
        UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
        Weapon->SetMaterial(0, DynamicMatInst);
        StartWeaponDissolveTimeline(DynamicMatInst);
    }
}