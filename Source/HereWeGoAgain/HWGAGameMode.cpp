#include "HWGAGameMode.h"

#include "GameplayMessagePayload.h"
#include "ProjectGameplayTags.h"
#include "Actors/Arena.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Containers/Queue.h"
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
			Arena->OnArenaMinQuotaCleared.AddDynamic(this, &AHWGAGameMode::HandleArenaMinQuotaReached);
			Arena->OnArenaDeactivated.AddDynamic(this, &AHWGAGameMode::HandleArenaDeactivated);

			AllArenas.Add(Arena);
		}
	}
	
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AHWGAGameMode::SelectRandomArenaToActivate, ArenaSpawnRate, true);
}

void AHWGAGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AHWGAGameMode::SelectRandomArenaToActivate()
{
	if (AllArenas.IsEmpty())
		return;
	
	TArray<AArena*> InactiveArenas = AllArenas.FilterByPredicate([](AArena* Arena)
	{
		return Arena && !Arena->bArenaIsActive;
	});

	if (InactiveArenas.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, InactiveArenas.Num() - 1);
		AArena* SelectedArena = InactiveArenas[RandomIndex];
		SelectedArena->ActivateArena();

		// We don't want to increase rage the first time an arena activates.
		if (!ArenaQueue.IsEmpty())
		{
			IncreaseRage();
		}
		
		ArenaQueue.Enqueue(SelectedArena);
		
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("No inactive arenas available!")); // Maybe we should do something here?
}

void AHWGAGameMode::IncreaseRage()
{
	CurrentRagePercent = CurrentRagePercent + RageGrowthRate;
	
	FUpdateRageMessage UpdateRageMessage;
	UpdateRageMessage.CurrentRagePercent = CurrentRagePercent;
	UpdateRageMessage.CurrentRageGrowthRate = RageGrowthRate;
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FGameplayTag ChannelTag = ProjectGameplayTags::Message_Rage_Updated;
	 
	MessageSubsystem.BroadcastMessage(ChannelTag, UpdateRageMessage);
}

void AHWGAGameMode::HandleArenaMinQuotaReached(AArena* DeactivatedActor)
{
	FArenaMinQuotaClearedMessage ArenaMinQuotaClearedMessage;
	ArenaMinQuotaClearedMessage.MinQuota = DeactivatedActor->MinCleaningQuota;
		
	// Send the activation message
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FGameplayTag ChannelTag = ProjectGameplayTags::Message_Arena_MinQuotaCleared;
		 
	MessageSubsystem.BroadcastMessage(ChannelTag, ArenaMinQuotaClearedMessage);
	
	//SelectRandomArenaToActivate();
}

void AHWGAGameMode::HandleArenaDeactivated(AArena* DeactivatedActor)
{
	ArenaQueue.Dequeue(DeactivatedActor);
}

FVector AHWGAGameMode::GetFirstArenaLocation() const
{
	if (ArenaQueue.IsEmpty())
	{
		return FVector::ZeroVector;
	}

	AArena* OutArena = nullptr;
	ArenaQueue.Peek(OutArena);

	return OutArena->GetActorLocation();
}
