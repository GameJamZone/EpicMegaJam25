#pragma once
#include "GameplayTagContainer.h"

#include "GameplayMessagePayload.generated.h"

USTRUCT(BlueprintType)
struct FGameplayMessagePayload
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category=Message, meta=(ToolTip="Tag used to match broadcast and receive messages."))
	FGameplayTag MessageTag;
};


USTRUCT(BlueprintType)
struct FNewArenaActivatedMessage : public FGameplayMessagePayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category=Message, meta=(ToolTip="The location of the newly activated arena."))
	FVector ArenaPosition;
	
	UPROPERTY(BlueprintReadWrite, Category=Message, meta=(ToolTip="A map of all the cleanable object names and the total count for the arena."))
	TMap<FGameplayTag, FCleanableObjectData> TotalCleanableObjects;

	UPROPERTY(BlueprintReadWrite, Category=Message)
	float ArenaMinCleaningQuota;
};

USTRUCT(BlueprintType)
struct FUpdateArenaTotalsMessage : public FGameplayMessagePayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category=Message)
	FGameplayTag ActorTypeTag;

	UPROPERTY(BlueprintReadWrite, Category=Message)
	int CurrentTotal; 
};