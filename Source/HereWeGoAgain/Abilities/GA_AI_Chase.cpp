#include "GA_AI_Chase.h"

#include "AbilityPayloads.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UGA_AI_Chase::UGA_AI_Chase()
{
	// Server-authoritative chase ability
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
}

void UGA_AI_Chase::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (!Spec.SourceObject.Get())
	{
		return;
	}
	
	if (const UAbilityPayloadBase* Payload = Cast<UAbilityPayloadBase>(Spec.SourceObject))
	{
		CooldownGameplayEffectClass = Payload->CooldownEffect;
	}

}

void UGA_AI_Chase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                   const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Get AI Controller
	APawn* Pawn = ActorInfo && ActorInfo->AvatarActor.IsValid()
		? Cast<APawn>(ActorInfo->AvatarActor.Get())
		: nullptr;

	AAIController* AIController = Pawn ? Cast<AAIController>(Pawn->GetController()) : nullptr;
	if (!AIController)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	AActor* TargetActor = GetWorld()->GetFirstPlayerController()->GetCharacter();

	if (!IsValid(TargetActor))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (AIController->IsFollowingAPath())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AIControllerPtr = AIController;
	TargetActorPtr = TargetActor;

	// Optional: set focus while chasing
	AIController->SetFocus(TargetActor);

	// Begin the move
	StartMoveTo(AIController, TargetActor);

	// Optional timeout
	if (MaxChaseTime > 0.f)
	{
		FTimerDelegate TimeoutDelegate;
		TimeoutDelegate.BindUObject(this, &UGA_AI_Chase::OnChaseTimeout);
		AIController->GetWorldTimerManager().SetTimer(TimeoutHandle, TimeoutDelegate, MaxChaseTime, false);
	}
}

void UGA_AI_Chase::StartMoveTo(AAIController* AIController, AActor* TargetActor)
{
	if (!AIController || !TargetActor)
	{
		return;
	}

	AIController->ReceiveMoveCompleted.AddDynamic(this, &UGA_AI_Chase::OnMoveCompleted);
	
	// Request a move towards the actor
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(TargetActor);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	MoveRequest.SetUsePathfinding(true);
	MoveRequest.SetAllowPartialPath(bAllowPartialPath);

	FNavPathSharedPtr OutPath;
	const FPathFollowingRequestResult ReqResult = AIController->MoveTo(MoveRequest, &OutPath);
	
	CurrentRequestId = ReqResult.MoveId;
	
	// If request was immediately blocked/failed, end early
	if (ReqResult.Code == EPathFollowingRequestResult::Failed)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UGA_AI_Chase::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult)
{
	// Ignore unrelated requests
	if (!AIControllerPtr.IsValid() || RequestID != CurrentRequestId)
	{
		return;
	}

	// If we reached the target or path is finished successfully, end ability successfully
	if (MovementResult == EPathFollowingResult::Success)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// If target vanished or move was aborted, cancel
	if (!TargetActorPtr.IsValid() || MovementResult == EPathFollowingResult::Invalid)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
}

void UGA_AI_Chase::OnChaseTimeout()
{
	// Timed out: cancel
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_AI_Chase::StopMove()
{
	if (AAIController* AIController = AIControllerPtr.Get())
	{
		AIController->StopMovement();
		AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UGA_AI_Chase::OnMoveCompleted);
		AIController->GetWorldTimerManager().ClearTimer(TimeoutHandle);
	}
}

void UGA_AI_Chase::EndAbility(const FGameplayAbilitySpecHandle Handle,
                              const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo,
                              bool bReplicateEndAbility, bool bWasCancelled)
{
	StopMove();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_AI_Chase::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 bool bReplicateCancelAbility)
{
	StopMove();
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}