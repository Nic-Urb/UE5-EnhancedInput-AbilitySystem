// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TemplateGameplayAbility.h"
#include "TemplateGameplayAbilitySet.generated.h"

// Fwd declaration
class UTemplateAbilitySystemComponent;
class UInputAction;

/**
 *	Data used by the ability set to grant gameplay ability
 */
USTRUCT()
struct FAbilityBindInfo
{
	GENERATED_BODY()

	/** Ability to grant **/
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UTemplateGameplayAbility> AbilityClass = nullptr;

	/** Level of ability to grant **/
	UPROPERTY(EditDefaultsOnly)
	uint32 AbilityLevel = 1;
	
	/** Input action to process input for the ability **/
	UPROPERTY(EditDefaultsOnly, meta=(Categories="InputAction"))
	TSoftObjectPtr<UInputAction> InputAction = nullptr;
};

/**
 *	Data used by the ability set to grant gameplay effects.
 */
USTRUCT()
struct FEffectBindInfo
{
	GENERATED_BODY()

	/** Gameplay effect to grant **/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	/** Level of gameplay effect to grant **/
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/**
 *	Data used by the ability set to grant attribute sets.
 */
USTRUCT()
struct FAttributeBindInfo
{
	GENERATED_BODY()

	/** Gameplay effect to grant **/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};

/**
 * Data asset used to grant gameplay ability
 */
UCLASS(BlueprintType)
class GAMETEMPLATE_API UTemplateGameplayAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

	/** Abilities to add to the ASC **/
	UPROPERTY(EditDefaultsOnly, Category="AbilitySet")
	TArray<FAbilityBindInfo> Abilities;

	/** Effect to add to the ASC **/
	UPROPERTY(EditDefaultsOnly, Category="AbilitySet")
	TArray<FEffectBindInfo> Effects;

	/** Attributes to add to the ASC **/
	UPROPERTY(EditDefaultsOnly, Category="AbilitySet")
	TArray<FAttributeBindInfo> Attributes;
	
public:
	void GiveAbilities(UTemplateAbilitySystemComponent* Asc,AGameTemplateCharacter* PlayerCharacter);
	void RemoveAbilities(UTemplateAbilitySystemComponent* Asc,AGameTemplateCharacter* PlayerCharacter);

private:
	void BindAbility(AGameTemplateCharacter* PlayerCharacter, struct FGameplayAbilitySpec& Spec) const;
	void UnbindAbility(AGameTemplateCharacter* PlayerCharacter, struct FGameplayAbilitySpec& Spec) const;
	
	void AddAbilitySpecHandles(const FGameplayAbilitySpecHandle& Handle);
	void AddEffectSpecHandles(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);
	void AddAbilitiesSpec(const FGameplayAbilitySpec& Spec);

private:
	/** Stored handles to the granted abilities **/
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	/** Stored handles to the granted effects **/
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> EffectSpecHandles;

	/** Stored spec to the granted abilities **/
	UPROPERTY()
	TArray<FGameplayAbilitySpec> AbilitiesSpec;

	// Pointers to the granted attribute sets
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};


