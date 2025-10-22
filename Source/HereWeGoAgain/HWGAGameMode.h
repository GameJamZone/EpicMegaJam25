#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HWGAGameMode.generated.h"

class AArena;

/**
 * 
 */
UCLASS()
class HEREWEGOAGAIN_API AHWGAGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// This array should only contain arenas but due to the way it's populated it's of type AActor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena")
	TArray<TObjectPtr<AArena>> AllArenas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena")
	float ArenaSpawnRate = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena")
	float RageGrowthRate = 0.10f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="HereWeGoAgain|Arena")
	float CurrentRagePercent = 0.f;
	
	virtual void SelectRandomArenaToActivate();

	void IncreaseRage(); 

	UFUNCTION()
	virtual void HandleArenaMinQuotaReached(AArena* DeactivatedActor);

	UFUNCTION()
	virtual void HandleArenaDeactivated(AArena* DeactivatedActor);

	TQueue<AArena*> ArenaQueue;
};