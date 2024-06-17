// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h" // for FGameplayAbilitySpecHandle
#include "EnhancedInputComponent.h" // for FInputBindingHandle
#include "GameplayAbilitySystem/TemplateGameplayAbilitySet.h"
#include "GameplayAbilitySystem/TemplateAbilitySystemComponent.h"

#include "GameTemplateCharacter.generated.h"

// Forward declaration
class UEnhancedInputLocalPlayerSubsystem;

USTRUCT()
struct FAbilityInputBinding
{
	GENERATED_BODY()

	int32  InputID = 0;
	uint32 OnPressedHandle = 0;
	uint32 OnReleasedHandle = 0;
	TArray<FGameplayAbilitySpecHandle> BoundAbilitiesStack;
};

/*
 * The base player character class used by this project
 * (Used to handle and store input and ability system)
 */
UCLASS(config=Game)
class AGameTemplateCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:

	/** The component used to handle ability system interaction **/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UTemplateAbilitySystemComponent* AbilitySystemComponent;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FollowCamera;

	/** Ability sets for this character ability system **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
	TArray<UTemplateGameplayAbilitySet*> AbilitySets; 
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* LookAction;

public:
	AGameTemplateCharacter();

	void GiveAbilities();
	//@NOTE: Remove for example on abilities swapping, it is also called pawn it's detached or destroyed from a controller
	void RemoveAbilities();

	/** Overrides **/
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void Destroyed() override;

	/** Ability input binding **/
	void SetInputBinding(UInputAction* InputAction, FGameplayAbilitySpec& AbilitySpec);
	void ClearInputBinding(FGameplayAbilitySpec& AbilitySpec);
	
	/** Implement IAbilitySystemInterface **/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void ResetBinds();
	void RunAbilitySystemSetup();
	void OnAbilityInputPressed(UInputAction* InputAction);
	void OnAbilityInputReleased(UInputAction* InputAction);

	void RemoveEntry(UInputAction* InputAction);

	FGameplayAbilitySpec* FindAbilitySpec(FGameplayAbilitySpecHandle Handle);	
private:
	UPROPERTY(transient)
	UEnhancedInputComponent* EnhancedInputComponent;

	UPROPERTY(transient)
	TMap<UInputAction*, FAbilityInputBinding> MappedAbilities;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

