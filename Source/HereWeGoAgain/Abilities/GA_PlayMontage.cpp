// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_PlayMontage.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_PlayMontage::UGA_PlayMontage(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_PlayMontage::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (!Spec.SourceObject.Get())
	{
		return;
	}

	if (ExpectedPayloadClass && !Spec.SourceObject->IsA(ExpectedPayloadClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("Payload type mismatch for %s"), *GetName());
		return;
	}

	if (const UAttackMontagePayload* Payload = Cast<UAttackMontagePayload>(Spec.SourceObject))
	{
		CachedMontage = Payload->Montage;
		CachedStartSection = Payload->StartSection;
		CachedPlayRate = Payload->PlayRate;
		bCachedStopWhenAbilityEnds = Payload->bStopWhenAbilityEnds;
		CachedAnimRootMotionTranslationScale = Payload->AnimRootMotionTranslationScale;
	}

}

void UGA_PlayMontage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!CachedMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("No montage set for %s"), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Play montage and bind ending callbacks
	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			CachedMontage,
			CachedPlayRate,
			CachedStartSection,
			bCachedStopWhenAbilityEnds,
			CachedAnimRootMotionTranslationScale);

	if (!Task)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Task->OnCompleted.AddDynamic(this, &UGA_PlayMontage::OnMontageCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_PlayMontage::OnMontageInterrupted);
	Task->OnCancelled.AddDynamic(this, &UGA_PlayMontage::OnMontageCancelled);
	Task->ReadyForActivation();

}

void UGA_PlayMontage::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_PlayMontage::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_PlayMontage::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
