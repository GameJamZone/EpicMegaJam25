#include "Arena.h"

#include "CleanableActor.h"
#include "SpawnArea.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "HereWeGoAgain/ArenaWidget.h"
#include "HereWeGoAgain/GameplayMessagePayload.h"
#include "HereWeGoAgain/ProjectGameplayTags.h"
#include "SpawnableInterface.h"
#include "Components/SphereComponent.h"
#include "HereWeGoAgain/GASPlayerCharacter.h"

AArena::AArena()
{
	PrimaryActorTick.bCanEverTick = true;

	CleanableArea = CreateDefaultSubobject<USphereComponent>(TEXT("CleanableArea"));
	CleanableArea->SetupAttachment(RootComponent);
	CleanableArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	CleanableArea->OnComponentBeginOverlap.AddDynamic(this, &AArena::OnAreanEntered);
	CleanableArea->OnComponentEndOverlap.AddDynamic(this, &AArena::OnAreanExited);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(CleanableArea);
	
	//WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	//WidgetComponent->SetupAttachment(RootComponent);

	// Set the widget class (replace with your blueprint path)
	// static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/Blueprints/Widgets/W_ArenaUIDebug"));
	// if (WidgetClass.Succeeded())
	// {
	// 	WidgetComponent->SetWidgetClass(WidgetClass.Class);
	// 	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); 
	// }
}

void AArena::ActivateArena()
{
	if (StaticMeshComponent && ActiveBuildingMesh)
		StaticMeshComponent->SetStaticMesh(ActiveBuildingMesh);
	
	SpawnAllCleanableActors();

	bArenaIsActive = true;

	// if (UUserWidget* Widget = WidgetComponent->GetUserWidgetObject())
	// {
	// 	if (UArenaWidget* ImageWidget = Cast<UArenaWidget>(Widget))
	// 	{
	// 		if (auto Texture = ImageWidget->ActiveArenaTexture)
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("Setting ActiveArenaTexture on %s"), *ImageWidget->GetName());
	// 			ImageWidget->SetImageTexture(Texture);
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("ActiveArenaTexture is null on %s"), *ImageWidget->GetName());
	// 		}
	// 	}
	// }
}

void AArena::DeactivateArena()
{
	if (StaticMeshComponent && InactiveBuildingMesh)
		StaticMeshComponent->SetStaticMesh(InactiveBuildingMesh);
	
	bArenaIsActive = false;
	bAlreadyEntered = false;
	bMinQuotaCleared = false;
	TotalCleanableObjects.Empty();
	SpawnedActors.Empty();

	TArray<AActor*> OverlappingActors;
	CleanableArea->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		AGASPlayerCharacter* ActorChar = Cast<AGASPlayerCharacter>(Actor);
		if (ActorChar)
		{
			ActorChar->SetDirectionArrowVisibility(true);
		}
	}
	
	// if (UUserWidget* Widget = WidgetComponent->GetUserWidgetObject())
	// {
	// 	if (UArenaWidget* ImageWidget = Cast<UArenaWidget>(Widget))
	// 	{
	// 		if (auto Texture = ImageWidget->DefaultTexture)
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("Setting ActiveArenaTexture on %s"), *ImageWidget->GetName());
	// 			ImageWidget->SetImageTexture(Texture);
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("ActiveArenaTexture is null on %s"), *ImageWidget->GetName());
	// 		}
	// 	}
	// }
}

TArray<FGameplayTag> AArena::GetAllUniqueSpawnedActorTags() const
{
	TArray<FGameplayTag> AllUniqueActorTags;
	TotalCleanableObjects.GetKeys(AllUniqueActorTags);
	
	return AllUniqueActorTags;
}

TMap<FGameplayTag, int32> AArena::GetTotalCleanableObjectsMap() const
{
	TMap<FGameplayTag, int32> TempMap;
	
	for (auto CleanableData : TotalCleanableObjects)
	{
		if (!TempMap.Contains(CleanableData.Key))
		{
			TempMap.Add(CleanableData.Key, CleanableData.Value.Max);
		}
	}
	
	return TempMap;
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
	if (!Spawned)
		return false;
	
	//Spawned->OnDestroyed.AddDynamic(this, &AArena::OnSpawnedActorDestroyed);
	
	if (ISpawnableInterface* SpawnedInterface = Cast<ISpawnableInterface>(Spawned))
	{
		Spawned->OnDestroyed.AddDynamic(this, &AArena::OnSpawnedActorDestroyed);
		SpawnedInterface->SetActorTag(ActorTypeTag);
		SpawnedActors.Add(Spawned);
		
		if (TotalCleanableObjects.Contains(ActorTypeTag))
			TotalCleanableObjects[ActorTypeTag].Max += 1;
		
		else
			TotalCleanableObjects.Add(ActorTypeTag, FCleanableObjectData(0,1));
		
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
	else
	{
		// This is required because it appears that the destroy function is called multiple times on the same actor,
		// it's unclear where it's coming from, definitely not from the Attrib Set or the overridden Destroy.
		return;
	}

	if (auto CleanableSpawnedActor = CastChecked<ISpawnableInterface>(DestroyedActor))
	{
		// If this fails check for an enemy
		FGameplayTag Key = CleanableSpawnedActor->GetActorTag();

		if (!TotalCleanableObjects.Contains(Key))
		{
			UE_LOG(LogTemp, Warning, TEXT("Attempting to destroy a cleanable actor but received an invalid type tag."));
			return; 
		}

		TotalCleanableObjects[Key].Current += 1;
		
		FUpdateArenaTotalsMessage UpdateArenaTotalsMessage;
		UpdateArenaTotalsMessage.ActorTypeTag = Key;
		UpdateArenaTotalsMessage.CurrentTotal = TotalCleanableObjects[Key].Current;
		
		// Send the activation message
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		FGameplayTag ChannelTag = ProjectGameplayTags::Message_Arena_Updated;
		 
		MessageSubsystem.BroadcastMessage(ChannelTag, UpdateArenaTotalsMessage);
		
		if (!bMinQuotaCleared)
		{
			bMinQuotaCleared = IsArenaMinQuotaCleared();
		}

		if (IsArenaCleared())
		{
			DeactivateArena();
			OnArenaDeactivated.Broadcast(this);
		}
	}
}

bool AArena::IsArenaMinQuotaCleared()
{
	int MaxQuota = 0;
	int CurrentAmountCleaned = 0;

	for (auto CleanableObjectData: TotalCleanableObjects)
	{
		MaxQuota += CleanableObjectData.Value.Max;
		CurrentAmountCleaned += CleanableObjectData.Value.Current;
	}

	float MinQuotaPercent = MinCleaningQuota / 100;
	float MinQuotaObjectCount = MaxQuota * MinQuotaPercent;

	if (CurrentAmountCleaned >= MinQuotaObjectCount)
	{
		OnArenaMinQuotaCleared.Broadcast(this);
		return true;
	}
	
	return false;
}

bool AArena::IsArenaCleared() const
{
	int32 currentTotal = 0;
	int32 maxTotal = 0;
	
	for (auto CleanableObjectData: TotalCleanableObjects)
	{
		currentTotal += CleanableObjectData.Value.Current;
		maxTotal += CleanableObjectData.Value.Max;
	}
	
	if (currentTotal == maxTotal)
	{
		return true;
	}
	
	return false;
}

void AArena::OnAreanEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	AGASPlayerCharacter* Actor = Cast<AGASPlayerCharacter>(OtherActor);
	
	if (!Actor || !bArenaIsActive)
	{
		return;
	}

	Actor->SetDirectionArrowVisibility(false);
	
	FNewArenaActivatedMessage NewArenaActivatedMessage;
	NewArenaActivatedMessage.ArenaPosition = GetActorLocation();
	NewArenaActivatedMessage.ArenaMinCleaningQuota = MinCleaningQuota;
			
	for (auto ActorTag : GetAllUniqueSpawnedActorTags())
	{
		NewArenaActivatedMessage.TotalCleanableObjects = TotalCleanableObjects;
	}
	
	// // Send the activation message
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	const FGameplayTag ChannelTag = ProjectGameplayTags::Message_Arena_Activated;
	 
	MessageSubsystem.BroadcastMessage(ChannelTag, NewArenaActivatedMessage);
	
	bAlreadyEntered = true;
}

void AArena::OnAreanExited(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AGASPlayerCharacter* Actor = Cast<AGASPlayerCharacter>(OtherActor);
	
	if (!Actor || !bArenaIsActive)
	{
		return;
	}

	Actor->SetDirectionArrowVisibility(true);
}
