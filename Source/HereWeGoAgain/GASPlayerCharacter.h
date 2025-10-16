// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "GASComponent.h"
#include "GASPlayerCharacter.generated.h"

struct FInputActionValue;
class UGASInputConfigDataAsset;

/**
 * Base class for both AI and Player characters.
 */
UCLASS(Blueprintable)
class AGASPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=GAS)
	UGASInputConfigDataAsset* InputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TArray<FAbilitySet_GameplayAbility> DefaultAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TArray<TSubclassOf<class UGameplayEffect>> DefaultEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
	UGASComponent* AbilitySystemComponent;

	AGASPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	UAbilitySystemComponent* GetAbilitySystemComponent() const
	{
		return AbilitySystemComponent;
	}
	
protected:
	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	virtual void GiveToAbilitySystem(FAbilitySet_GrantedHandles* OutGrantedHandles) const;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	virtual void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
};

