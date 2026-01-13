// Tcohneyn All Rights Reserved


#include "AuraGameplayTags.h"

namespace VitalAttributes
{
  UE_DEFINE_GAMEPLAY_TAG(Attributes_Vital_Health, "Attributes.Vital.Health");
  //UE_DEFINE_GAMEPLAY_TAG(Attributes_Vital_MaxHealth, "Attributes.Vital.MaxHealth");
  UE_DEFINE_GAMEPLAY_TAG(Attributes_Vital_Mana, "Attributes.Vital.Mana");
  //UE_DEFINE_GAMEPLAY_TAG(Attributes_Vital_MaxMana, "Attributes.Vital.MaxMana");
}

namespace Message
{
  UE_DEFINE_GAMEPLAY_TAG(Message_HealthCrystal, "Message.HealthCrystal");
  UE_DEFINE_GAMEPLAY_TAG(Message_HealthPotion, "Message.HealthPotion");
  UE_DEFINE_GAMEPLAY_TAG(Message_ManaCrystal, "Message.ManaCrystal");
  UE_DEFINE_GAMEPLAY_TAG(Message_ManaPotion, "Message.ManaPotion");
}
namespace Event
{
  UE_DEFINE_GAMEPLAY_TAG(Event_Montage_Firebolt, "Event.Montage.Firebolt");
  //UE_DEFINE_GAMEPLAY_TAG(Event_Montage_Attack_Melee, "Event.Montage.Attack.Melee");
}