// Tcohneyn All Rights Reserved


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Game/AuraGameModeBase.h"
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

// 函数：初始化默认属性
// 作用：为一个角色应用其职业对应的初始属性（主要属性、次要属性、生命属性）
// 参数：
//   - WorldContextObject: 用于获取游戏世界和游戏模式的对象
//   - CharacterClass: 角色职业枚举，决定应用哪套属性
//   - Level: 角色等级，用于属性缩放
//   - ASC: 目标角色的AbilitySystemComponent，用于应用GameplayEffect
void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
    // 1. 获取游戏模式并转换为项目特定的AuraGameModeBase
    // 游戏模式通常持有全局配置数据，如角色职业信息[8](@ref)
    AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    // 安全检查：如果获取游戏模式失败，则直接返回
    if (AuraGameMode == nullptr) return;

    // 2. 获取AbilitySystemComponent的所有者角色（AvatarActor）
    // Avatar是游戏世界中具体代表该ASC的实体[6](@ref)
    AActor* AvatarActor = ASC->GetAvatarActor();

    // 3. 从游戏模式中获取角色职业信息数据资产
    // 这个数据资产应包含不同职业的属性配置[8](@ref)
    UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;
    // 根据传入的职业枚举，获取该职业的默认属性配置
    FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

    // 4. 应用主要属性（Primary Attributes）的GameplayEffect
    // 创建GameplayEffect的上下文（Context），用于传递来源等信息[6,7](@ref)
    FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
    // 设置属性来源为角色自身[7](@ref)
    PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
    // 创建GameplayEffect规格（Spec），包含效果的具体细节和等级[6,8](@ref)
    const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
    // 将GameplayEffect应用到自身[6,8](@ref)
    ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

    // 5. 应用次要属性（Secondary Attributes）的GameplayEffect
    // 次要属性通常从主要属性派生，但配置在CharacterClassInfo中而非职业特定信息里[8](@ref)
    FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
    SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
    const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

    // 6. 应用生命属性（Vital Attributes）的GameplayEffect
    // 生命属性通常是生命值、法力值等基础数值，直接配置在CharacterClassInfo中[8](@ref)
    FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
    VitalAttributesContextHandle.AddSourceObject(AvatarActor);
    const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}