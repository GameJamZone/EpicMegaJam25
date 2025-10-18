// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_HitTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/GameUserSettings.h"
#include "HereWeGoAgain/ProjectGameplayTags.h"
#include "Kismet/GameplayStatics.h"

void UANS_HitTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	ActorsToIgnore.Reset();
}

void UANS_HitTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	const FVector StartSocket = MeshComp->GetSocketLocation(StartSocketName);
	const FVector EndSocket = MeshComp->GetSocketLocation(EndSocketName);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<FHitResult> HitResults;
	
	UKismetSystemLibrary::SphereTraceMultiForObjects(MeshComp, StartSocket, EndSocket, Radius, ObjectTypes,
	                                                 false, ActorsToIgnore, EDrawDebugTrace::None,
	                                                 HitResults, true, FLinearColor::Red,
	                                                 FLinearColor::Green, 1.f);

	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.bBlockingHit)
		{
			AActor* HitActor = Hit.GetActor();
			ActorsToIgnore.Add(HitActor);

			FGameplayEventData EventData;
			EventData.EventTag = ProjectGameplayTags::Event_Combat_Hit;
			EventData.Instigator = MeshComp->GetOwner();
			EventData.Target = HitActor;
			EventData.ContextHandle = FGameplayEffectContextHandle(); // optional
			EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Hit);

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, EventData.EventTag, EventData);

			UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
			
			if (!AnimInst)
			{
				continue;
			}
			
			// If this notify is inside a montage, Animation will be a UAnimMontage
			if (UAnimMontage* ThisMontage = Cast<UAnimMontage>(Animation))
			{
				// Stop only this montage if it's still playing
				if (AnimInst->Montage_IsPlaying(ThisMontage))
				{
					AnimInst->Montage_Stop(0.15f, ThisMontage);
				}
				else
				{
					// Fallback: stop any playing montage
					AnimInst->StopAllMontages(0.15f);
				}
			}
		}
	}
}
