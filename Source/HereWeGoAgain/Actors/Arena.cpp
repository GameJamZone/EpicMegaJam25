#include "Arena.h"

#include "SpawnArea.h"
#include "HereWeGoAgain/Data/SpawnTableRow.h"


AArena::AArena()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AArena::ActivateArena()
{
	SpawnFromDataTable();
	return true;
}

void AArena::BeginPlay()
{
	Super::BeginPlay();
}

void AArena::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArena::SpawnFromDataTable()
{
	if (!SpawnDataTable) return;

	SpawnedActors.Empty();

	static const FString Context = TEXT("SpawnArea Context");
	const TArray<FName> RowNames = SpawnDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		if (const FSpawnTableRow* Row = SpawnDataTable->FindRow<FSpawnTableRow>(RowName, Context))
		{
			if (!Row->ActorClass.IsNull())
			{
				UClass* LoadedClass = Row->ActorClass.LoadSynchronous();
				
				if (!LoadedClass)
					continue;
				
				for (int j = 0; j < SpawnAreas.Num(); ++j)
				{
					ASpawnArea* SpawnArea = SpawnAreas[j].Get();

					const FGameplayTag SpawnActorTypeTag = FGameplayTag::RequestGameplayTag(RowName);
					if (SpawnArea->AllowedObjectTypes.Contains(SpawnActorTypeTag))
					{
						const uint32 RandomSpawnAmount = FMath::RandRange(SpawnArea->MinSpawnCount, SpawnArea->MaxSpawnCount);

						for (uint32 i = 0; i < RandomSpawnAmount; i++)
							SpawnOneActor(SpawnArea, LoadedClass);
					}
				}
			}
		}
	}
}

void AArena::SpawnOneActor(ASpawnArea* SpawnArea, UClass* LoadedClass)
{
	const FVector Location = SpawnArea->GetRandomPointInArea() + FVector(0, 0, 50.f);
	const FRotator Rotation = FRotator::ZeroRotator;
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(LoadedClass, Location, Rotation);
				
	if (Spawned)
	{
		SpawnedActors.Add(Spawned);
	}
}
