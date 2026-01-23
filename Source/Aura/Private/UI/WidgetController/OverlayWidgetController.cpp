// Tcohneyn All Rights Reserved


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"
// 用于将初始属性值广播给 UI
void UOverlayWidgetController::BroadcastInitialValues()
{
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

    OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
    OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
    OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
    OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

    //WidgetController 绑定 EffectAssetTagsDelegate
    Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
        [this](const FGameplayTagContainer& AssetTags)
        {
            for (const FGameplayTag& Tag : AssetTags)
            {
                // 例如，假设 Tag = Message.HealthPotion MatchesTag 检查标签是否是另一个标签的子级或完全相等，支持层级匹配。
                // "Message.HealthPotion".MatchesTag("Message") 将返回 True
                // "Message".MatchesTag("Message.HealthPotion") 将返回 False
                FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
                if (Tag.MatchesTag(MessageTag))
                {
                    const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
                    MessageWidgetRowDelegate.Broadcast(*Row);
                }
            }
        }
        );
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
    AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
    AuraPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
    AuraPlayerState->OnLevelChangedDelegate.AddLambda(
        [this](int32 NewLevel)
        {
            OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
        }
        );
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

    // 绑定生命值变化回调：
    // 从 AbilitySystemComponent 获取 "Health 属性" 的 Delegate
    // 并将本类的 HealthChanged 函数绑定到该 Delegate 上
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnHealthChanged.Broadcast(Data.NewValue);
        }
        );
    // 绑定最大生命值变化回调
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnMaxHealthChanged.Broadcast(Data.NewValue);
        }
        );
    // 绑定魔力值变化回调：
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnManaChanged.Broadcast(Data.NewValue);
        }
        );
    // 绑定最大魔力值变化回调
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnMaxManaChanged.Broadcast(Data.NewValue);
        }
        );
    if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
    {
        if (AuraASC->bStartupAbilitiesGiven)
        {
            OnInitializeStartupAbilities(AuraASC);
        }
        else
        {
            AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
        }

        AuraASC->EffectAssetTags.AddLambda(
            [this](const FGameplayTagContainer& AssetTags)
            {
                for (const FGameplayTag& Tag : AssetTags)
                {

                    // 举例来说，假设 Tag = Message.HealthPotion（消息.治疗药水）
                    // "Message.HealthPotion".MatchesTag("Message") 将返回 True，
                    // 而 "Message".MatchesTag("Message.HealthPotion") 将返回 False
                    FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
                    if (Tag.MatchesTag(MessageTag))
                    {
                        const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
                        MessageWidgetRowDelegate.Broadcast(*Row);
                    }
                }
            }
            );
    }
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
    // TODO 获取所有给定技能的信息，查找对应的技能详情，并将其广播到各个 UI 控件
    if (!AuraAbilitySystemComponent->bStartupAbilitiesGiven) return;
    FForEachAbility BroadcastDelegate;
    BroadcastDelegate.BindLambda([this, AuraAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
    {
        //TODO 需要一种方法，能够根据给定的技能规格（Ability Spec）查找出其对应的技能标签（Ability Tag）
        FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
        Info.InputTag = AuraAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
        AbilityInfoDelegate.Broadcast(Info);
    });
    AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}


void UOverlayWidgetController::OnXPChanged(int32 NewXP) const
{
    const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
    const ULevelUpInfo* LevelUpInfo = AuraPlayerState->LevelUpInfo;
    checkf(LevelUpInfo, TEXT("Unabled to find LevelUpInfo. Please fill out AuraPlayerState Blueprint"));

    const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
    const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

    if (Level <= MaxLevel && Level > 0)
    {
        const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
        const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

        const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
        const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

        const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

        OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
    }
}