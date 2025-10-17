  // Tcohneyn All Rights Reserved


#include "Input/AuraInputConfig.h"

// 根据给定的输入标签（InputTag）查找对应的输入动作（InputAction）
const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    // 遍历所有已配置的技能输入动作
    for (const FAuraInputAction& Action : AbilityInputActions)
    {
        // 检查当前动作是否有效且标签匹配
        if (Action.InputAction && Action.InputTag == InputTag)
        {
            // 找到匹配项，返回对应的输入动作
            return Action.InputAction;
        }
    }

    // 如果未找到匹配项且需要记录日志
    if (bLogNotFound)
    {
        // 输出错误日志，包含未找到的标签和配置对象信息[6,7](@ref)
        UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s], on InputConfig [%s]"), 
               *InputTag.ToString(), *GetNameSafe(this));
    }

    // 未找到匹配项，返回空指针
    return nullptr;
}