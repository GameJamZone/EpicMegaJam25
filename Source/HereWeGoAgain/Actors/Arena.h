#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Arena.generated.h"

/// Arena Manager, handles spawning debris, fire and general mess for the player to clean up.
UCLASS()
class HEREWEGOAGAIN_API AArena : public AActor
{
	GENERATED_BODY()
	
public:
	AArena();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena", meta=(ToolTip="The areas marking where to spawn a cleanable actor."))
	TArray<TObjectPtr<class ASpawnArea>> SpawnAreas;

	bool ActivateArena();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	/** List of spawned actors */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="HereWeGoAgain|Spawn", meta=(ToolTip="The actors that are currently spawned in this area."))
	TArray<AActor*> SpawnedActors;

	/** Spawns all actors defined in the data table */
	UFUNCTION(BlueprintCallable, Category="HereWeGoAgain|Spawn")
	bool SpawnAllCleanableActors();

	/** Spawns all actors defined in the data table */
	UFUNCTION(BlueprintCallable, Category="HereWeGoAgain|Spawn")
	bool SpawnOneActor(ASpawnArea* SpawnArea, UClass* LoadedClass);
};
