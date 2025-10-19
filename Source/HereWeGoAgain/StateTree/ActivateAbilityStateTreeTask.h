#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "AbilitySystemComponent.h"
#include "ActivateAbilityStateTreeTask.generated.h"

USTRUCT()
struct FActivateGameplayAbilityTask_InstanceData
{
	GENERATED_BODY()

	// The ability class to activate. You can bind this from the actor in the StateTree editor.
	UPROPERTY(EditAnywhere, Category="Ability", meta=(StateTreeBindable="true"))
	TSubclassOf<UGameplayAbility> AbilityClass;
	
	// If true, the task runs until the ability ends or timeout elapses.
	UPROPERTY(EditAnywhere, Category="Flow")
	bool bWaitForEnd = false;
};

USTRUCT(BlueprintType)
struct FStateTreeTask_ActivateGameplayAbility : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FActivateGameplayAbilityTask_InstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};