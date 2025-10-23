// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASPlayerCharacter.h"

#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include <Abilities/GameplayAbility.h>
#include "GASPlayerState.h"
#include "HealthBar.h"
#include "HWGACharacterAttributeSet.h"
#include "HWGAGameMode.h"
#include "ProjectGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/GASEnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/staticmesh.h"

AGASPlayerCharacter::AGASPlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	// Configure character movement
	// bUseControllerRotationYaw = true;
	// GetCharacterMovement()->bOrientRotationToMovement = true;
	// GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);

	SpringArmComponent->SetRelativeRotation(FRotator(-60.0f, 45.f, 0.0f));

	SpringArmComponent->TargetArmLength = 2200.0f;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 1.1f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	DirectionArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DirectionArrow"));
	DirectionArrow->SetupAttachment(RootComponent);
	DirectionArrow->SetVisibility(true);

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarComponent->SetupAttachment(RootComponent);
}

void AGASPlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// set the player controller reference
	PlayerController = Cast<APlayerController>(GetController());
}

void AGASPlayerCharacter::SetDirectionArrowVisibility(bool bIsVisible)
{
	DirectionArrow->SetVisibility(bIsVisible);
}


void AGASPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	// get the current rotation
	const FRotator OldRotation = GetActorRotation();

	// are we aiming with the mouse?
	if (bUsingMouse)
	{
		if (GetLocalViewingPlayerController())
		{
			// get the cursor world location
			FHitResult OutHit; 
			PlayerController->GetHitResultUnderCursorByChannel(MouseAimTraceChannel, true, OutHit);

			// find the aim rotation 
			const FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), OutHit.Location);

			// save the aim angle
			AimAngle = AimRot.Yaw;

			// update the yaw, reuse the pitch and roll
			SetActorRotation(FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll));

		}

	} else {

		// use quaternion interpolation to blend between our current rotation
		// and the desired aim rotation using the shortest path
		const FRotator TargetRot = FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll);

		SetActorRotation(TargetRot);
	}

	AHWGAGameMode* GM = GetWorld() ? Cast<AHWGAGameMode>(GetWorld()->GetAuthGameMode()) : nullptr;
	if (GM)
	{
		FVector MyLocation = DirectionArrow->GetComponentLocation();
		FVector TargetLocation = GM->GetFirstArenaLocation();

		// Direction vector (ignore vertical if you want only yaw rotation)
		FVector Direction = (TargetLocation - MyLocation);
		Direction.Z = 0.f; // Optional – keep the arrow level

		FRotator LookAtRotation = Direction.Rotation() + FRotator(0, 90, 0);

		// Apply rotation
		DirectionArrow->SetWorldRotation(LookAtRotation);
	}
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

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel, INDEX_NONE, AbilityToGrant.Payload);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

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

	InitializeFromPlayerState();
}

void AGASPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeFromPlayerState();
}

void AGASPlayerCharacter::InitializeFromPlayerState()
{
	if (!AbilitySystemComponent)
	{
		if (AGASPlayerState* GASPlayerState = Cast<AGASPlayerState>(GetPlayerState()))
		{
			AbilitySystemComponent = GASPlayerState->GetGASAbilitySystemComp();
			AbilitySystemComponent->InitAbilityActorInfo(GASPlayerState, this);
			//GiveToAbilitySystem(AbilitySystemComponent->GrantedHandles);
		}
	}
}


void AGASPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeFromPlayerState();
	GiveToAbilitySystem(AbilitySystemComponent->GrantedHandles);
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
	const FVector2D Axis = InputActionValue.Get<FVector2D>();

	AimAngle = FMath::RadiansToDegrees(FMath::Atan2(Axis.Y, -Axis.X)) - 45.f;
	
	// calculate the forward component of the input
	FRotator FlatRot = GetControlRotation();
	FlatRot.Pitch = 0.0f;
	FlatRot.Yaw = 45.f;
	
	// apply the forward input
	AddMovementInput(FlatRot.RotateVector(FVector::ForwardVector), Axis.Y);
	
	// apply the right input
	AddMovementInput(FlatRot.RotateVector(FVector::RightVector), Axis.X);
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

void AGASPlayerCharacter::SetActorTag(FGameplayTag ActorTag)
{
	
}

FGameplayTag AGASPlayerCharacter::GetActorTag() const
{
	return FGameplayTag();
}

void AGASPlayerCharacter::UpdateHealth(float Percent)
{
	if (auto* HealthBar = Cast<UHealthBar>(HealthBarComponent->GetWidget()))
	{
		HealthBar->InitialiseHealthBar(Percent);
	}
}

void AGASPlayerCharacter::DepleteHealth(float Damage)
{
	if (auto* HealthBar = Cast<UHealthBar>(HealthBarComponent->GetWidget()))
	{
		HealthBar->DepleteHealthBar(Damage);
	}
}
