// Tcohneyn All Rights Reserved


#include "Character/AuraCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
AAuraCharacter::AAuraCharacter()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    CameraBoom->SetupAttachment(RootComponent);
    // 使用绝对旋转（不跟随父组件旋转）
    // 这样可以固定相机臂的方向，而不是随着角色旋转而旋转
    CameraBoom->SetUsingAbsoluteRotation(true);
    CameraBoom->TargetArmLength = 750.0f;
    CameraBoom->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
    // 启用相机平滑延迟（相机跟随时有缓动效果）
    CameraBoom->bEnableCameraLag = true;

    // 禁止继承角色的 Pitch（俯仰）旋转
    CameraBoom->bInheritPitch = false;

    // 禁止继承角色的 Roll（横滚）旋转
    CameraBoom->bInheritRoll = false;

    // 禁止继承角色的 Yaw（偏航）旋转
    CameraBoom->bInheritYaw = false;

    // 禁用相机碰撞检测（相机不会因为障碍物自动缩近）
    // 适用于俯视游戏，否则相机会被卡近
    CameraBoom->bDoCollisionTest = false;
    
    FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
    
    // -------------------- 角色移动设置 --------------------

    // 让角色朝向移动方向旋转（而不是朝向控制器方向）
    // 常见于 Top-Down（俯视）或第三人称 RPG
    GetCharacterMovement()->bOrientRotationToMovement = true;

    // 设置角色旋转速率（Yaw 旋转 400°/秒，Pitch 和 Roll 不变）
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);

    // 让角色移动时受限于一个平面（适合 2D/俯视游戏）
    GetCharacterMovement()->bConstrainToPlane = true;

    // 开始游戏时将角色自动贴到平面上
    GetCharacterMovement()->bSnapToPlaneAtStart = true;


    // -------------------- 控制器旋转设置 --------------------

    // 不使用控制器的 Pitch（防止上下看影响角色旋转）
    bUseControllerRotationPitch = false;

    // 不使用控制器的 Roll（防止倾斜）
    bUseControllerRotationRoll = false;

    // 不使用控制器的 Yaw（防止左右看影响角色旋转）
    bUseControllerRotationYaw = false;

    
}