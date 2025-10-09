// Tcohneyn All Rights Reserved


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
    // 尝试通过传入的世界上下文对象获取当前玩家控制器（本地玩家索引默认为0）
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        // 从玩家控制器中获取HUD，并转换为我们自定义的AAuraHUD类型
        if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
        {
            // 从玩家控制器中获取玩家状态（PlayerState）
            AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();

            // 从玩家状态中获取能力系统组件（ASC）
            UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

            // 从玩家状态中获取属性集（AttributeSet）
            UAttributeSet* AS = PS->GetAttributeSet();

            // 构造一个用于初始化Widget Controller的参数结构体
            const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);

            // 调用AuraHUD的Getter函数，获取或构建Overlay Widget Controller实例并返回
            return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
        }
    }

    // 如果任意步骤失败（无法获取控制器、HUD等），返回空指针
    return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
    // 尝试通过传入的世界上下文对象获取当前玩家控制器（本地玩家索引默认为0）
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        // 从玩家控制器中获取HUD，并转换为我们自定义的AAuraHUD类型
        if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
        {
            // 从玩家控制器中获取玩家状态（PlayerState）
            AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();

            // 从玩家状态中获取能力系统组件（ASC）
            UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

            // 从玩家状态中获取属性集（AttributeSet）
            UAttributeSet* AS = PS->GetAttributeSet();

            // 构造一个用于初始化Widget Controller的参数结构体
            const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);

            // 调用AuraHUD的Getter函数，获取或构建Overlay Widget Controller实例并返回
            return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
        }
    }

    // 如果任意步骤失败（无法获取控制器、HUD等），返回空指针
    return nullptr;
}