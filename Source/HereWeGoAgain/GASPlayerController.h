// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "GASPlayerController.generated.h"

UCLASS()
class AGASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGASPlayerController();

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

protected:
	// To add mapping context
	virtual void BeginPlay();
};


