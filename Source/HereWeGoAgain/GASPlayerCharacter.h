// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "GASComponent.h"
#include "GASPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
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

	AGASPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyControllerChanged() override;

	UAbilitySystemComponent* GetAbilitySystemComponent() const
	{
		return AbilitySystemComponent;
	}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=GAS)
	UGASInputConfigDataAsset* InputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TArray<FAbilitySet_GameplayAbility> DefaultAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TArray<TSubclassOf<class UGameplayEffect>> DefaultEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	UGASComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	bool bUsingMouse = false;

private:

	UPROPERTY(editanywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(editanywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, Category="Input")
	TEnumAsByte<ETraceTypeQuery> MouseAimTraceChannel;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	double AimAngle = 0.f;
	
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

