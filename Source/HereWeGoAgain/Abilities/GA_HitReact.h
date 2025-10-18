// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityPayloads.h"
#include "Abilities/GameplayAbility.h"
#include "GA_HitReact.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGOAGAIN_API UGA_HitReact : public UGameplayAbility
{
	GENERATED_BODY()

	UGA_HitReact(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditDefaultsOnly, Category="Payload")
	TSubclassOf<UAttackMontagePayload> ExpectedPayloadClass = UAttackHitReactPayload::StaticClass();
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	void ApplyHitStop(ACharacter* Char, float Dilation, float DurationSec,	FVector KnockDir);
	
	void Jolt(ACharacter* Char, USkeletalMeshComponent* Mesh);

public:

	// Montage to play
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Montage")
	TObjectPtr<UAnimMontage> Montage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="Knockback")
	float KnockbackDuration = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category="Knockback")
	float KnockbackStrength = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category="HitStop")
	float HitStopDuration = 0.07f;

	UPROPERTY(EditDefaultsOnly, Category="HitStop")
	float HitStopDilation = 0.06f; // clamp away from zero

	UPROPERTY(EditDefaultsOnly, Category="Jolt")
	float JoltDuration = 0.08f;

	UPROPERTY(EditDefaultsOnly, Category="Jolt")
	float JoltCycles = 1.75f; // 1.5–2.0 cycles recommended
	
	UPROPERTY(EditDefaultsOnly, Category="Jolt")
	float JoltAngle = 8.f;
};
