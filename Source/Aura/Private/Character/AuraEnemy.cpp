// Tcohneyn All Rights Reserved


#include "Character/AuraEnemy.h"

#include "AI/AuraAIController.h"
#include "AuraGameplayTagsController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "MotionWarpingComponent.h"

AAuraEnemy::AAuraEnemy()
{
    // 设置角色的网格体 (SkeletalMeshComponent) 对 "可见性通道" 的碰撞响应为 "阻挡"。
    // 这样做的目的通常是让射线检测（比如鼠标悬停、瞄准线 TraceChannel）能够命中敌人。
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComp"));

    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->RotationRate.Yaw = 400.f;

    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
    HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
    HealthBar->SetupAttachment(GetRootComponent());

}

void AAuraEnemy::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (!HasAuthority()) return;
    AuraAIController = Cast<AAuraAIController>(NewController);
    AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
    AuraAIController->RunBehaviorTree(BehaviorTree);
    AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
    AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
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

int32 AAuraEnemy::GetPlayerLevel()
{
    return Level;
}

void AAuraEnemy::Die()
{
    SetLifeSpan(LifeSpan);
    Super::Die();
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
    CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
    return CombatTarget;
}

void AAuraEnemy::BeginPlay()
{
    // 1. 首先调用父类（AActor或ACharacter）的BeginPlay()，确保引擎基础逻辑和组件正确初始化
    Super::BeginPlay();
    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
    // 2. 初始化技能系统角色信息（如AbilitySystemComponent和AttributeSet）
    // 此函数通常用于将当前敌人Actor注册到Gameplay Ability System (GAS) 中，使其能够使用技能和属性[5](@ref)
    InitAbilityActorInfo();

    if (HasAuthority())
    {
        UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
    }

    // 3. 获取并设置健康条用户控件的控制器
    // 假设HealthBar是一个UWidgetComponent或其他承载UI的组件，此行获取其上的用户控件并转换为特定类型
    if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
    {
        // 将当前敌人实例设置为该UI控件的控制器（WidgetController）
        // 这使得UI能够从此敌人实例获取实时数据（如当前生命值）并进行显示[1](@ref)
        AuraUserWidget->SetWidgetController(this);
    }

    // 4. 从AbilitySystemComponent中获取属性集（AttributeSet）并进行安全检查
    if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
    {
        // 4.1 为"当前生命值（Health）"属性注册变化监听委托
        // 当Health属性值发生任何改变时，后面的Lambda表达式将被自动调用[3](@ref)
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data) // Lambda表达式，捕获this指针以访问当前对象成员
            {
                // 当Health变化时，广播新的生命值，从而触发UI更新（例如血条缩短）
                // 任何绑定到此OnHealthChanged委托的UI元素（如血条Progress Bar）都会收到通知[1](@ref)
                OnHealthChanged.Broadcast(Data.NewValue);
            }
            );

        // 4.2 为"最大生命值（MaxHealth）"属性注册变化监听委托
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                // 当MaxHealth变化时（例如受到某种效果影响），广播新的最大值，UI据此调整血条总长度
                OnMaxHealthChanged.Broadcast(Data.NewValue);
            }
            );

        AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).
                                AddUObject(
                                    this,
                                    &AAuraEnemy::HitReactTagChanged
                                    );

        // 5. 初始化UI：主动广播一次当前的生命值和最大生命值
        // 这确保了在游戏一开始，UI就能显示正确的初始状态，而不是空着等待第一次属性变化[1](@ref)
        OnHealthChanged.Broadcast(AuraAS->GetHealth());
        OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
    }
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    bHitReacting = NewCount > 0;
    GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
    if (AuraAIController && AuraAIController->GetBlackboardComponent())
    {
        AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
    }
}

void AAuraEnemy::InitAbilityActorInfo()
{
    Super::InitAbilityActorInfo();
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
        if (HasAuthority())
        {
            InitializeDefaultAttributes();
        }
    }
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
    UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}