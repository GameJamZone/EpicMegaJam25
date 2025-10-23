#include "ProjectGameplayTags.h"

namespace ProjectGameplayTags
{
    // Input tags
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Move,             "Input.Move",          "Axis input: Move");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Look,              "Input.Look",          "Axis input: Look");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Jump,              "Input.Jump",          "Ability input: Jump");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Combat_Hit,        "Event.Combat.Hit",   "Event hit");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Combat_Hit_Death,        "Event.Combat.Hit.Death",   "Event hit Death");

    // Message tags
    UE_DEFINE_GAMEPLAY_TAG(Message_Arena_Activated, "Message.Arena.Activated");
    UE_DEFINE_GAMEPLAY_TAG(Message_Arena_Updated, "Message.Arena.Updated");
    UE_DEFINE_GAMEPLAY_TAG(Message_Arena_MinQuotaCleared, "Message.Arena.MinQuota.Cleared");
    UE_DEFINE_GAMEPLAY_TAG(Message_City_Destruction_Updated, "Message.City.Destruction.Updated");
    UE_DEFINE_GAMEPLAY_TAG(Message_Rage_Updated, "Message.Rage.Updated");
    UE_DEFINE_GAMEPLAY_TAG(Message_Boss_Defeated, "Message.Boss.Defeated");
}
