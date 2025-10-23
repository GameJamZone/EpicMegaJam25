#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "SpawnableInterface.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Actor.h"
#include "HereWeGoAgain/GASComponent.h"
#include "CleanableActor.generated.h"


class UWidgetComponent;
class UAttributeSet;

UCLASS()
class HEREWEGOAGAIN_API ACleanableActor : public AActor, public IAbilitySystemInterface, public ISpawnableInterface
{
	GENERATED_BODY()
	
public:	
	ACleanableActor();
	
	virtual void Destroyed() override
	{
		OnDestroyed.Broadcast(this);
		Super::Destroyed();
	}

	virtual void SetActorTag(FGameplayTag ActorTag) override
	{
		ActorTypeTag = ActorTag;
	};

	virtual FGameplayTag GetActorTag() const override
	{
		return ActorTypeTag;
	};

	virtual void UpdateHealth(float Percent) override;

	virtual void DepleteHealth(float Damage) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	// Optional accessor, if you need to fetch the created attribute set instance.
	UFUNCTION(BlueprintCallable, Category="GAS")
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	// Give one ability at runtime (server-only)
	UFUNCTION(BlueprintCallable, Category="GAS")
	void GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 AbilityLevel = 1);

	// Apply one effect to self at runtime (server-only)
	UFUNCTION(BlueprintCallable, Category="GAS")
	struct FActiveGameplayEffectHandle ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float EffectLevel = 1.f);

	UPROPERTY(editanywhere, BlueprintReadOnly, Category = "HealthBar")
	TObjectPtr<UWidgetComponent> HealthBarComponent;

protected:
	// GAS bootstrapping
	void InitializeAbilitySystemIfNeeded();
	void CreateAndRegisterAttributeSetIfNeeded();

	// Startup abilities/effects
	void GiveStartupAbilities();
	void RemoveStartupAbilities();
	void ApplyStartupEffects();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cleanable)
	FGameplayTag ActorTypeTag;
	
	// Core GAS components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// Optional: created at runtime if DefaultAttributeSetClass is set
	UPROPERTY(Transient)
	UAttributeSet* AttributeSet;

	// Editor-configurable defaults
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UAttributeSet> DefaultAttributeSetClass;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TArray<FAbilitySet_GameplayAbility> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

	// Default level used for abilities unless overridden
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	int32 DefaultAbilityLevel = 1;

	// Internal guard so we don't double-grant
	UPROPERTY(Transient)
	bool bStartupAbilitiesGiven = false;

	// If true, apply DefaultEffects once after ASC init (server-only)
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	bool bApplyDefaultEffectsOnSpawn = true;
};
