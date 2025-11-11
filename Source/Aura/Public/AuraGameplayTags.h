// Tcohneyn All Rights Reserved

#ifndef GAMEPLAYTAGS_H
// 防止头文件被多次包含，使用宏定义进行保护
#define GAMEPLAYTAGS_H

#include "NativeGameplayTags.h"

namespace VitalAttributes
{
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_Health);
    //AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_MaxHealth);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_Mana);
    //AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_MaxMana);
}

namespace Message
{
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_HealthCrystal);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_HealthPotion);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_ManaCrystal);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_ManaPotion);
}
namespace Event
{
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_Firebolt);
}
#endif  // !GAMEPLAYTAGS_H