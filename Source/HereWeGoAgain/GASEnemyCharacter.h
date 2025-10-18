#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GASEnemyCharacter.generated.h"

struct FAbilitySet_GameplayAbility;
class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;

UCLASS()
class AGASEnemyCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGASEnemyCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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

protected:
	// Character
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;

	// GAS bootstrapping
	void InitializeAbilitySystemIfNeeded();
	void CreateAndRegisterAttributeSetIfNeeded();

	// Startup abilities/effects
	void GiveStartupAbilities();
	void RemoveStartupAbilities();
	void ApplyStartupEffects();

protected:
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