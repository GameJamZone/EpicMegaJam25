// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASPlayerCharacter.h"

#include "UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include <Abilities/GameplayAbility.h>

#include "GASPlayerController.h"
#include "GASPlayerState.h"
#include "ProjectGameplayTags.h"
#include "GameFramework/PlayerState.h"
#include "Input/GASEnhancedInputComponent.h"

AGASPlayerCharacter::AGASPlayerCharacter(const FObjectInitializer& ObjectInitializer)
{
	// Configure character movement
	// bUseControllerRotationYaw = true;
	// GetCharacterMovement()->bOrientRotationToMovement = true;
	// GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
}

void AGASPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AGASPlayerCharacter::GiveToAbilitySystem(FAbilitySet_GrantedHandles* OutGrantedHandles) const
{
	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < DefaultAbilities.Num(); ++AbilityIndex)
	{
		const FAbilitySet_GameplayAbility& AbilityToGrant = DefaultAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			//TODO LOG
			continue;
		}
		
		UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < DefaultEffects.Num(); ++EffectIndex)
	{
		auto& EffectToGrant = DefaultEffects[EffectIndex];

		if (!IsValid(EffectToGrant))
		{
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectToSelf(GameplayEffect, 1, AbilitySystemComponent->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// Would be good to add the AttribSets here.
}

void AGASPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (InputConfig)
	{

		UGASEnhancedInputComponent* EnhancedInputComponent = CastChecked<UGASEnhancedInputComponent>(PlayerInputComponent);
		
		if (EnhancedInputComponent)
		{
			TArray<uint32> BindHandles;
			EnhancedInputComponent->BindAbilityActions(InputConfig, this, &AGASPlayerCharacter::Input_AbilityInputTagPressed, &AGASPlayerCharacter::Input_AbilityInputTagReleased, /*out*/ BindHandles);
			
			EnhancedInputComponent->BindNativeAction(InputConfig, ProjectGameplayTags::Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move,false);
			EnhancedInputComponent->BindNativeAction(InputConfig, ProjectGameplayTags::Input_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look,false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AGASPlayerCharacter::SetupPlayerInputComponent - Failed to set the player input component!"));
		}
	}
}

void AGASPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!AbilitySystemComponent)
	{
		// TODO GAS Log 
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	GiveToAbilitySystem(AbilitySystemComponent->GrantedHandles);
}

void AGASPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!AbilitySystemComponent)
	{
		if (AGASPlayerState* GASPlayerState = Cast<AGASPlayerState>(GetPlayerState()))
		{
			AbilitySystemComponent = GASPlayerState->GetGASAbilitySystemComp();
		}
	}
}


void AGASPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!AbilitySystemComponent)
	{
		if (AGASPlayerState* GASPlayerState = Cast<AGASPlayerState>(GetPlayerState()))
		{
			AbilitySystemComponent = GASPlayerState->GetGASAbilitySystemComp();
		}
	}
}

void AGASPlayerCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!AbilitySystemComponent) 
	{
		// TODO GAS Log 
		return;
	}

	AbilitySystemComponent->AbilityInputTagPressed(InputTag);
}

void AGASPlayerCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!AbilitySystemComponent)
	{
		// TODO GAS Log 
		return;
	}

	AbilitySystemComponent->AbilityInputTagReleased(InputTag);
}

void AGASPlayerCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(MovementDirection, Value.Y);
	}
	
}


void AGASPlayerCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		AddControllerPitchInput(Value.Y);
	}
}
