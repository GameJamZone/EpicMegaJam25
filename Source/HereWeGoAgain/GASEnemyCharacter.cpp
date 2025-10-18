// C++
// GASEnemyCharacter.cpp

#include "GASEnemyCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GASComponent.h"
#include "HWGACharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"

AGASEnemyCharacter::AGASEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	// ASC lives on the Character for AI/enemies (no PlayerState path needed).
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	check(AbilitySystemComponent);

	// Replication defaults
	SetReplicates(true);
	bAlwaysRelevant = false;

	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AGASEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeAbilitySystemIfNeeded();

	// In case an AI Controller is already present at BeginPlay on server, grant now.
	if (HasAuthority())
	{
		if (!bStartupAbilitiesGiven)
		{
			GiveStartupAbilities();
		}
		if (bApplyDefaultEffectsOnSpawn)
		{
			ApplyStartupEffects();
		}
	}
}

void AGASEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server: ensure ASC is initialized with this pawn as Avatar
	InitializeAbilitySystemIfNeeded();

	if (HasAuthority())
	{
		if (!bStartupAbilitiesGiven)
		{
			GiveStartupAbilities();
		}
		// Effects are often safe to re-apply (e.g., stat init). Guard if you prefer one-time.
		if (bApplyDefaultEffectsOnSpawn)
		{
			ApplyStartupEffects();
		}
	}
}

void AGASEnemyCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	// Client: mirror InitAbilityActorInfo
	InitializeAbilitySystemIfNeeded();
}

void AGASEnemyCharacter::InitializeAbilitySystemIfNeeded()
{
	if (!AbilitySystemComponent) return;

	// If already initialized to this avatar, skip
	const AActor* CurrentAvatar = AbilitySystemComponent->GetAvatarActor_Direct();
	if (CurrentAvatar == this) return;

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	CreateAndRegisterAttributeSetIfNeeded();
}

void AGASEnemyCharacter::CreateAndRegisterAttributeSetIfNeeded()
{
	if (AttributeSet || !AbilitySystemComponent) return;
	
	AttributeSet = NewObject<UAttributeSet>(this, UHWGACharacterAttributeSet::StaticClass());
	if (AttributeSet)
	{
		// Register with ASC so attributes replicate and can be modified by effects
		AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet);
	}
}

void AGASEnemyCharacter::GiveStartupAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent || bStartupAbilitiesGiven)
	{
		return;
	}

	for (FAbilitySet_GameplayAbility& AbilityClass : DefaultAbilities)
	{
		if (!IsValid(AbilityClass.Ability)) continue;

		UGameplayAbility* AbilityCDO = AbilityClass.Ability->GetDefaultObject<UGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityClass.AbilityLevel, INDEX_NONE, AbilityClass.Payload);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityClass.InputTag);

		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}

	bStartupAbilitiesGiven = true;
}

void AGASEnemyCharacter::RemoveStartupAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	TArray<FGameplayAbilitySpecHandle> ToClear;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		// We tagged SourceObject as 'this' when granting; use that to find ours
		if (Spec.SourceObject == this)
		{
			ToClear.Add(Spec.Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& H : ToClear)
	{
		AbilitySystemComponent->ClearAbility(H);
	}

	bStartupAbilitiesGiven = false;
}

void AGASEnemyCharacter::ApplyStartupEffects()
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : DefaultEffects)
	{
		if (!*EffectClass) continue;

		FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
		Ctx.AddSourceObject(this);

		const float Level = 1.f;
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, Level, Ctx);
		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void AGASEnemyCharacter::GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 AbilityLevel)
{
	if (!HasAuthority() || !AbilitySystemComponent || !*AbilityClass) return;

	FGameplayAbilitySpec Spec(AbilityClass, AbilityLevel, INDEX_NONE, this);
	AbilitySystemComponent->GiveAbility(Spec);
}

FActiveGameplayEffectHandle AGASEnemyCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float EffectLevel)
{
	FActiveGameplayEffectHandle Handle;
	if (!HasAuthority() || !AbilitySystemComponent || !*EffectClass) return Handle;

	FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
	Ctx.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, EffectLevel, Ctx);
	if (SpecHandle.IsValid())
	{
		Handle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	return Handle;
}