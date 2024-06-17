// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TemplateAttributeSet.h"
#include "ExampleAttributeSet.generated.h"

/**
 * Example class which shows how to implement a new attribute set
 * (Attribute examples include: stamina)
 */
UCLASS(BlueprintType)
class GAMETEMPLATE_API UExampleAttributeSet : public UTemplateAttributeSet
{
	GENERATED_BODY()
	
public:
	UExampleAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category="Attributes");
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UExampleAttributeSet, Stamina);

	/** Override this function to clamp an attribute value **/
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	
};
