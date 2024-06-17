// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/TemplateGameplayAbilitySet.h"

#include "GameplayAbilitySystem/TemplateAbilitySystemComponent.h"
#include "Gametemplate/GameTemplate.h"
#include "Player/GameTemplateCharacter.h"

void UTemplateGameplayAbilitySet::GiveAbilities(UTemplateAbilitySystemComponent* Asc, AGameTemplateCharacter* PlayerCharacter)
{
	check(Asc);
	if (!Asc->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets
		return;
	}

	// Grant the gameplay attributes
	for (const FAttributeBindInfo& AttributeBindInfo : Attributes)
	{
		if (!IsValid(AttributeBindInfo.AttributeSet))
		{
			UE_LOG(ProjectLog, Error, TEXT("GrantedGameplayAttribute on ability set [%s] is not valid"), *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(Asc->GetOwner(), AttributeBindInfo.AttributeSet);
		Asc->AddAttributeSetSubobject(NewSet);

		AddAttributeSet(NewSet);
	}

	// Grant the gameplay abilities
	for (const FAbilityBindInfo& AbilityBindInfo : Abilities)
	{
		if (AbilityBindInfo.AbilityClass.IsNull())
		{
			UE_LOG(ProjectLog, Error, TEXT("GrantedGameplayAbility on ability set [%s] is not valid"), *GetNameSafe(this));
			continue;
		}

		UTemplateGameplayAbility* AbilityCDO = AbilityBindInfo.AbilityClass.LoadSynchronous()->GetDefaultObject<UTemplateGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO,AbilityBindInfo.AbilityLevel);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = Asc->GiveAbility(AbilitySpec);

		AddAbilitySpecHandles(AbilitySpecHandle);
		AddAbilitiesSpec(AbilitySpec);

		// Bind ability to the input
		BindAbility(PlayerCharacter,AbilitySpec);
	}

	// Grant the gameplay effects
	for (const FEffectBindInfo& EffectBindInfo : Effects)
	{
		if (!IsValid(EffectBindInfo.GameplayEffect))
		{
			UE_LOG(ProjectLog, Error, TEXT("GrantedGameplayEffect on ability set [%s] is not valid"), *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectBindInfo.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = Asc->ApplyGameplayEffectToSelf(GameplayEffect, EffectBindInfo.EffectLevel, Asc->MakeEffectContext()); 

		AddEffectSpecHandles(GameplayEffectHandle);
	}

	
}

void UTemplateGameplayAbilitySet::RemoveAbilities(UTemplateAbilitySystemComponent* Asc,
	AGameTemplateCharacter* PlayerCharacter)
{
	if (!Asc->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets
		return;
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitySpecHandles)
	{
		// @TODO: Remove this debug print
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("We're removing abilities, which was previously saved as handle."));
		Asc->ClearAbility(AbilitySpecHandle);
	}

	for (FGameplayAbilitySpec& AbilitySpec : AbilitiesSpec)
	{
		// @TODO: Remove this debug print
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Ability has been unbound from input."));
		UnbindAbility(PlayerCharacter,AbilitySpec);
	}

	for (FActiveGameplayEffectHandle& EffectSpecHandle : EffectSpecHandles)
	{
		// @TODO: Remove this debug print
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("We're removing effects, which was previously saved as handle."));
		Asc->RemoveActiveGameplayEffect(EffectSpecHandle);
	}

	for (UAttributeSet* AttributeSet : GrantedAttributeSets)
	{
		// @TODO: Remove this debug print
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Attribute has been removed."));
		Asc->RemoveSpawnedAttribute(AttributeSet);
	}
	
	AbilitySpecHandles.Reset();
	EffectSpecHandles.Reset();
	AbilitiesSpec.Reset();
	GrantedAttributeSets.Reset();
}

void UTemplateGameplayAbilitySet::BindAbility(AGameTemplateCharacter* PlayerCharacter, FGameplayAbilitySpec& Spec) const
{
	check(Spec.Ability);
	check(PlayerCharacter);

	for (const FAbilityBindInfo& BindInfo : Abilities)
	{
		if (BindInfo.AbilityClass.LoadSynchronous() == Spec.Ability->GetClass())
		{
			PlayerCharacter->SetInputBinding(BindInfo.InputAction.LoadSynchronous(),Spec);
		}
	}
}

void UTemplateGameplayAbilitySet::UnbindAbility(AGameTemplateCharacter* PlayerCharacter,
	FGameplayAbilitySpec& Spec) const
{
	check(PlayerCharacter);
	PlayerCharacter->ClearInputBinding(Spec);
}

void UTemplateGameplayAbilitySet::AddAbilitySpecHandles(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void UTemplateGameplayAbilitySet::AddEffectSpecHandles(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		EffectSpecHandles.Add(Handle);
	}
}

void UTemplateGameplayAbilitySet::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void UTemplateGameplayAbilitySet::AddAbilitiesSpec(const FGameplayAbilitySpec& Spec)
{
	AbilitiesSpec.Add(Spec);
}
