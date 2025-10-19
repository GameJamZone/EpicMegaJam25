#include "ActivateAbilityStateTreeTask.h"
#include "Components/StateTreeComponent.h"

EStateTreeRunStatus FStateTreeTask_ActivateGameplayAbility::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FActivateGameplayAbilityTask_InstanceData& InstanceData = Context.GetInstanceData<FActivateGameplayAbilityTask_InstanceData>(*this);
	
	const AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	if (!OwnerActor || !InstanceData.AbilityClass)
		return EStateTreeRunStatus::Failed;

	if (UAbilitySystemComponent* ASC = OwnerActor->FindComponentByClass<UAbilitySystemComponent>())
	{
		if (ASC->TryActivateAbilityByClass(InstanceData.AbilityClass))
		{
			if (InstanceData.bWaitForEnd)
			{
				// Continue ticking until ability ends
				return EStateTreeRunStatus::Running;
			}
			else
			{
				return EStateTreeRunStatus::Succeeded;
			}
		}
	}
	
	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FStateTreeTask_ActivateGameplayAbility::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{

	const FActivateGameplayAbilityTask_InstanceData& InstanceData = Context.GetInstanceData<FActivateGameplayAbilityTask_InstanceData>(*this);
	
	if (!InstanceData.bWaitForEnd)
		return EStateTreeRunStatus::Succeeded;

	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	if (UAbilitySystemComponent* ASC = OwnerActor->FindComponentByClass<UAbilitySystemComponent>())
	{
		const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(InstanceData.AbilityClass);
		if (!Spec || !Spec->IsActive())
		{
			return EStateTreeRunStatus::Succeeded; // Ability ended
		}
	}

	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_ActivateGameplayAbility::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// Nothing to clean up (no bound delegates).
}
