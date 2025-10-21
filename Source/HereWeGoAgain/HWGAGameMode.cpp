#include "HWGAGameMode.h"

#include "GameplayMessagePayload.h"
#include "ProjectGameplayTags.h"
#include "Actors/Arena.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Kismet/GameplayStatics.h"

void AHWGAGameMode::BeginPlay()
{

	Super::BeginPlay();

	TArray<AActor*> TempFoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArena::StaticClass(), TempFoundActors);
	ensureMsgf(!TempFoundActors.IsEmpty(), TEXT("Found %d actors in the level."), TempFoundActors.Num());

	for (AActor* Actor : TempFoundActors)
	{
		if (AArena* Arena = Cast<AArena>(Actor))
		{
			Arena->OnArenaMinQuotaReached.AddDynamic(this, &AHWGAGameMode::HandleArenaMinQuotaReached);
			Arena->OnArenaDeactivated.AddDynamic(this, &AHWGAGameMode::HandleArenaDeactivated);

			AllArenas.Add(Arena);
		}
	}

	//FTimerHandle handle;
	//GetWorld()->GetTimerManager().SetTimer(handle, this, &AHWGAGameMode::SelectRandomArenaToActivate, 10.f, false); // 10 seconds ( 10 * 1000 = 10000 ms

	SelectRandomArenaToActivate();
}

void AHWGAGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool AHWGAGameMode::SelectRandomArenaToActivate() const
{
	if (AllArenas.IsEmpty())
		return false;
	
	TArray<AArena*> InactiveArenas = AllArenas.FilterByPredicate([](AArena* Arena)
	{
		return Arena && !Arena->bArenaIsActive;
	});

	if (InactiveArenas.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, InactiveArenas.Num() - 1);
		AArena* SelectedArena = InactiveArenas[RandomIndex];
		SelectedArena->ActivateArena();

		FNewArenaActivatedMessage NewArenaActivatedMessage;
		NewArenaActivatedMessage.ArenaPosition = SelectedArena->GetActorLocation();
		NewArenaActivatedMessage.ArenaMinCleaningQuota = SelectedArena->MinCleaningQuota;
			
		for (auto ActorTag : SelectedArena->GetAllUniqueSpawnedActorTags())
		{
			NewArenaActivatedMessage.TotalCleanableObjects = SelectedArena->GetTotalCleanableObjectsMap();
		}

		// Send the activation message
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		FGameplayTag ChannelTag = ProjectGameplayTags::Message_Arena_Activated;
		 
		MessageSubsystem.BroadcastMessage(ChannelTag, NewArenaActivatedMessage);

		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("No inactive arenas available!")); // Maybe we should do something here?
	return false;
}

void AHWGAGameMode::HandleArenaMinQuotaReached(AArena* DeactivatedActor)
{
	SelectRandomArenaToActivate();
}

void AHWGAGameMode::HandleArenaDeactivated(AArena* DeactivatedActor)
{
	//TODO
}