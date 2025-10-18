#include "SpawnArea.h"

#include "HereWeGoAgain/Data/SpawnTableRow.h"
#include "Kismet/KismetMathLibrary.h"


ASpawnArea::ASpawnArea()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpawnArea::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ASpawnArea::BeginPlay()
{
	Super::BeginPlay();
	
#if WITH_EDITOR
	DrawDebugBox(GetWorld(), GetActorLocation(), SpawnExtent, FColor::Green, true, 5.f);
#endif
}

void ASpawnArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ASpawnArea::GetRandomPointInArea() const
{
	const FVector Origin = GetActorLocation();
	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, SpawnExtent);
}