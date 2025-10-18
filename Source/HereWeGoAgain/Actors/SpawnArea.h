#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "SpawnArea.generated.h"

UCLASS()
class HEREWEGOAGAIN_API ASpawnArea : public AActor
{
	GENERATED_BODY()
	
public:
	ASpawnArea();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Spawn")
	bool SpawnMultipleActors = true; // Set to false by default and hide the extent settings.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Spawn")
	int MinSpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Spawn")
	int MaxSpawnCount = 1;
	
	/** Size of the spawn area box */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Spawn")
	FVector SpawnExtent = FVector(100.f, 100.f, 100.f);
	
	FVector GetRandomPointInArea() const;
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
