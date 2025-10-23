// Fill out your copyright notice in the Description page of Project Settings.
#include "ANS_HitTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/PlayerState.h"
#include "HereWeGoAgain/HWGACharacterAttributeSet.h"
#include "HereWeGoAgain/ProjectGameplayTags.h"
#include "Kismet/GameplayStatics.h"


UANS_HitTrace::UANS_HitTrace(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
}

void UANS_HitTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ActorsToIgnore.Reset(); 
}

void UANS_HitTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	ActorsToIgnore.Reset();
}

void UANS_HitTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	if (!MeshComp) return;
	
	const FTransform LocalStartXform = MeshComp->GetSocketTransform(StartSocketName, RTS_Component);
	const FTransform LocalEndXform   = MeshComp->GetSocketTransform(EndSocketName, RTS_Component);

	// Convert to local positions (component space)
	const FVector LocalStart = LocalStartXform.GetLocation();
	const FVector LocalEnd   = LocalEndXform.GetLocation();

	// Copy component transform now (cheap) — we won't call UpdateComponentToWorld or other unsafe calls.
	const FTransform ComponentXform = MeshComp->GetComponentTransform();

	// Now defer the trace to next tick so the rest of the engine has finalized world transforms.
	// We capture the world-space positions using the component transform we just captured.
	FVector DeferredWorldStart = ComponentXform.TransformPosition(LocalStart);
	FVector DeferredWorldEnd   = ComponentXform.TransformPosition(LocalEnd);

	// DrawDebugSphere(MeshComp->GetWorld(), DeferredWorldStart, 20.f, 12, FColor::Red, false, 0.1f);
	// DrawDebugSphere(MeshComp->GetWorld(), DeferredWorldEnd, 20.f, 12, FColor::Green, false, 0.1f);
	
	if (UWorld* World = MeshComp->GetWorld())
	{
		FTimerDelegate TraceDelegate;
		TraceDelegate.BindLambda([this, MeshComp, DeferredWorldStart, DeferredWorldEnd]()
		{
			if (!MeshComp) return;
			PerformSphereTraceDeferred(MeshComp, DeferredWorldStart, DeferredWorldEnd);
		});

		World->GetTimerManager().SetTimerForNextTick(TraceDelegate);
	}
}

void UANS_HitTrace::PerformSphereTraceDeferred(USkeletalMeshComponent* MeshComp, const FVector& WorldStart, const FVector& WorldEnd)
{
	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::SphereTraceMultiForObjects(
		MeshComp,
		WorldStart,
		WorldEnd,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		DrawDebugType,
		HitResults,
		true,
		FLinearColor::Green,
		FLinearColor::Green,
		1.f
	);

	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.bBlockingHit)
		{

			AActor* HitActor = Hit.GetActor();

			if (bShouldHitOnce)
				ActorsToIgnore.Add(HitActor);
			
			if (bDoHitStun)
			{
				FGameplayEventData EventData;
				EventData.EventTag = ProjectGameplayTags::Event_Combat_Hit;
				EventData.Instigator = MeshComp->GetOwner();
				EventData.Target = HitActor;
				EventData.ContextHandle = FGameplayEffectContextHandle(); // optional
				EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Hit);

				auto* Targetasc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
				if (!Targetasc)
				{
					auto* targetchar = Cast<ACharacter>(HitActor);
					if (auto* TPS = targetchar->GetPlayerState())
					{
						UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TPS, EventData.EventTag, EventData);
					}
				}
				else
				{
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, EventData.EventTag, EventData);
				}
			}
			
			//apply damage gameplay effect 
			if (DamageEffect)
			{
				auto* Owner = Cast<ACharacter>(MeshComp->GetOwner());
				UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);

				APlayerState* PS = Owner->GetPlayerState();
				if (!SourceASC && PS)
				{
					SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
				}
				
				if (SourceASC) 
				{
					FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
					
					AActor* SourceActor = SourceASC->GetOwnerActor();   // e.g. PlayerState or AIController owner
					AActor* AvatarActor = SourceASC->GetAvatarActor();  // usually the Character

					EffectContext.AddInstigator(SourceActor, AvatarActor);
					EffectContext.AddSourceObject(AvatarActor); // optional but often good to include
					EffectContext.AddHitResult(Hit);
					
					FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, 1, EffectContext);
					if (SpecHandle.IsValid())
					{
						auto* Targetasc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
						if (!Targetasc)
						{
							auto* targetchar = Cast<ACharacter>(HitActor);
							if (auto* TPS = targetchar->GetPlayerState())
							{
								Targetasc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TPS);
								
							}
						}

						float DamageMultiplier = SourceASC->GetSet<UHWGACharacterAttributeSet>()->GetAttackMultiplier();
						float FinalDamage = DamageAmount * DamageMultiplier;

						FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
						Spec->SetSetByCallerMagnitude(ProjectGameplayTags::Event_Combat_Hit_Health, -FinalDamage);

						UE_LOG(LogTemp, Warning, TEXT("Applying %f damage to %s"), FinalDamage, *HitActor->GetName());

						SourceASC->ApplyGameplayEffectSpecToTarget(*Spec, Targetasc);
					}
				}
			}
		}
	}
}

