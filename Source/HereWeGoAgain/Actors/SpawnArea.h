#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "SpawnArea.generated.h"

USTRUCT(BlueprintType)
struct FSpawnableActorConfig
{
	GENERATED_BODY()
	
	/** Actor class to spawn (soft reference so it loads on demand) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Class of the actor to spawn."))
	TSoftClassPtr<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Spawn")
	bool bSpawnMultipleActors = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bSpawnMultipleActors", EditConditionHides), Category="HereWeGoAgain|Spawn")
	int MinSpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bSpawnMultipleActors", EditConditionHides), Category="HereWeGoAgain|Spawn")
	int MaxSpawnCount = 1;
};


UCLASS()
class HEREWEGOAGAIN_API ASpawnArea : public AActor
{
	GENERATED_BODY()
	
public:
	ASpawnArea();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HereWeGoAgain|Spawn")
	TArray<FSpawnableActorConfig> SpawnableObjectConfigs;
	
	/** Size of the spawn area box */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Spawn")
	FVector SpawnExtent = FVector(100.f, 100.f, 100.f);
	
	FVector GetRandomPointInArea() const;
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
