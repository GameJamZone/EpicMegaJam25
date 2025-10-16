// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GASComponent.h"
#include "GameFramework/PlayerState.h"
#include "GASPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AGASPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGASPlayerState();

	UGASComponent* GetGASAbilitySystemComp() const
	{
		return AbilitySystemComponent;
	}
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return GetGASAbilitySystemComp();
	}
	
private:
	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = GAS_CATEGORY)
	TObjectPtr<UGASComponent> AbilitySystemComponent;
};
