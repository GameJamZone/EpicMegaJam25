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
	
	/** Actor class to spawn (soft reference so it loads on demand) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Class of the actor to spawn."))
	TSoftClassPtr<AActor> ActorClass;

	// Optional spawn offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Random spawn offset radius."))
	float SpawnRadius = 100.f;
};