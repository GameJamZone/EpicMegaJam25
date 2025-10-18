#include "HWGAGameMode.h"

#include "Actors/Arena.h"
#include "Kismet/GameplayStatics.h"

void AHWGAGameMode::BeginPlay()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArena::StaticClass(), AllArenas);
	ensureMsgf(!AllArenas.IsEmpty(), TEXT("Found %d actors in the level."), AllArenas.Num());

	SelectRandomArenaToActivate();
	
	Super::BeginPlay();
}

void AHWGAGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AHWGAGameMode::SelectRandomArenaToActivate() const
{
	if (AllArenas.IsEmpty())
		return;
	
	const uint32 RandomValue = FMath::RandRange(0, AllArenas.Num()-1);
	
	if (AArena* Arena = CastChecked<AArena>(AllArenas[RandomValue]))
	{
		Arena->ActivateArena();
	}
}
