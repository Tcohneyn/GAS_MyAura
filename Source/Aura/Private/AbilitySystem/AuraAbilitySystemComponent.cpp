// Tcohneyn All Rights Reserved


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTagsController.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
//当角色设置完 AbilityActorInfo 后调用，用于触发委托绑定。
void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::ClientEffectApplied);
    
}

// 为角色添加一系列初始技能
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    // 遍历传入的初始技能类数组
    for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
    {
        // 创建技能规格说明，参数为技能类和技能等级(这里设为1级)
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
        
        // 标准的授予技能方法（当前被注释，暂未使用）
        //GiveAbility(AbilitySpec);
        
        // 授予技能并立即激活一次（当前使用的授予方式）
        // GiveAbilityAndActivateOnce(AbilitySpec);

        if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
        {
            // **核心操作：为技能规格添加启动输入标签**
            // 从技能对象 (AuraAbility) 中获取其预设的 StartupInputTag (启动输入标签)
            // 并将这个标签添加到技能规格的 **动态来源标签集** (Dynamic Spec Source Tags) 中
            AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbility->StartupInputTag);

            // **授予技能**
            // 调用 GiveAbility 函数，将配置好的技能规格 (AbilitySpec) 授予给拥有该技能系统组件的角色
            // 这使得角色能够使用这个技能
            GiveAbility(AbilitySpec);
        }
    }
}

// 处理技能输入按键按住状态
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
    // 检查输入标签是否有效
    if (!InputTag.IsValid()) return;

    // 遍历所有可激活的技能
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        // 检查当前技能的动态标签是否与输入标签精确匹配
        if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
        {
            // 通知技能输入被按下（无论技能是否已激活）
            AbilitySpecInputPressed(AbilitySpec);
            
            // 如果技能尚未激活，则尝试激活它
            if (!AbilitySpec.IsActive())
            {
                TryActivateAbility(AbilitySpec.Handle);
            }
        }
    }
}

// 处理技能输入按键释放状态
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
    // 检查输入标签是否有效
    if (!InputTag.IsValid()) return;

    // 遍历所有可激活的技能
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        // 检查当前技能的动态标签是否与输入标签精确匹配
        if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
        {
            // 通知技能输入被释放
            AbilitySpecInputReleased(AbilitySpec);
        }
    }
}

//OnGameplayEffectAppliedDelegateToSelf回调函数
void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
    FActiveGameplayEffectHandle ActiveEffectHandle)
{
    //GEngine->AddOnScreenDebugMessage(1,8.f,FColor::Blue,FString("Effect Applied!"));
    FGameplayTagContainer TagContainer;
    EffectSpec.GetAllAssetTags(TagContainer);
    // for (const FGameplayTag& Tag : TagContainer)
    // {
    //     //TODO: Broadcast the tag to the Widget Controller
    //     const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
    //     GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, Msg);
    // }
    EffectAssetTags.Broadcast(TagContainer);
}