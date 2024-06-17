// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "TemplateAttributeSet.generated.h"

// Fwd declaration
class UTemplateAbilitySystemComponent;

// Define macro for accessing and initializing attributes
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Base attribute set class for the project
 * (Do not use it directly)
 */
UCLASS(Abstract)
class GAMETEMPLATE_API UTemplateAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UTemplateAttributeSet() = default;

	UTemplateAbilitySystemComponent* GetAbilitySystemComponent() const;
};
