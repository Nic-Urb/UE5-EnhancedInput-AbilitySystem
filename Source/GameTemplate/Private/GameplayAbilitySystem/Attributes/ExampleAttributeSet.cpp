// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/ExampleAttributeSet.h"

UExampleAttributeSet::UExampleAttributeSet()
	: Stamina(100.0f)
{
}

void UExampleAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// Clamp stamina value
	if (Attribute == GetStaminaAttribute())
	{
		// Do not allow health to go negative or above max stamina
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	}
}
