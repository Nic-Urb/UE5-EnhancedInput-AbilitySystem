// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/TemplateGameplayAbility.h"

#include "GameplayAbilitySystem/TemplateAbilitySystemComponent.h"
#include "Player/GameTemplateCharacter.h"
#include "Player/GameTemplateController.h"

UTemplateAbilitySystemComponent* UTemplateGameplayAbility::GetOurAbilitySystemComponentInfo() const
{
	return CurrentActorInfo ? Cast<UTemplateAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr;
}

AGameTemplateCharacter* UTemplateGameplayAbility::GetOurPlayerCharacterInfo() const
{
	return CurrentActorInfo ? Cast<AGameTemplateCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

AGameTemplateController* UTemplateGameplayAbility::GetOurPlayerControllerInfo() const
{
	return CurrentActorInfo ? Cast<AGameTemplateController>(CurrentActorInfo->PlayerController.Get()) : nullptr;
}

void UTemplateGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	OnAbilityAdded();
}

void UTemplateGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	OnAbilityRemoved();
}
