#include "Arena.h"

#include "SpawnArea.h"

AArena::AArena()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AArena::ActivateArena()
{
	SpawnAllCleanableActors();
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

bool AArena::SpawnAllCleanableActors()
{
	SpawnedActors.Empty();

	for (ASpawnArea* SpawnArea : SpawnAreas)
	{
		if (!SpawnArea)
		{
			UE_LOG(LogTemp, Warning, TEXT("A spawn area in Arena - %s is invalid."), *(this->GetName()));
			return false;
		}
		
		for (FSpawnableActorConfig ActorConfig : SpawnArea->SpawnableObjectConfigs)
		{
			UClass* LoadedClass = ActorConfig.ActorClass.LoadSynchronous();
			
			if (!LoadedClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("The assigned class in the actor config for spawn area - %s is invalid."), *(SpawnArea->GetName()));
				return false;
			}

			if (ActorConfig.bSpawnMultipleActors)
			{
				const uint32 RandomSpawnAmount = FMath::RandRange(ActorConfig.MinSpawnCount, ActorConfig.MaxSpawnCount);
				
				for (uint32 i = 0; i < RandomSpawnAmount; i++)
				{
					SpawnOneActor(SpawnArea, LoadedClass);
				}
			}
			else
			{
				if (!SpawnOneActor(SpawnArea, LoadedClass))
				{
					SpawnOneActor(SpawnArea, LoadedClass);
				}
			}
		}
	}
	
	return true;
}

bool AArena::SpawnOneActor(ASpawnArea* SpawnArea, UClass* LoadedClass)
{
	const FVector Location = SpawnArea->GetRandomPointInArea() + FVector(0, 0, 50.f);
	const FRotator Rotation = FRotator::ZeroRotator;
	
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(LoadedClass, Location, Rotation);
				
	if (Spawned)
	{
		SpawnedActors.Add(Spawned);
		return true;
	}

	return false;
}
