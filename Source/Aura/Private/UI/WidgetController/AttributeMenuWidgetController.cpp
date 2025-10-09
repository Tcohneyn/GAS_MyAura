// Tcohneyn All Rights Reserved


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
//------------------------------------------------------------
// 将回调函数绑定到依赖对象
//------------------------------------------------------------
void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
    UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
    check(AttributeInfo);
    for (auto& Pair : AS->TagsToAttributes)
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
        [this, Pair](const FOnAttributeChangeData& Data)
        {
            BroadcastAttributeInfo(Pair.Key, Pair.Value());
        }
    );
    }
}

//------------------------------------------------------------
// 广播初始属性值到 UI 层
//------------------------------------------------------------
void UAttributeMenuWidgetController::BroadcastInitialValues()
{
    // 将 AttributeSet 转换为 Aura 的自定义属性集类型。
    // CastChecked 若转换失败会直接触发断言，保证运行时安全。
    UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

    // 检查 AttributeInfo 是否已经设置（在蓝图中绑定的 Data Asset）。
    check(AttributeInfo);

    for (auto& Pair : AS->TagsToAttributes)
    {
        BroadcastAttributeInfo(Pair.Key, Pair.Value());
    }
    // // 从 Data Asset 中查找与“力量（Strength）”标签对应的属性信息结构体。
    // // 这里使用 AuraGameplayTags 获取统一的标签定义。
    // FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(
    //     FAuraGameplayTags::Get().Attributes_Primary_Strength
    // );
    //
    // // 将 AttributeSet 中当前的力量值写入结构体中。
    // Info.AttributeValue = AS->GetStrength();
    //
    // // 广播属性信息给绑定了该委托（Delegate）的 UI Widget。
    // // UI 层接收到该信息后会更新显示（如属性名、数值等）。
    // AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
    FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
    Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
    AttributeInfoDelegate.Broadcast(Info);
}