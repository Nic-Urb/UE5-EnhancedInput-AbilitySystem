// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TemplateGameplayAbility.h"
#include "TemplateAbilitySystemComponent.generated.h"

/**
 * Base ability system component class used by this project
 */
UCLASS()
class GAMETEMPLATE_API UTemplateAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/** Overrides **/
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	/** Returns a list of currently active ability instances that match the tags **/
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer,TArray<UTemplateGameplayAbility*>& ActiveAbilities);
};
