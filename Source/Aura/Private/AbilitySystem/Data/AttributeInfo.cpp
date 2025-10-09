// Tcohneyn All Rights Reserved


#include "AbilitySystem/Data/AttributeInfo.h"

// 函数定义：在属性信息数组中查找与给定标签完全匹配的属性信息
// 参数：
//   - AttributeTag: 要查找的游戏玩法标签（FGameplayTag类型）
//   - bLogNotFound: 布尔值，控制当未找到匹配项时是否记录错误日志（可选参数，默认值可能在函数声明中设置）
// 返回值：找到的FAuraAttributeInfo结构体；如果未找到，返回一个默认构造的FAuraAttributeInfo
FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
    // 遍历AttributeInformation数组中的所有FAuraAttributeInfo元素
    for (const FAuraAttributeInfo& Info : AttributeInformation)
    {
        // 检查当前遍历到的Info的AttributeTag是否与传入的AttributeTag完全匹配
        if (Info.AttributeTag.MatchesTagExact(AttributeTag))
        {
            // 如果找到完全匹配的标签，立即返回对应的FAuraAttributeInfo信息
            return Info;
        }
    }

    // 如果遍历完整个数组仍未找到匹配的标签，且bLogNotFound参数为true
    if (bLogNotFound)
    {
        // 记录错误日志，显示未能找到哪个标签以及在哪一个AttributeInfo资产中
        UE_LOG(LogTemp, Error, TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."), 
               *AttributeTag.ToString(),    // 将标签转换为字符串用于显示
               *GetNameSafe(this));          // 获取当前对象的名称用于显示
    }

    // 返回一个默认构造的FAuraAttributeInfo（通常所有成员变量为空或默认值）
    return FAuraAttributeInfo();
}