// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/TemplateAttributeSet.h"
#include "GameplayAbilitySystem/TemplateAbilitySystemComponent.h"

UTemplateAbilitySystemComponent* UTemplateAttributeSet::GetAbilitySystemComponent() const
{
	return Cast<UTemplateAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
