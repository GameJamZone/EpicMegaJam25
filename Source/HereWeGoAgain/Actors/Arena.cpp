#include "Arena.h"

#include "CleanableActor.h"
#include "SpawnArea.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "HereWeGoAgain/GameplayMessagePayload.h"
#include "HereWeGoAgain/ProjectGameplayTags.h"

AArena::AArena()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AArena::ActivateArena()
{
	SpawnAllCleanableActors();
	return true;
}

TArray<FGameplayTag> AArena::GetAllUniqueSpawnedActorTags() const
{
	TArray<FGameplayTag> AllUniqueActorTags;
	TotalCleanableObjects.GetKeys(AllUniqueActorTags);
	
	return AllUniqueActorTags;
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
					SpawnOneActor(SpawnArea, LoadedClass, ActorConfig.ActorTypeTag);
				}
			}
			else
			{
				if (!SpawnOneActor(SpawnArea, LoadedClass, ActorConfig.ActorTypeTag))
				{
					SpawnOneActor(SpawnArea, LoadedClass, ActorConfig.ActorTypeTag);
				}
			}
		}
	}
	
	return true;
}

bool AArena::SpawnOneActor(ASpawnArea* SpawnArea, UClass* LoadedClass, FGameplayTag ActorTypeTag)
{
	const FVector Location = SpawnArea->GetRandomPointInArea() + FVector(0, 0, 50.f);
	const FRotator Rotation = FRotator::ZeroRotator;
	
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(LoadedClass, Location, Rotation);
				
	if (auto CleanableSpawnedActor = CastChecked<ACleanableActor>(Spawned))
	{
		CleanableSpawnedActor->OnDestroyed.AddDynamic(this, &AArena::OnSpawnedActorDestroyed);
		CleanableSpawnedActor->ActorTypeTag = ActorTypeTag;
		SpawnedActors.Add(CleanableSpawnedActor);
		
		if (TotalCleanableObjects.Contains(ActorTypeTag))
			TotalCleanableObjects[ActorTypeTag] += 1;
		
		else
			TotalCleanableObjects.Add(ActorTypeTag, 1);
		
		return true;
	}
	
	return false;
}

void AArena::OnSpawnedActorDestroyed(AActor* DestroyedActor)
{
	if (SpawnedActors.Contains(DestroyedActor))
	{
		SpawnedActors.Remove(DestroyedActor);
	}

	if (auto CleanableSpawnedActor = CastChecked<ACleanableActor>(DestroyedActor))
	{
		// If this fails check for an enemy
		FGameplayTag Key = CleanableSpawnedActor->ActorTypeTag;
		TotalCleanableObjects[Key] -= 1;

		FUpdateArenaTotalsMessage UpdateArenaTotalsMessage;
		UpdateArenaTotalsMessage.ActorTypeTag = Key;
		UpdateArenaTotalsMessage.CurrentTotal = TotalCleanableObjects[Key];
		
		// Send the activation message
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		FGameplayTag ChannelTag = ProjectGameplayTags::Message_Arena_Updated;
		 
		MessageSubsystem.BroadcastMessage(ChannelTag, UpdateArenaTotalsMessage);
	}
}
