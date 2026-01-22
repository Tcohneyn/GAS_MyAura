// Tcohneyn All Rights Reserved


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityTypes.h"
#include "Character/AuraCharacterBase.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
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
void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level,
    UAbilitySystemComponent* ASC)
{

    // 2. 获取AbilitySystemComponent的所有者角色（AvatarActor）
    // Avatar是游戏世界中具体代表该ASC的实体[6](@ref)
    AActor* AvatarActor = ASC->GetAvatarActor();

    // 3. 从游戏模式中获取角色职业信息数据资产
    // 这个数据资产应包含不同职业的属性配置[8](@ref)
    UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
    if (!CharacterClassInfo) return;

    // 根据传入的职业枚举，获取该职业的默认属性配置
    FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

    // 4. 应用主要属性（Primary Attributes）的GameplayEffect
    // 创建GameplayEffect的上下文（Context），用于传递来源等信息[6,7](@ref)
    FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
    // 设置属性来源为角色自身[7](@ref)
    PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
    // 创建GameplayEffect规格（Spec），包含效果的具体细节和等级[6,8](@ref)
    const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level,
        PrimaryAttributesContextHandle);
    // 将GameplayEffect应用到自身[6,8](@ref)
    ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

    // 5. 应用次要属性（Secondary Attributes）的GameplayEffect
    // 次要属性通常从主要属性派生，但配置在CharacterClassInfo中而非职业特定信息里[8](@ref)
    FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
    SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
    const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level,
        SecondaryAttributesContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

    // 6. 应用生命属性（Vital Attributes）的GameplayEffect
    // 生命属性通常是生命值、法力值等基础数值，直接配置在CharacterClassInfo中[8](@ref)
    FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
    VitalAttributesContextHandle.AddSourceObject(AvatarActor);
    const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level,
        VitalAttributesContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,
    ECharacterClass CharacterClass)
{
    UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
    if (CharacterClassInfo == nullptr) return;
    for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
        ASC->GiveAbility(AbilitySpec);
    }
    const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
    for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
    {
        if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
        {
            FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
            ASC->GiveAbility(AbilitySpec);
        }
    }
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
    AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    if (AuraGameMode == nullptr) return nullptr;
    return AuraGameMode->CharacterClassInfo;
}


bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
    if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return AuraEffectContext->IsBlockedHit();
    }
    return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
    if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return AuraEffectContext->IsCriticalHit();
    }
    return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
    if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        AuraEffectContext->SetIsBlockedHit(bInIsBlockedHit);
    }
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
    bool bInIsCriticalHit)
{
    if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
    }
}

/**
 * UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius
 * 作用：在指定范围内搜寻所有“实现了战斗接口”且“尚未死亡”的有效角色（Actor）。
 * * @param WorldContextObject   世界上下文对象，用于访问 UWorld
 * @param OutOverlappingActors [输出] 存储筛选后的有效目标 Actor 数组
 * @param ActorsToIgnore       碰撞查询中需要忽略的 Actor（例如施法者本人）
 * @param Radius               检测球体的半径
 * @param SphereOrigin         检测球体的中心坐标（如矛尖位置）
 */
TArray<AActor*> UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
    const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
    TArray<AActor*> OutOverlappingActors;
    // 1. 初始化碰撞查询参数
    FCollisionQueryParams SphereParams;
    if (IsNoAuraCharacterBaseIgnored(ActorsToIgnore)) return OutOverlappingActors;
    // 将忽略列表中的 Actor 加入查询参数，防止检测到施法者自己或其他特定对象
    SphereParams.AddIgnoredActors(ActorsToIgnore);

    // 2. 获取当前游戏世界实例
    // 使用 EGetWorldErrorMode::LogAndReturnNull 确保在获取失败时安全返回且记录日志
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        // 存储物理引擎返回的原始重叠检测结果
        TArray<FOverlapResult> Overlaps;

        // 3. 执行多对象类型重叠检测
        // OverlapMultiByObjectType：根据对象类型（ObjectType）进行范围搜寻
        // FCollisionObjectQueryParams::AllDynamicObjects：检测所有动态对象（包括 Pawn, PhysicsBody 等）
        // FCollisionShape::MakeSphere(Radius)：创建一个球形的物理形状进行检测
        World->OverlapMultiByObjectType(
            Overlaps,
            SphereOrigin,
            FQuat::Identity,
            FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
            FCollisionShape::MakeSphere(Radius),
            SphereParams
            );
        // 4. 遍历所有检测到的原始碰撞结果进行二次逻辑筛选
        for (FOverlapResult& Overlap : Overlaps)
        {
            // 安全检查：先判空
            if (AActor* OverlapActor = Overlap.GetActor())
            {
                // 筛选条件 A：检查检测到的 Actor 是否实现了 UCombatInterface（战斗接口）
                // 筛选条件 B：通过接口调用 Execute_IsDead 检查该角色是否已经死亡（活着才会被选中）
                if (OverlapActor->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(OverlapActor))
                {
                    // 5. 获取该角色的化身（Avatar）并加入结果数组
                    // Execute_GetAvatar：确保获取的是逻辑控制的实体 Actor
                    // AddUnique：确保去重，即使 Actor 有多个碰撞组件重叠，也只在数组中记录一次
                    OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(OverlapActor));
                }
            }
        }
    }
    return OutOverlappingActors;
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
    const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
    const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
    const bool bFriends = bBothArePlayers || bBothAreEnemies;
    return !bFriends;
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass,
    int32 CharacterLevel)
{
    UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
    if (CharacterClassInfo == nullptr) return 0;

    const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
    const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

    return static_cast<int32>(XPReward);
}


bool UAuraAbilitySystemLibrary::IsNoAuraCharacterBaseIgnored(const TArray<AActor*>& ActorsToIgnore)
{
    for (AActor* IgnoredActor : ActorsToIgnore)
    {
        // 关键：首先检查指针有效性！
        if (IsValid(IgnoredActor))
        {
            // 方法1：检查是否为AAuraCharacterBase或其派生类
            if (IgnoredActor->IsA(AAuraCharacterBase::StaticClass()))
            {
                // 发现了AAuraCharacterBase（或其派生类）的实例，不符合“不包含”的条件
                return false;
            }

            // 方法2：更显式地使用GetClass()->IsChildOf()
            // if (IgnoredActor->GetClass()->IsChildOf(AAuraCharacterBase::StaticClass()))
            // {
            //     return false;
            // }
        }
    }
    // 遍历完成，没有找到任何AAuraCharacterBase的实例
    return true;
}
void UAuraAbilitySystemLibrary::ShuffleSpawnLocations(TArray<FVector>& Locations)
{
    if (Locations.Num() > 0)
    {
        const int32 LastIndex = Locations.Num() - 1;
        for (int32 i = 0; i <= LastIndex; ++i)
        {
            int32 Index = FMath::RandRange(i, LastIndex);
            if (i != Index)
            {
                Locations.Swap(i, Index);
            }
        }
    }
}
