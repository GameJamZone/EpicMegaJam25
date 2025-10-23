// Fill out your copyright notice in the Description page of Project Settings.


#include "HWGACharacterAttributeSet.h"

#include "GameplayMessagePayload.h"
#include "GASPlayerCharacter.h"
#include "GASPlayerState.h"
#include "ProjectGameplayTags.h"
#include "Actors/SpawnableInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"

class UGameplayMessageSubsystem;

UHWGACharacterAttributeSet::UHWGACharacterAttributeSet()
{
}

void UHWGACharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHWGACharacterAttributeSet, Health, OldHealth);
}

void UHWGACharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHWGACharacterAttributeSet, MaxHealth, OldMaxHealth);
}

void UHWGACharacterAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHWGACharacterAttributeSet, MovementSpeed, OldMovementSpeed);
}

void UHWGACharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWGACharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWGACharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UHWGACharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UHWGACharacterAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

}

void UHWGACharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	AActor* Actor = GetOwningAbilitySystemComponent()->GetAvatarActor();


	if (Attribute == GetMaxHealthAttribute())
	{
		SetHealth(NewValue);
		if (ISpawnableInterface* SpawnedInterface = Cast<ISpawnableInterface>(Actor))
		{
			const float Percent = NewValue / 10.f;
			SpawnedInterface->UpdateHealth(Percent);
		}
	}
	
	if (Attribute == GetHealthAttribute())
	{
		if (NewValue <= 0.f)
		{
			if (auto* PlayerActor = Cast<AGASPlayerCharacter>(Actor))
			{
				FGameplayMessagePayload Payload;
			
				UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
				FGameplayTag ChannelTag = ProjectGameplayTags::Event_Combat_Hit_Death;
				MessageSubsystem.BroadcastMessage(ChannelTag, Payload);

				return;
			}
			
			if (NewValue != OldValue)
			{
				Actor->Destroy();
			}
		}

		if (NewValue < OldValue)
		{
			if (ISpawnableInterface* SpawnedInterface = Cast<ISpawnableInterface>(Actor))
			{
				const float Damage = (OldValue - NewValue)/ 10.f;
				SpawnedInterface->DepleteHealth(Damage);
			}
		}
	}
	
	if (Attribute == GetBaseMovementSpeedAttribute())
	{
		SetMovementSpeed(NewValue);
	}

	if (Attribute == GetMovementSpeedAttribute())
	{
		const auto* Character = Cast<AGASPlayerCharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor() );

		if (!IsValid(Character))
		{
			return;
		}
		
		Character->GetCharacterMovement()->MaxWalkSpeed = NewValue;
	}
}

void UHWGACharacterAttributeSet::ClampAttributeChange(const FGameplayAttribute Attribute, float NewValue) const
{
}
