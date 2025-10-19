// C++
#pragma once

#include "AITypes.h"
#include "StateTreeConditionBase.h"
#include "GameFramework/Pawn.h"
#include "StateTreeExecutionTypes.h"
#include "IsPlayerInRangeCondition.generated.h"

USTRUCT()
struct FStateTreePlayerInRangeInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	APawn* OwningPawn = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float Range = 600.f;
};

STATETREE_POD_INSTANCEDATA(FStateTreePlayerInRangeInstanceData);

USTRUCT(meta = (DisplayName = "Is Player In Range"))
struct FIsPlayerInRangeCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreePlayerInRangeInstanceData;

	FIsPlayerInRangeCondition() = default;
	explicit FIsPlayerInRangeCondition(const EGenericAICheck InOperator, const EStateTreeCompare InInverts = EStateTreeCompare::Default)
		: bInvert(InInverts == EStateTreeCompare::Invert)
		, Operator(InOperator)
	{}
	
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	// Core condition logic.
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bInvert = false;

	UPROPERTY(EditAnywhere, Category = "Condition", meta = (InvalidEnumValues = "IsTrue"))
	EGenericAICheck Operator = EGenericAICheck::Equal;
};
