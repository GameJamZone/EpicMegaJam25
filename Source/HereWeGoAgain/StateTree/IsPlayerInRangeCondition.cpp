// C++
#include "IsPlayerInRangeCondition.h"
#include "Kismet/GameplayStatics.h"
#include "StateTreeExecutionContext.h"

namespace UE::StateTree::Conditions
{
	template<typename T>
	bool CompareNumbers(const T Left, const T Right, const EGenericAICheck Operator)
	{
		switch (Operator)
		{
		case EGenericAICheck::Equal:
			return Left == Right;
			break;
		case EGenericAICheck::NotEqual:
			return Left != Right;
			break;
		case EGenericAICheck::Less:
			return Left < Right;
			break;
		case EGenericAICheck::LessOrEqual:
			return Left <= Right;
			break;
		case EGenericAICheck::Greater:
			return Left > Right;
			break;
		case EGenericAICheck::GreaterOrEqual:
			return Left >= Right;
			break;
		default:
			ensureMsgf(false, TEXT("Unhandled operator %d"), Operator);
			return false;
			break;
		}
	}
}

bool FIsPlayerInRangeCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	const APawn* ControlledPawn = InstanceData.OwningPawn;
	if (!ControlledPawn)
	{
		return false;
	}

	const UWorld* World = ControlledPawn->GetWorld();
	if (!World)
	{
		return false;
	}

	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!PlayerPawn)
	{
		return false;
	}

	const FVector A = ControlledPawn->GetActorLocation();
	const FVector B = PlayerPawn->GetActorLocation();

	const FVector::FReal Left = FVector::DistSquared(A, B);
	const FVector::FReal Right = FMath::Square(InstanceData.Range);
	const bool bResult = UE::StateTree::Conditions::CompareNumbers<FVector::FReal>(Left, Right, Operator);

	return bResult ^ bInvert;
}
