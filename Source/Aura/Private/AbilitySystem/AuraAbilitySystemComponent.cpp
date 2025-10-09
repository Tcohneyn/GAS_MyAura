// Tcohneyn All Rights Reserved


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTagsController.h"
//当角色设置完 AbilityActorInfo 后调用，用于触发委托绑定。
void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::EffectApplied);
    
}
//OnGameplayEffectAppliedDelegateToSelf回调函数
void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
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