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


UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class UAttackHitReactPayload : public UAbilityPayloadBase
{
	GENERATED_BODY()

public:
	
	// Montage to play
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Montage")
	TObjectPtr<UAnimMontage> Montage = nullptr;

	//how long knockback last for
	UPROPERTY(EditDefaultsOnly, Category="Knockback")
	float KnockbackDuration = 0.25f;

	//how hard to push actor
	UPROPERTY(EditDefaultsOnly, Category="Knockback")
	float KnockbackStrength = 1200.f;

	//how long hit stop will last for
	UPROPERTY(EditDefaultsOnly, Category="HitStop")
	float HitStopDuration = 0.07f;

	//how much time slowdown/stop  
	UPROPERTY(EditDefaultsOnly, Category="HitStop")
	float HitStopDilation = 0.06f; // clamp away from zero

	//how long to shake actor for
	UPROPERTY(EditDefaultsOnly, Category="Jolt")
	float JoltDuration = 0.08f;

	//frequency of shakes
	UPROPERTY(EditDefaultsOnly, Category="Jolt")
	float JoltCycles = 1.75f; // 1.5–2.0 cycles recommended

	//how much to jolt
	UPROPERTY(EditDefaultsOnly, Category="Jolt")
	float JoltAngle = 8.f;

};
