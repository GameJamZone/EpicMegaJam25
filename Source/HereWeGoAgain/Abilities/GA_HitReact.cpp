// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_HitReact.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "GameFramework/RootMotionSource.h"
#include "HereWeGoAgain/ProjectGameplayTags.h"


UGA_HitReact::UGA_HitReact(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag   = ProjectGameplayTags::Event_Combat_Hit;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

}

void UGA_HitReact::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
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

	if (const UAttackHitReactPayload* Payload = Cast<UAttackHitReactPayload>(Spec.SourceObject))
	{
		KnockbackDuration = Payload->KnockbackDuration;
		KnockbackStrength = Payload->KnockbackStrength;
		HitStopDuration = Payload->HitStopDuration;
		HitStopDilation = Payload->HitStopDilation;
		JoltDuration = Payload->JoltDuration;
		JoltCycles = Payload->JoltCycles;
		JoltAngle = Payload->JoltAngle;
	}
}

void UGA_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
	    EndAbility(Handle, ActorInfo, ActivationInfo, true, false); return;
	}

    AActor* Avatar = ActorInfo->AvatarActor.Get();
    if (!Avatar)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false); return;
    }

	AActor* Instigator = TriggerEventData ? const_cast<AActor*>(TriggerEventData->Instigator.Get()) : nullptr;

	// Extract hit direction
	FVector KnockDir = FVector::ZeroVector;
	float Magnitude = KnockbackStrength;

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		FHitResult HR;
		if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TriggerEventData->TargetData, 0))
		{
			HR = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TriggerEventData->TargetData, 0);
			// Push away from instigator -> target, fallback to impact normal
			if (Instigator)
			{
				KnockDir = (Avatar->GetActorLocation() - Instigator->GetActorLocation()).GetSafeNormal();
			}
			if (KnockDir.IsNearlyZero() && HR.Normal.IsNearlyZero() == false)
			{
				KnockDir = (-HR.Normal).GetSafeNormal();
			}
		}
	}
	
    if (ACharacter* Char = Cast<ACharacter>(Avatar))
    {
    	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			Montage,
			1.0f);
    	Task->ReadyForActivation();
    	
		ApplyHitStop(Char, HitStopDilation, HitStopDuration, KnockDir);
    	Jolt(Char, ActorInfo->SkeletalMeshComponent.Get());
    }
    else
    {
        // Non-character: try physics impulse
        if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Avatar->GetRootComponent()))
        {
            if (Prim->IsSimulatingPhysics() && !KnockDir.IsNearlyZero())
            {
                Prim->AddImpulse(KnockDir * Magnitude, NAME_None, true);
            }
        }
    }
	
    EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UGA_HitReact::ApplyHitStop(ACharacter* Char, float Dilation, float DurationSec, FVector KnockDir)
{
	if (!Char || DurationSec <= 0.f) return;
	Dilation = FMath::Clamp(Dilation, 0.05f, 1.0f);

	const float PrevDilation = Char->CustomTimeDilation;
	Char->CustomTimeDilation = Dilation;

	FTimerHandle Restore;
	Char->GetWorldTimerManager().SetTimer(Restore, FTimerDelegate::CreateWeakLambda(Char, [Char, PrevDilation, this, KnockDir]()
	{
		if (IsValid(Char)) Char->CustomTimeDilation = PrevDilation;
		
		if (!KnockDir.IsNearlyZero())
		{
			auto* Task = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
				this,
				NAME_None,
				KnockDir,
				KnockbackStrength,
				KnockbackDuration,
				true,   // bIsAdditive
				nullptr,
				ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
				FVector::ZeroVector,
				0.f,
				false);
			if (Task) Task->ReadyForActivation();
		}
  
		
	}), DurationSec, false);
}

void UGA_HitReact::Jolt(ACharacter* Char, USkeletalMeshComponent* Mesh)
{
	if (!Mesh) return;

	const FRotator BaseRot = Mesh->GetRelativeRotation();
	const double Start = FPlatformTime::Seconds();

	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Mesh, BaseRot, Start, this](float DeltaTime)
	{
		const double Now = FPlatformTime::Seconds();
		const double Elapsed = Now - Start;
		const float Alpha = FMath::Clamp(static_cast<float>(Elapsed / JoltDuration), 0.f, 1.f);
		const float Falloff = 1.f - Alpha;

		// Simple 2-axis sine oscillation with exponential falloff
		const float FrequencyHz = JoltCycles / JoltDuration;
		const float T = static_cast<float>(Now * FrequencyHz * 2.0 * PI);
		const float Pitch = FMath::Sin(T) * JoltAngle * Falloff;
		const float Yaw   = FMath::Cos(T * 0.85f) * JoltAngle * 0.7f * Falloff;

		Mesh->SetRelativeRotation(BaseRot + FRotator(Pitch, Yaw, 0.f));

		if (Elapsed >=  JoltDuration|| !IsValid(Mesh))
		{
			if (IsValid(Mesh)) Mesh->SetRelativeRotation(BaseRot);
			return false; // stop
		}
		return true; // keep
	}));
}


