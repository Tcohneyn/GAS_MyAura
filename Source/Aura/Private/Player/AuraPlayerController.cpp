// Tcohneyn All Rights Reserved


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTagsController.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;
    Spline = CreateDefaultSubobject<USplineComponent>("SplineComponent");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    CursorTrace();
    AutoRun();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount ,ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
    if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
    {
        UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter,DamageTextComponentClass);
        DamageText->RegisterComponent();
        DamageText->AttachToComponent(TargetCharacter->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
        DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
    }
}

// 自动寻路功能的核心函数
void AAuraPlayerController::AutoRun()
{
    // 检查是否处于自动寻路状态，如果不是则直接返回
    if (!bAutoRunning) return;
    
    // 获取当前控制的角色（Pawn）
    if (APawn* ControlledPawn = GetPawn())
    {
        // 在样条线上找到离角色当前位置最近的点
        const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
            ControlledPawn->GetActorLocation(), 
            ESplineCoordinateSpace::World
        );
        
        // 获取样条线在该点处的切线方向（即移动方向）
        const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
            LocationOnSpline, 
            ESplineCoordinateSpace::World
        );
        
        // 向角色添加移动输入，使其沿样条线方向移动
        ControlledPawn->AddMovementInput(Direction);

        // 计算当前在样条线上的位置与目标位置之间的距离
        const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
        
        // 如果距离小于等于自动寻路的接受半径，则停止自动寻路
        if (DistanceToDestination <= AutoRunAcceptanceRadius)
        {
            bAutoRunning = false;
        }
    }
}
void AAuraPlayerController::CursorTrace()
{
    GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
    if (!CursorHit.bBlockingHit) return;

    LastActor = ThisActor;
    ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

    if (LastActor != ThisActor)
    {
        if (LastActor) LastActor->UnHighlightActor();
        if (ThisActor) ThisActor->HighlightActor();
    }
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    //GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
    if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        bTargeting = ThisActor ? true : false;
        bAutoRunning = false;
    }
}

// 处理技能输入按键释放事件
void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    // 检查释放的输入标签是否为鼠标左键（LMB）
    if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        // 如果不是鼠标左键，直接传递给技能系统组件处理技能释放逻辑
        if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
        return; // 对于非LMB输入，处理到此结束
    }


    // 将释放事件传递给技能系统，用于释放目标型技能（如攻击锁定目标）
    if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);

    if (!bTargeting && !bShiftKeyDown)
    {
        // 获取控制的Pawn
        const APawn* ControlledPawn = GetPawn();
        
        // 检查是否为短按（按压时间小于等于短按阈值）且Pawn有效
        if (FollowTime <= ShortPressThreshold && ControlledPawn)
        {
            // 使用导航系统查找从当前位置到缓存目的地的路径
            if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
                this, ControlledPawn->GetActorLocation(), CachedDestination))
            {
                // 清空样条线上的所有点
                Spline->ClearSplinePoints();
                
                // 将导航路径上的所有点添加到样条线中
                for (const FVector& PointLoc : NavPath->PathPoints)
                {
                    Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
                    // 绘制调试球体，显示路径点（绿色，持续5秒）
                    //DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f);
                }
                CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
                
                // 设置自动寻路状态为true
                bAutoRunning = true;
            }
        }
        
        // 重置按压时间
        FollowTime = 0.f;
        // 重置目标锁定状态
        bTargeting = false;
    }
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
        return;
    }

    if (bTargeting || bShiftKeyDown)
    {
        if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
    }
    else
    {
        // 累计按住时间，用于区分点击和长按
        FollowTime += GetWorld()->GetDeltaSeconds();

        // 检查光标射线检测是否发生了有效阻挡（即是否击中了场景中的物体）
        if (CursorHit.bBlockingHit) 
            // 如果发生了阻挡，将缓存的目标点设置为射线击中点的精确位置
                CachedDestination = CursorHit.ImpactPoint;

        // 获取控制的Pawn并执行移动
        if (APawn* ControlledPawn = GetPawn())
        {
            // 计算从角色位置到目标位置的方向向量（归一化）
            const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
            // 添加移动输入
            ControlledPawn->AddMovementInput(WorldDirection);
        }
    }
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
    if (AuraAbilitySystemComponent == nullptr)
    {
        AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
    }
    return AuraAbilitySystemComponent;
}


void AAuraPlayerController::BeginPlay()
{
    Super::BeginPlay();
    check(AuraContext);
    // 获取本地玩家
    ULocalPlayer* LocalPlayer = GetLocalPlayer();

    // 获取增强输入本地玩家子系统
    auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    //check(Subsystem);
    if (Subsystem)
    {
        Subsystem->AddMappingContext(AuraContext, 0);
    }

    // 显示鼠标光标（true 表示显示，false 表示隐藏）
    bShowMouseCursor = true;

    // 设置默认鼠标指针样式，这里是系统默认箭头形状
    DefaultMouseCursor = EMouseCursor::Default;

    // 创建一个“游戏 + UI 混合输入模式”配置对象
    // 在该模式下，既可以接收游戏输入（如键盘移动），又可以与 UI 交互（如点击按钮）
    FInputModeGameAndUI InputModeData;

    // 设置鼠标锁定模式为“不锁定”
    // DoNotLock：鼠标可以自由移出视口，适合需要切到其他窗口的情况
    // LockOnCapture：在捕获鼠标后锁定在视口内
    // LockAlways：始终锁定鼠标在视口中
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    // 设置在捕获输入时是否隐藏鼠标光标
    // false 表示捕获时光标仍可见，true 表示捕获时隐藏光标
    InputModeData.SetHideCursorDuringCapture(false);

    // 将配置好的输入模式应用到当前 PlayerController
    // 这样就能在游戏中同时使用 UI 和游戏输入
    SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
    AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
    AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
    AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
    AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
    const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
    const FRotator Rotator = GetControlRotation();
    const FRotator YawRotation(0.0, Rotator.Yaw, 0.0);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    if (APawn* ControlledPawn = GetPawn<APawn>())
    {
        // 沿前向方向移动（Y值控制前后）
        ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
        // 沿右向方向移动（X值控制左右）
        ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
    }
}