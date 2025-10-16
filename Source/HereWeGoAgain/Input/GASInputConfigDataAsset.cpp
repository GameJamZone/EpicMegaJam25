// Fill out your copyright notice in the Description page of Project Settings.


#include "GASInputConfigDataAsset.h"
#include "Logging/LogCategory.h"
#include "Logging/LogMacros.h"

UGASInputConfigDataAsset::UGASInputConfigDataAsset(const FObjectInitializer& ObjectInitializer)
{

}

const UInputAction* UGASInputConfigDataAsset::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FGASInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfigDataAsset [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UGASInputConfigDataAsset::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FGASInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfigDataAsset [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
