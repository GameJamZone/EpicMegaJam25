#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "SpawnTableRow.generated.h"

/**
 * A single row describing what to spawn and how.
 */
USTRUCT(BlueprintType)
struct FSpawnTableRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Gameplay tag identifier for this spawn entry */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Spawn", ToolTip="Gameplay Tag ID for this spawn entry"))
	FGameplayTag SpawnTag;
	
	/** Actor class to spawn (soft reference so it loads on demand) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Class of the actor to spawn."))
	TSoftClassPtr<AActor> ActorClass;

	// Number of instances to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Number of actors to spawn."))
	int32 Count = 1;

	// Optional spawn offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Random spawn offset radius."))
	float SpawnRadius = 100.f;
};