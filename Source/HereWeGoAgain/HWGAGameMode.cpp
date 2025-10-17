// Fill out your copyright notice in the Description page of Project Settings.


#include "HWGAGameMode.h"

#include "Actors/Arena.h"
#include "Kismet/GameplayStatics.h"

void AHWGAGameMode::BeginPlay()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArena::StaticClass(), AllArenas);

	UE_LOG(LogTemp, Log, TEXT("Found %d actors in the level."), AllArenas.Num());

	if (AArena* Arena = CastChecked<AArena>(AllArenas[0]))
	{
		Arena->ActivateArena();
	}
	
	Super::BeginPlay();
}

void AHWGAGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AHWGAGameMode::SelectRandomArenaToActivate() const
{
}
