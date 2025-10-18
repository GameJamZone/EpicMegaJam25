#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HWGAGameMode.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGOAGAIN_API AHWGAGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// This array should only contain arenas but due to the way it's populated it's of type AActor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HereWeGoAgain|Arena")
	TArray<AActor*> AllArenas;
	
	virtual void SelectRandomArenaToActivate() const;
};
