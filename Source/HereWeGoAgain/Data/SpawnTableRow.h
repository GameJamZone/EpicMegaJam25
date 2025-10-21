#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
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

USTRUCT(BlueprintType)
struct FUpgradeTableRow : public FGameplayTagTableRow
{
	GENERATED_BODY()

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="Upgrade", meta=(ToolTip="Name of the upgrade to spawn."))
	FText UpgradeName;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="Upgrade", meta=(ToolTip="Description of the upgrade to spawn."))
	FText Description;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="Upgrade", meta=(ToolTip="Icon to display for the upgrade."))
	UTexture2D* Icon;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="Upgrade", meta=(ToolTip="Gameplay effect to apply when the upgrade is purchased."))
	TSoftClassPtr<UGameplayEffect> UpgradeEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Upgrade", meta=(ToolTip="Gameplay tag to apply to the actor when the upgrade is purchased."))
	FGameplayTag SetByCallerTag;

	// Default magnitude to apply
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Upgrade", meta=(ToolTip="Default magnitude to apply to the upgrade when purchased."))
	float Magnitude = 0.0f;
};