#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "Abilities/GameplayAbility.h"
#include "Navigation/PathFollowingComponent.h"
#include "GA_AI_Chase.generated.h"

class AAIController;

UCLASS()
class UGA_AI_Chase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_AI_Chase();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// UGameplayAbility
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
	                        const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo,
	                        bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle,
	                           const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo,
	                           bool bReplicateCancelAbility) override;

protected:
	// Movement parameters
	UPROPERTY(EditDefaultsOnly, Category = "Chase")
	float AcceptanceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Chase")
	bool bStopOnOverlap = true;

	UPROPERTY(EditDefaultsOnly, Category = "Chase")
	bool bAllowPartialPath = true;

	// Optional time limit for the chase. 0 = unlimited.
	UPROPERTY(EditDefaultsOnly, Category = "Chase")
	float MaxChaseTime = 3.f;

protected:
	// Internal

	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult);

	UFUNCTION()
	void OnChaseTimeout();

	void StartMoveTo(class AAIController* AIController, class AActor* TargetActor);
	void StopMove();

	// Resolved at activation
	TWeakObjectPtr<AActor> TargetActorPtr;
	TWeakObjectPtr<AAIController> AIControllerPtr;

	// Delegate handles
	FDelegateHandle MoveCompletedHandle;

	// Optional timer for timeout
	FTimerHandle TimeoutHandle;

	// Track the current request
	struct FAIRequestID CurrentRequestId;
};