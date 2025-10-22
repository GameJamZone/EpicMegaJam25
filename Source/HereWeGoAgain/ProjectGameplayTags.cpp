#include "ProjectGameplayTags.h"

namespace ProjectGameplayTags
{
    // Input tags
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Move,             "Input.Move",          "Axis input: Move");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Look,              "Input.Look",          "Axis input: Look");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Jump,              "Input.Jump",          "Ability input: Jump");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Combat_Hit,        "Event.Combat.Hit",   "Event hit");

    // Message tags
    UE_DEFINE_GAMEPLAY_TAG(Message_Arena_Activated, "Message.Arena.Activated");
    UE_DEFINE_GAMEPLAY_TAG(Message_Arena_Updated, "Message.Arena.Updated");
    UE_DEFINE_GAMEPLAY_TAG(Message_Rage_Updated, "Message.Rage.Updated");
}
