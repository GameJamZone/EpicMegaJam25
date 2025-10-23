#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"

#include "SpawnableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USpawnableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ISpawnableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void SetActorTag(FGameplayTag ActorTag) = 0;

	UFUNCTION()
	virtual FGameplayTag GetActorTag() const = 0;

	UFUNCTION()
	virtual void UpdateHealth(float Percent) = 0;

	UFUNCTION()
	virtual void DepleteHealth(float Damage) = 0;
};
