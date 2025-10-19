#include "CleanableActor.h"

#include "AbilitySystemComponent.h"
#include "../GASComponent.h"
#include "HereWeGoAgain/HWGACharacterAttributeSet.h"


ACleanableActor::ACleanableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	check(AbilitySystemComponent);
	
}

void ACleanableActor::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeAbilitySystemIfNeeded();
	
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


void ACleanableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ACleanableActor::GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 AbilityLevel)
{
	if (!HasAuthority() || !AbilitySystemComponent || !*AbilityClass) return;

	FGameplayAbilitySpec Spec(AbilityClass, AbilityLevel, INDEX_NONE, this);
	AbilitySystemComponent->GiveAbility(Spec);
}

struct FActiveGameplayEffectHandle ACleanableActor::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass,
	float EffectLevel)
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

void ACleanableActor::InitializeAbilitySystemIfNeeded()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	CreateAndRegisterAttributeSetIfNeeded();
}

void ACleanableActor::CreateAndRegisterAttributeSetIfNeeded()
{
	if (AttributeSet || !AbilitySystemComponent) return;
	
	AttributeSet = NewObject<UAttributeSet>(this, UHWGACharacterAttributeSet::StaticClass());
	if (AttributeSet)
	{
		// Register with ASC so attributes replicate and can be modified by effects
		AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet);
	}
}

void ACleanableActor::GiveStartupAbilities()
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

void ACleanableActor::RemoveStartupAbilities()
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

void ACleanableActor::ApplyStartupEffects()
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

