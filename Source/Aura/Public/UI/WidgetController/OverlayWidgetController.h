// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

class UAuraAbilitySystemComponent;
class UAbilityInfo;
class UAuraUserWidget;
struct FAuraAbilityInfo;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag MessageTag = FGameplayTag();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Message = FText();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UAuraUserWidget> MessageWidget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Image = nullptr;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
    GENERATED_BODY()

public:
    //广播初始值
    virtual void BroadcastInitialValues() override;
    virtual void BindCallbacksToDependencies() override;
    //生命值委托
    UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
    FOnAttributeChangedSignature OnHealthChanged;
    UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
    FOnAttributeChangedSignature OnMaxHealthChanged;
    //魔力值委托
    UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
    FOnAttributeChangedSignature OnManaChanged;
    UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
    FOnAttributeChangedSignature OnMaxManaChanged;
    //数据表行委托
    UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
    FMessageWidgetRowSignature MessageWidgetRowDelegate;
    //能力信息委托
    UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
    FAbilityInfoSignature AbilityInfoDelegate;
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
    TObjectPtr<UDataTable> MessageWidgetDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
    TObjectPtr<UAbilityInfo> AbilityInfo;
    
    template <typename T>
    T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

    void OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent);
};
//从指定的 DataTable 中查找与给定 GameplayTag 匹配的行
template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
    return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}