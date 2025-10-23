#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"

#include "Arena.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UStaticMesh;
class UImage;
class UTexture2D;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundCue;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArenaMinQuotaReached, AArena*, ArenaActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArenaDeactivated, AArena*, DeactivatedArenaActor);

USTRUCT(BlueprintType)
struct FCleanableObjectData
{
	GENERATED_BODY()

	FCleanableObjectData(int32 Current, int32 Max): Current(Current), Max(Max){};
	FCleanableObjectData(): Current(0), Max(0){};

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Spawn")
	int32 Current;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Spawn")
	int32 Max;
};



/// Arena Manager, handles spawning debris, fire and general mess for the player to clean up.
UCLASS()
class HEREWEGOAGAIN_API AArena : public AActor
{
	GENERATED_BODY()
	
public:
	AArena();

	UPROPERTY(BlueprintAssignable, Category="HereWeGoAgain|Events")
	FOnArenaMinQuotaReached OnArenaMinQuotaCleared;

	UPROPERTY(BlueprintAssignable, Category="HereWeGoAgain|Events")
	FOnArenaDeactivated OnArenaDeactivated;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HereWeGoAgain|Arena|Effects")
	TObjectPtr<USoundCue> SoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HereWeGoAgain|Arena|Effects")
	TObjectPtr<UNiagaraComponent> NiagaraEffectComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena")
	TObjectPtr<UStaticMesh> ActiveBuildingMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena")
	TObjectPtr<UStaticMesh> InactiveBuildingMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HereWeGoAgain|Arena|Effects")
	TArray<TObjectPtr<UNiagaraSystem>> ActivationNiagaraEffects;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena")
	TObjectPtr<USphereComponent> CleanableArea;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HereWeGoAgain|UI")
	TObjectPtr<UTexture2D> ActiveTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HereWeGoAgain|UI")
	TObjectPtr<UTexture2D> InActiveTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena", meta=(ToolTip="The areas marking where to spawn a cleanable actor."))
	TArray<TObjectPtr<class ASpawnArea>> SpawnAreas;
	
	bool bAlreadyEntered = false;

	void ActivateArena();
	void DeactivateArena();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena")
	bool bArenaIsActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena")
	bool bMinQuotaCleared = false;

	UFUNCTION(BlueprintCallable, Category="HereWeGoAgain|Arena")
	TArray<FGameplayTag> GetAllUniqueSpawnedActorTags() const;

	UFUNCTION(BlueprintCallable, Category="HereWeGoAgain|Arena")
	TMap<FGameplayTag, int32> GetTotalCleanableObjectsMap() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HereWeGoAgain|Arena", meta=(ToolTip="The areas marking where to spawn a cleanable actor."))
	float MinCleaningQuota;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	/** List of spawned actors */
	TArray<TObjectPtr<AActor>> SpawnedActors;
	
	UPROPERTY(BlueprintReadOnly, Category=Message)
	TMap<FGameplayTag, FCleanableObjectData> TotalCleanableObjects;

	UFUNCTION(BlueprintCallable, Category="HereWeGoAgain|Spawn")
	virtual void PlayActivationEffects();
	
	/** Spawns all actors defined in the data table */
	UFUNCTION(BlueprintCallable, Category="HereWeGoAgain|Spawn")
	bool SpawnAllCleanableActors();
	
	/** Spawns all actors defined in the data table */
	UFUNCTION(BlueprintCallable, Category="HereWeGoAgain|Spawn")
	bool SpawnOneActor(ASpawnArea* SpawnArea, UClass* LoadedClass, FGameplayTag ActorTypeTag);
	
	UFUNCTION()
	void OnSpawnedActorDestroyed(AActor* DestroyedActor);

	// Clearing Arena
	bool IsArenaMinQuotaCleared(FGameplayTag Key);
	bool IsArenaCleared() const;

	UFUNCTION()
	void OnAreanEntered(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnAreanExited(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};