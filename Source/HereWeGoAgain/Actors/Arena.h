#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"

#include "Arena.generated.h"

class UImage;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArenaMinQuotaReached, AArena*, ArenaActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArenaDeactivated, AArena*, DeactivatedArenaActor);


/// Arena Manager, handles spawning debris, fire and general mess for the player to clean up.
UCLASS()
class HEREWEGOAGAIN_API AArena : public AActor
{
	GENERATED_BODY()
	
public:
	AArena();

	UPROPERTY(BlueprintAssignable, Category="HereWeGoAgain|Events")
	FOnArenaMinQuotaReached OnArenaMinQuotaReached;

	UPROPERTY(BlueprintAssignable, Category="HereWeGoAgain|Events")
	FOnArenaDeactivated OnArenaDeactivated;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* WidgetComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HereWeGoAgain|UI")
	TObjectPtr<UTexture2D> ActiveTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HereWeGoAgain|UI")
	TObjectPtr<UTexture2D> InActiveTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena", meta=(ToolTip="The areas marking where to spawn a cleanable actor."))
	TArray<TObjectPtr<class ASpawnArea>> SpawnAreas;
	
	void ActivateArena();
	void DeactivateArena();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena")
	bool bArenaIsActive = false; 

	UFUNCTION(BlueprintCallable, Category="HereWeGoAgain|Arena")
	TArray<FGameplayTag> GetAllUniqueSpawnedActorTags() const;

	UFUNCTION(BlueprintCallable, Category="HereWeGoAgain|Arena")
	TMap<FGameplayTag, int32> GetTotalCleanableObjectsMap() const
	{
		return TotalCleanableObjects;
	};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HereWeGoAgain|Arena", meta=(ToolTip="The areas marking where to spawn a cleanable actor."))
	float MinCleaningQuota;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	/** List of spawned actors */
	TArray<TObjectPtr<AActor>> SpawnedActors;
	
	UPROPERTY(BlueprintReadOnly, Category=Message)
	TMap<FGameplayTag, int32> TotalCleanableObjects;
	
	/** Spawns all actors defined in the data table */
	UFUNCTION(BlueprintCallable, Category="HereWeGoAgain|Spawn")
	bool SpawnAllCleanableActors();
	
	/** Spawns all actors defined in the data table */
	UFUNCTION(BlueprintCallable, Category="HereWeGoAgain|Spawn")
	bool SpawnOneActor(ASpawnArea* SpawnArea, UClass* LoadedClass, FGameplayTag ActorTypeTag);
	
	UFUNCTION()
	void OnSpawnedActorDestroyed(AActor* DestroyedActor);

	// Clearing Arena
	bool IsArenaMinQuotaCleared() const;
	bool IsArenaCleared() const;
};