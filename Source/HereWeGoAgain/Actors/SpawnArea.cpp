#include "SpawnArea.h"

#include "HereWeGoAgain/Data/SpawnTableRow.h"
#include "Kismet/KismetMathLibrary.h"


ASpawnArea::ASpawnArea()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASpawnArea::OnConstruction(const FTransform& Transform)
{
#if WITH_EDITOR
	DrawDebugBox(GetWorld(), GetActorLocation(), SpawnExtent, FColor::Green, false, 1.f);
#endif
	
	Super::OnConstruction(Transform);
}

void ASpawnArea::BeginPlay()
{
	Super::BeginPlay();
}

FVector ASpawnArea::GetRandomPointInArea() const
{
	const FVector Origin = GetActorLocation();
	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, SpawnExtent);
}