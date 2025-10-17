// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY() 
public:
    template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
    void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
    // 检查输入配置是否有效[2](@ref)
    check(InputConfig);

    // 遍历输入配置中所有的技能输入动作
    for (const FAuraInputAction& Action : InputConfig->AbilityInputActions)
    {
        // 确保输入动作和输入标签都有效
        if (Action.InputAction && Action.InputTag.IsValid())
        {
            // 绑定按键按下事件（Started对应按键按下瞬间）
            if (PressedFunc)
            {
                BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
            }

            // 绑定按键释放事件（Completed对应按键释放）
            if (ReleasedFunc)
            {
                BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
            }
			
            // 绑定按键按住事件（Triggered对应按键按住期间持续触发）
            if (HeldFunc)
            {
                BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
            }
        }
    }
};
