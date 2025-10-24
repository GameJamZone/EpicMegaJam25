// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_PlayMontage.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "HereWeGoAgain/HWGACharacterAttributeSet.h"

UGA_PlayMontage::UGA_PlayMontage(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = false;
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

		if (Payload->CooldownEffect)
		{
			CooldownGameplayEffectClass = Payload->CooldownEffect;
		}
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

	float TempCachedPlayRate =  CachedPlayRate * GetAbilitySystemComponentFromActorInfo()->GetSet<UHWGACharacterAttributeSet>()->GetAttackSpeedMultiplier();
	// Play montage and bind ending callbacks
	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			CachedMontage,
			TempCachedPlayRate,
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

	if (UAnimMontage* MontageAsset = CachedMontage)
	{
		const float EstimatedDuration = MontageAsset->GetPlayLength() / TempCachedPlayRate;
		FTimerHandle CleanupTimer;
		ActorInfo->AvatarActor->GetWorldTimerManager().SetTimer(
			CleanupTimer,
			FTimerDelegate::CreateWeakLambda(this, [this, Handle, ActorInfo, ActivationInfo]()
			{
				if (IsActive())
				{
					EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
				}
			}),
			EstimatedDuration + 0.2f,
			false);
	}
}

void UGA_PlayMontage::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
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
