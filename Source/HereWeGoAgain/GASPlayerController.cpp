// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASPlayerController.h"

#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "GASPlayerCharacter.h"


AGASPlayerController::AGASPlayerController()
{
}

void AGASPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// FInputModeGameAndUI InputModeGameAndUI;
	// //InputModeGameOnly.SetConsumeCaptureMouseDown(false);
	// SetInputMode(InputModeGameAndUI);
}

void AGASPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	auto character = Cast<AGASPlayerCharacter>(GetCharacter());

	if (!character) 
	{
		// TODO Log GAS
		return;
	}

	if (!character->AbilitySystemComponent)
	{
		// TODO Log GAS
		return;
	}

	character->AbilitySystemComponent->ProcessAbilityInput(DeltaTime, bGamePaused);

	Super::PostProcessInput(DeltaTime, bGamePaused);
	
}