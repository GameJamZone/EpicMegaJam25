#pragma once
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

namespace ProjectGameplayTags
{
    // Input tags (these match properties restricted to Categories="InputTag")
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Move);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Look);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Jump);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Combat_Hit);
    
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Arena_Activated);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Arena_Updated);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Arena_MinQuotaCleared);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_City_Destruction_Updated);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Rage_Updated);
}
