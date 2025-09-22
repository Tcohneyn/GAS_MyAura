// Tcohneyn All Rights Reserved


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
    // 设置角色的网格体 (SkeletalMeshComponent) 对 "可见性通道" 的碰撞响应为 "阻挡"。
    // 这样做的目的通常是让射线检测（比如鼠标悬停、瞄准线 TraceChannel）能够命中敌人。
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraEnemy::HighlightActor()
{
    // 启用网格体的自定义深度渲染。
    // 自定义深度渲染通常配合后处理材质，用于实现轮廓描边效果（Highlight/Outline）。
    GetMesh()->SetRenderCustomDepth(true);
    // 设置网格体的自定义深度模板值为 "红色高亮"。
    GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
    // 同样处理敌人的武器，使武器也会显示高亮描边。
    Weapon->SetRenderCustomDepth(true);
    Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
    // 关闭网格体的自定义深度渲染，高亮效果消失。
    GetMesh()->SetRenderCustomDepth(false);
    // 同样关闭武器的自定义深度渲染。
    Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::BeginPlay()
{
    Super::BeginPlay();
    InitAbilityActorInfo();
}

void AAuraEnemy::InitAbilityActorInfo()
{
    Super::InitAbilityActorInfo();
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
    }
}