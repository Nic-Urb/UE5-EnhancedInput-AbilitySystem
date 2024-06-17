// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/GameTemplateCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"

namespace AbilityInputBindingImpl
{
	constexpr int32 InvalidInputID = 0;
	int32 IncrementingInputID = InvalidInputID;

	static int32 GetNextInputID()
	{
		return ++IncrementingInputID;
	}
}


//////////////////////////////////////////////////////////////////////////
// AGameTemplateCharacter

AGameTemplateCharacter::AGameTemplateCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Initialize AbilitySystemComponent, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UTemplateAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	// It's a single player template, so every GE's is replicated to every client
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AGameTemplateCharacter::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (UTemplateGameplayAbilitySet* AbilitySet : AbilitySets)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveAbilities(AbilitySystemComponent, this);
			}
		}
	}
}

void AGameTemplateCharacter::RemoveAbilities()
{
	// Remove any abilities added from previous call
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (UTemplateGameplayAbilitySet* AbilitySet : AbilitySets)
		{
			if (AbilitySet)
			{
				AbilitySet->RemoveAbilities(AbilitySystemComponent,this);
			}
		}
	}
}

void AGameTemplateCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server AbilitySystem init
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	GiveAbilities();
}

void AGameTemplateCharacter::UnPossessed()
{
	Super::UnPossessed();

	RemoveAbilities();
}

void AGameTemplateCharacter::Destroyed()
{
	Super::Destroyed();

	RemoveAbilities();
}

//////////////////////////////////////////////////////////////////////////
// Ability Input handling

void AGameTemplateCharacter::SetInputBinding(UInputAction* InputAction, FGameplayAbilitySpec& AbilitySpec)
{
	using namespace AbilityInputBindingImpl;

	if (AbilitySpec.InputID == InvalidInputID)
	{
		AbilitySpec.InputID = GetNextInputID();
	}

	FAbilityInputBinding* AbilityInputBinding = MappedAbilities.Find(InputAction);
	if (!AbilityInputBinding)
	{
		AbilityInputBinding = &MappedAbilities.Add(InputAction);
	}

	AbilityInputBinding->BoundAbilitiesStack.AddUnique(AbilitySpec.Handle);

	if (EnhancedInputComponent)
	{
		// Pressed event
		if (AbilityInputBinding->OnPressedHandle == 0)
		{
			AbilityInputBinding->OnPressedHandle = EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &AGameTemplateCharacter::OnAbilityInputPressed, InputAction).GetHandle();
		}

		// Released event
		if (AbilityInputBinding->OnReleasedHandle == 0)
		{
			AbilityInputBinding->OnReleasedHandle = EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &AGameTemplateCharacter::OnAbilityInputReleased, InputAction).GetHandle();
		}
	}
}

void AGameTemplateCharacter::ClearInputBinding(FGameplayAbilitySpec& AbilitySpec)
{
	using namespace AbilityInputBindingImpl;

	TArray<UInputAction*> InputActionsToClear;
	for (auto& InputBinding : MappedAbilities)
	{
		if (InputBinding.Value.BoundAbilitiesStack.Find(AbilitySpec.Handle))
		{
			InputActionsToClear.Add(InputBinding.Key);
		}
	}

	for (UInputAction* InputAction : InputActionsToClear)
	{
		FAbilityInputBinding* AbilityInputBinding = MappedAbilities.Find(InputAction);
		if (AbilityInputBinding->BoundAbilitiesStack.Remove(AbilitySpec.Handle) > 0)
		{
			if (AbilityInputBinding->BoundAbilitiesStack.Num() == 0)
			{
				// NOTE: This will invalidate the `AbilityInputBinding` ref above
				RemoveEntry(InputAction);
			}
		}
	}

	AbilitySpec.InputID = InvalidInputID;
}

UAbilitySystemComponent* AGameTemplateCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGameTemplateCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Add input mapping context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, InputPriority);
		}
	}
	
	// Set up action bindings
	if ((EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGameTemplateCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGameTemplateCharacter::Look);
		
		// @NOTE: You can add more actions in here
	}

	// Set up ability system controls
	ResetBinds();
	for (const auto& Ability : MappedAbilities)
	{
		UInputAction* InputAction = Ability.Key;

		// Pressed event
		EnhancedInputComponent->BindAction(InputAction,ETriggerEvent::Started,this,&AGameTemplateCharacter::OnAbilityInputPressed,InputAction);

		// Release event
		EnhancedInputComponent->BindAction(InputAction,ETriggerEvent::Completed,this,&AGameTemplateCharacter::OnAbilityInputReleased,InputAction);
	}
	// Run ability system setup
	RunAbilitySystemSetup();
}

void AGameTemplateCharacter::ResetBinds()
{
	for (auto& InputBinding : MappedAbilities)
	{
		if (EnhancedInputComponent)
		{
			EnhancedInputComponent->RemoveBindingByHandle(InputBinding.Value.OnPressedHandle);
			EnhancedInputComponent->RemoveBindingByHandle(InputBinding.Value.OnReleasedHandle);
		}

		if (AbilitySystemComponent)
		{
			const int32 ExpectedInputID = InputBinding.Value.InputID;

			for (FGameplayAbilitySpecHandle AbilityHandle : InputBinding.Value.BoundAbilitiesStack)
			{
				FGameplayAbilitySpec* FoundAbility = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle);
				if (FoundAbility && FoundAbility->InputID == ExpectedInputID)
				{
					FoundAbility->InputID = AbilityInputBindingImpl::InvalidInputID;
				}
			}
		}
	}
}

void AGameTemplateCharacter::RunAbilitySystemSetup()
{
	for (auto& InputBinding : MappedAbilities)
	{
		const int32 NewInputID = AbilityInputBindingImpl::GetNextInputID();
		InputBinding.Value.InputID = NewInputID;

		for (FGameplayAbilitySpecHandle AbilityHandle : InputBinding.Value.BoundAbilitiesStack)
		{
			FGameplayAbilitySpec* FoundAbility = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle);
			if (FoundAbility != nullptr)
			{
				FoundAbility->InputID = NewInputID;
			}
		}
	}
}

void AGameTemplateCharacter::OnAbilityInputPressed(UInputAction* InputAction)
{
	// The AbilitySystemComponent may not have been valid when we first bound input... try again.
	
	if (!AbilitySystemComponent)
	{
		RunAbilitySystemSetup();
	}

	if (AbilitySystemComponent)
	{
		using namespace AbilityInputBindingImpl;

		FAbilityInputBinding* FoundBinding = MappedAbilities.Find(InputAction);
		if (FoundBinding)
		{
			for (FGameplayAbilitySpecHandle AbilityHandle : FoundBinding->BoundAbilitiesStack)
			{
				FGameplayAbilitySpec* FoundAbility = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle);
				if (FoundAbility != nullptr && ensure(FoundAbility->InputID != InvalidInputID))
				{
					AbilitySystemComponent->AbilityLocalInputPressed(FoundAbility->InputID);
				}
			}
		}
	}
}

void AGameTemplateCharacter::OnAbilityInputReleased(UInputAction* InputAction)
{
	if (AbilitySystemComponent)
	{
		using namespace AbilityInputBindingImpl;

		FAbilityInputBinding* FoundBinding = MappedAbilities.Find(InputAction);
		if (FoundBinding)
		{
			for (FGameplayAbilitySpecHandle AbilityHandle : FoundBinding->BoundAbilitiesStack)
			{
				FGameplayAbilitySpec* FoundAbility = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle);
				if (FoundAbility != nullptr && ensure(FoundAbility->InputID != InvalidInputID))
				{
					AbilitySystemComponent->AbilityLocalInputReleased(FoundAbility->InputID);
				}
			}
		}
	}
}

void AGameTemplateCharacter::RemoveEntry(UInputAction* InputAction)
{
	if (FAbilityInputBinding* Bindings = MappedAbilities.Find(InputAction))
	{
		if (EnhancedInputComponent)
		{
			EnhancedInputComponent->RemoveBindingByHandle(Bindings->OnPressedHandle);
			EnhancedInputComponent->RemoveBindingByHandle(Bindings->OnReleasedHandle);
		}

		for (FGameplayAbilitySpecHandle AbilityHandle : Bindings->BoundAbilitiesStack)
		{
			using namespace AbilityInputBindingImpl;

			FGameplayAbilitySpec* AbilitySpec = FindAbilitySpec(AbilityHandle);
			if (AbilitySpec && AbilitySpec->InputID == Bindings->InputID)
			{
				AbilitySpec->InputID = InvalidInputID;
			}
		}
		MappedAbilities.Remove(InputAction);
	}
}

FGameplayAbilitySpec* AGameTemplateCharacter::FindAbilitySpec(FGameplayAbilitySpecHandle Handle)
{
	FGameplayAbilitySpec* FoundAbility = nullptr;
	if (AbilitySystemComponent)
	{
		FoundAbility = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	}
	return FoundAbility;
}

//////////////////////////////////////////////////////////////////////////
// Input functionality

void AGameTemplateCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGameTemplateCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




