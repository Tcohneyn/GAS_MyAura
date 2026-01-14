// Tcohneyn All Rights Reserved


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraProjectile::AAuraProjectile()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    
    Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
    SetRootComponent(Sphere);
    Sphere->SetCollisionObjectType(ECC_Projectile);
    Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
    Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovement->InitialSpeed = 550.f;
    ProjectileMovement->MaxSpeed = 550.f;
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::BeginPlay()
{
    Super::BeginPlay();
    SetLifeSpan(LifeSpan);
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
    
    LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAuraProjectile::Destroyed()
{
    if (!bHit && !HasAuthority()&&LoopingSoundComponent)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
        if (LoopingSoundComponent) LoopingSoundComponent->Stop();
    }
    Super::Destroyed();
}

// 投射物重叠事件处理函数（通常在碰撞体组件上绑定）
// 此函数在投射物的碰撞体与任何其他Actor的重叠事件发生时自动调用
void AAuraProjectile::OnSphereOverlap(
    UPrimitiveComponent* OverlappedComponent, // 产生重叠的自身碰撞体组件（例如一个球形碰撞体）
    AActor* OtherActor,                       // 与投射物发生重叠的另一个Actor（例如角色、墙壁）
    UPrimitiveComponent* OtherComp,           // 另一个Actor上具体的原始组件（例如其网格体）
    int32 OtherBodyIndex,                     // 其他组件的Body索引（用于复杂网格的特定部分）
    bool bFromSweep,                          // 是否来自扫描检测（true）或简单重叠检测（false）
    const FHitResult& SweepResult)           // 扫描检测的详细命中结果（包含位置、法线等信息）
{
    if (!DamageEffectSpecHandle.Data.IsValid() || DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor)
    {
        return;
    }
    if (!UAuraAbilitySystemLibrary::IsNotFriend(DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser(), OtherActor))
    {
        return;
    }
    if (!bHit)
    {
    // 1. 在碰撞位置播放冲击音效
    // 使用UGameplayStatics工具类在世界的碰撞点播放一个一次性的声音
    UGameplayStatics::PlaySoundAtLocation(
        this,                    // 世界上下文对象（通常传this）
        ImpactSound,             // 要播放的音效资源
        GetActorLocation(),      // 音效播放的世界坐标（投射物当前位置）
        FRotator::ZeroRotator    // 音效的旋转（通常为零旋转）
    );

    // 2. 在碰撞位置生成视觉冲击特效（如火花、烟雾粒子效果）
    // 使用Niagara系统（UE4/UE5的下一代特效系统）在碰撞点生成一个特效
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        this,                    // 世界上下文对象
        ImpactEffect,            // 要生成的Niagara粒子系统资源
        GetActorLocation()       // 特效生成的世界坐标
    );

    // 3. 停止投射物自身的循环音效（如飞行呼啸声）
    // 假设LoopingSoundComponent是一个指向UAudioComponent的指针，用于播放循环飞行音效
    if (LoopingSoundComponent) LoopingSoundComponent->Stop();
    }
    // 4. 网络游戏中的权威判断：检查当前实例是否在服务器上运行
    if (HasAuthority()) // 仅在服务器端执行销毁逻辑
    {
        if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
        {
            TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
        }
        // 服务器端：直接销毁投射物Actor，销毁操作会通过网络同步到所有客户端
        Destroy();
    }
    else
    {
        // 客户端：不直接销毁，而是设置一个标记bHit为true
        // 这个标记可能用于：
        //   - 在Tick函数中延迟销毁
        //   - 触发客户端的其他视觉/逻辑效果
        //   - 等待服务器确认后再进行销毁（避免预测错误）
        bHit = true;
    }
}

