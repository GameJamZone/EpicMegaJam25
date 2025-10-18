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

	UFUNCTION()
	void OnMontageCompleted();

public:

	// Montage to play
	UPROPERTY()
	TObjectPtr<UAnimMontage> Montage = nullptr;
	
	UPROPERTY()
	float KnockbackDuration = 0.25f;

	UPROPERTY()
	float KnockbackStrength = 1200.f;

	UPROPERTY()
	float HitStopDuration = 0.07f;

	UPROPERTY()
	float HitStopDilation = 0.06f; // clamp away from zero

	UPROPERTY()
	float JoltDuration = 0.08f;

	UPROPERTY()
	float JoltCycles = 1.75f; // 1.5–2.0 cycles recommended
	
	UPROPERTY()
	float JoltAngle = 8.f;
};
