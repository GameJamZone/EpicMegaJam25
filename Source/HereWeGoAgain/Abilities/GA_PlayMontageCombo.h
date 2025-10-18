// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_PlayMontage.h"
#include "GA_PlayMontageCombo.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGOAGAIN_API UGA_PlayMontageCombo : public UGA_PlayMontage
{
	GENERATED_BODY()

public:
	UGA_PlayMontageCombo(const FObjectInitializer& ObjectInitializer);
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	// Combo configuration
	UPROPERTY(EditDefaultsOnly, Category="Combo")
	TArray<FName> ComboSections;

	// Time allowed between presses to keep the combo going
	UPROPERTY(EditDefaultsOnly, Category="Combo", meta=(ClampMin="0.0"))
	float ComboResetTime = 0.6f;

protected:
	// Internal state
	UPROPERTY(Transient)
	int32 ComboIndex = -1;

	UPROPERTY(Transient)
	double LastAttackTime = -1.0;

	UPROPERTY(Transient)
	bool bInputBuffered = false;

	// Optional helper to reset from elsewhere (e.g., when player gets hit)
	UFUNCTION(BlueprintCallable, Category="Combo")
	void ResetCombo() { ComboIndex = -1; LastAttackTime = -1.0; }

	virtual void OnMontageCompleted() override;
	virtual void OnMontageCancelled() override;
	virtual void OnMontageInterrupted() override;
};
