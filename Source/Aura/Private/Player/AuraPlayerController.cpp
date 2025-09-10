// Tcohneyn All Rights Reserved


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
    FHitResult CursorHit;
    GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
    if (!CursorHit.bBlockingHit) return;

    LastActor = ThisActor;
    ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

    /**
     * 从鼠标光标位置进行直线检测（Line Trace），会有以下几种情况：
     *  A. LastActor 为空 && ThisActor 为空
     *     - 什么也不做
     *  B. LastActor 为空 && ThisActor 有效
     *     - 高亮 ThisActor
     *  C. LastActor 有效 && ThisActor 为空
     *     - 取消高亮 LastActor
     *  D. 两者都有效，但 LastActor != ThisActor
     *     - 取消高亮 LastActor，并高亮 ThisActor
     *  E. 两者都有效，且是同一个 Actor
     *     - 什么也不做
     */

    if (LastActor == nullptr)
    {
        if (ThisActor != nullptr)
        {
            // Case B
            ThisActor->HighlightActor();
        }
        else
        {
            // Case A - both are null, do nothing
        }
    }
    else // LastActor is valid
    {
        if (ThisActor == nullptr)
        {
            // Case C
            LastActor->UnHighlightActor();
        }
        else // both actors are valid
        {
            if (LastActor != ThisActor)
            {
                // Case D
                LastActor->UnHighlightActor();
                ThisActor->HighlightActor();
            }
            else
            {
                // Case E - do nothing
            }
        }
    }
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
    UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
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