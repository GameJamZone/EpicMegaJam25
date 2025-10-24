// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_PlayMontageCombo.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "HereWeGoAgain/HWGACharacterAttributeSet.h"

UGA_PlayMontageCombo::UGA_PlayMontageCombo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = false;
}

void UGA_PlayMontageCombo::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
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
		ComboSections = Payload->ComboSections;
		ComboResetTime = Payload->ComboResetTime;
	}
}

void UGA_PlayMontageCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Decide which section to play (persisted across activations)
	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	const bool bHasCombo = ComboSections.Num() > 0;

	if (bHasCombo)
	{
		if (LastAttackTime < 0.0 || (Now - LastAttackTime) > ComboResetTime)
			ComboIndex = 0;
		else
			ComboIndex = (ComboIndex + 1) % ComboSections.Num();
	}

	const FName StartSection =
		(bHasCombo && ComboSections.IsValidIndex(ComboIndex))
			? ComboSections[ComboIndex]
			: CachedStartSection;

	LastAttackTime = Now;

	float TempCachedPlayRate =  CachedPlayRate * GetAbilitySystemComponentFromActorInfo()->GetSet<UHWGACharacterAttributeSet>()->GetAttackSpeedMultiplier();
	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			CachedMontage,
			TempCachedPlayRate,
			StartSection,
			bCachedStopWhenAbilityEnds,
			CachedAnimRootMotionTranslationScale);

	if (!Task)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Task->OnCompleted.AddDynamic(this, &UGA_PlayMontageCombo::OnMontageCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_PlayMontageCombo::OnMontageInterrupted);
	Task->OnCancelled.AddDynamic(this, &UGA_PlayMontageCombo::OnMontageCancelled);
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

void UGA_PlayMontageCombo::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	bInputBuffered = true;
}

void UGA_PlayMontageCombo::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
}

void UGA_PlayMontageCombo::OnMontageCompleted()
{

	UE_LOG(LogTemp, Warning, TEXT("[ATTACK] Montage completed, ending ability manually."));
	
	// Cache before ending; we’ll re-activate immediately if input was buffered
	UAbilitySystemComponent* ASC = CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr;
	const FGameplayAbilitySpecHandle HandleCopy = CurrentSpecHandle;

	const bool bWantsChain = bInputBuffered;
	bInputBuffered = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

	if (bWantsChain && ASC)
	{
		ASC->TryActivateAbility(HandleCopy);
	}
}

void UGA_PlayMontageCombo::OnMontageCancelled()
{
	UE_LOG(LogTemp, Warning, TEXT("[ATTACK] Montage Cancelled, ending ability manually."));
	UAbilitySystemComponent* ASC = CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr;
	const FGameplayAbilitySpecHandle HandleCopy = CurrentSpecHandle;

	const bool bWantsChain = bInputBuffered;
	bInputBuffered = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

	if (bWantsChain && ASC)
	{
		ASC->TryActivateAbility(HandleCopy);
	}
}

void UGA_PlayMontageCombo::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("[ATTACK] Montage interrupted, ending ability manually."));
	UAbilitySystemComponent* ASC = CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr;
	const FGameplayAbilitySpecHandle HandleCopy = CurrentSpecHandle;

	const bool bWantsChain = bInputBuffered;
	bInputBuffered = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

	if (bWantsChain && ASC)
	{
		ASC->TryActivateAbility(HandleCopy);
	}
}
