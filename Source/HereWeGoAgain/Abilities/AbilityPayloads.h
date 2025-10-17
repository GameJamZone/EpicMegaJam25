#pragma once

#include "AbilityPayloads.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class UAbilityPayloadBase: public UObject
{
	GENERATED_BODY()
public:
	
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class UAttackMontagePayload : public UAbilityPayloadBase
{
	GENERATED_BODY()

public:
	// Montage to play
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Montage")
	TObjectPtr<UAnimMontage> Montage = nullptr;

	// Optional: start at a specific section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Montage")
	FName StartSection = NAME_None;

	// Play rate multiplier
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Montage", meta=(ClampMin="0.01"))
	float PlayRate = 1.f;

	// If true, the montage stops automatically when the ability ends
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Montage")
	bool bStopWhenAbilityEnds = true;

	// Scales root motion translation while the montage plays (1.0 = unchanged)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Montage", meta=(ClampMin="0.0"))
	float AnimRootMotionTranslationScale = 1.f;
};
