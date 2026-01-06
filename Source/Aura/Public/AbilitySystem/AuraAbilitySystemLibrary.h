// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AuraAbilitySystemLibrary.generated.h"

//class UOverlayWidgetController;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

    UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController",meta=(WorldContext="WorldContextObject"))
    static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController",meta=(WorldContext="WorldContextObject"))
    static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, Category="GameplayTags")
    static FGameplayTag GetTag(FName TagName){return FGameplayTag::RequestGameplayTag(TagName);}

    UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
    static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

    UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
    static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC);

    UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
    static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
    static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
    static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
    static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

    UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
    static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
};
