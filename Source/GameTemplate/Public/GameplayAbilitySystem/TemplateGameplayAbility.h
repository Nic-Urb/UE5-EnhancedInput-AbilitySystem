// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TemplateGameplayAbility.generated.h"

// Fwd declaration (to avoid circular dependency)
class UTemplateAbilitySystemComponent;
class AGameTemplateCharacter;
class AGameTemplateController;

/**
 * The base gameplay ability class used by this project
 */
UCLASS(Abstract)
class GAMETEMPLATE_API UTemplateGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UTemplateAbilitySystemComponent;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Ability")
	UTemplateAbilitySystemComponent* GetOurAbilitySystemComponentInfo() const;
	
	UFUNCTION(BlueprintCallable, Category = "Ability")
	AGameTemplateCharacter* GetOurPlayerCharacterInfo() const;
	
	UFUNCTION(BlueprintCallable, Category = "Ability")
	AGameTemplateController* GetOurPlayerControllerInfo() const;

protected:

	/** Overrides **/
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	
	/** Can be called when ability is granted to the ability system component **/
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void OnAbilityAdded();

	/** Can be called when ability is removed from the ability system component **/
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void OnAbilityRemoved();
	
};