// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityPayloads.h"
#include "Abilities/GameplayAbility.h"
#include "GA_PlayMontage.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGOAGAIN_API UGA_PlayMontage : public UGameplayAbility
{
	GENERATED_BODY()

	UGA_PlayMontage(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditDefaultsOnly, Category="Payload")
	TSubclassOf<UAttackMontagePayload> ExpectedPayloadClass = UAttackMontagePayload::StaticClass();
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	// Cached from payload
	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedMontage = nullptr;

	UPROPERTY()
	FName CachedStartSection = NAME_None;

	UPROPERTY()
	float CachedPlayRate = 1.f;

	UPROPERTY()
	bool bCachedStopWhenAbilityEnds = true;

	UPROPERTY()
	float CachedAnimRootMotionTranslationScale = 1.f;

	// Montage callbacks
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();

	UFUNCTION()
	void OnMontageInterrupted();

};
