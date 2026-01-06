// Tcohneyn All Rights Reserved


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTagsController.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

// 投射物技能的核心激活函数
void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, 
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
    // 首先调用父类的ActivateAbility，确保基础能力初始化逻辑得以执行
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
    void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
    {
    // 检查当前是否在服务器端运行（权威端）
    // 在多人游戏中，关键逻辑（如生成投射物）必须在服务器上执行以保证同步
    const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
    if (!bIsServer) return; // 如果不是服务器，直接返回（客户端不执行生成逻辑）

    // 尝试将技能的拥有者（AvatarActor）转换为战斗接口
    // 目的是获取生成投射物所需的具体位置信息（如角色的武器挂点）
    ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
    if (CombatInterface)
    {
        // 通过战斗接口获取特定的骨骼套接字位置（例如右手或武器尖端）
        // 这个位置将作为投射物的生成起点
        const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
        FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
        
        // 创建投射物的初始变换（Transform）
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SocketLocation); // 设置生成位置
        SpawnTransform.SetRotation(Rotation.Quaternion());

        // 使用延迟生成方式创建投射物Actor
        // 延迟生成允许我们在投射物完全初始化之前配置其属性，避免不必要的中间状态或重复初始化[6,7](@ref)
        AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
            ProjectileClass,       // 要生成的投射物蓝图类
            SpawnTransform,        // 初始变换（位置）
            GetOwningActorFromActorInfo(), // 设置拥有者（Owner），通常是释放技能的角色
            Cast<APawn>(GetOwningActorFromActorInfo()), // 设置控制者（Instigator），用于归属判断和伤害来源识别
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn // 碰撞处理策略：始终生成，即使有碰撞也会尝试调整位置
        );

        const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
        FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
        EffectContextHandle.SetAbility(this);
        EffectContextHandle.AddSourceObject(Projectile);
        TArray<TWeakObjectPtr<AActor>> Actors;
        Actors.Add(Projectile);
        EffectContextHandle.AddActors(Actors);
        FHitResult HitResult;
        HitResult.Location = ProjectileTargetLocation;
        EffectContextHandle.AddHitResult(HitResult);
        const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
        
        FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
        // 为游戏效果规格（Gameplay Effect Spec）设置一个基于调用者（Set by Caller）的数值
        // 这个函数的作用是：将一个浮点数值（50.f）与一个特定的游戏标签（GameplayTags.Damage）关联起来，并绑定到这次要应用的游戏效果上

        //GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,FString::Printf(TEXT("Damage: %f"),ScaledDamage));
        for (auto& Pair : DamageTypes)
        {
            const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
        }
        
        Projectile->DamageEffectSpecHandle = SpecHandle;

        // 完成投射物的生成过程
        // 此调用会正式将投射物注册到世界场景中，并执行其完整的初始化流程（如蓝图构造脚本）[7](@ref)
        Projectile->FinishSpawning(SpawnTransform);
    }
}