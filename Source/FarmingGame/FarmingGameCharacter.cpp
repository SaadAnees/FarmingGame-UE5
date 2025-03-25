// Copyright Epic Games, Inc. All Rights Reserved.

#include "FarmingGameCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "CultivationArea.h"
#include "Crop.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AFarmingGameCharacter

AFarmingGameCharacter::AFarmingGameCharacter()
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
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	Budget = 1000.0f;

	if (Budget < 0)
	{
		Budget = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFarmingGameCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	//APlayerController* PC = Cast<APlayerController>(GetController());
	//if (PC)
	//{
	//	PC->bShowMouseCursor = true;   // Show cursor
	//	PC->bEnableClickEvents = true; // Enable click detection
	//	PC->bEnableMouseOverEvents = true;
	//}
}



void AFarmingGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFarmingGameCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFarmingGameCharacter::Look);

		EnhancedInputComponent->BindAction(IA_SpawnCrop, ETriggerEvent::Started, this, &AFarmingGameCharacter::SpawnCrop);
		//PlayerInputComponent->BindAction("SpawnCrop", IE_Pressed, this, &AFarmingGameCharacter::SpawnCrop);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFarmingGameCharacter::Move(const FInputActionValue& Value)
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

void AFarmingGameCharacter::Look(const FInputActionValue& Value)
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

void AFarmingGameCharacter::ModifyBudget(float Amount)
{
	Budget += Amount;

	// Ensure budget doesn't go below zero
	if (Budget < 0)
	{
		Budget = 0;
	}

	// Log updated budget
	UE_LOG(LogTemp, Warning, TEXT("💰 Updated Budget: %f"), Budget);
}

void AFarmingGameCharacter::SpawnCrop()
{
	UE_LOG(LogTemp, Warning, TEXT("✅ SpawnCrop() function called!"));

	if (!CropClass)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ CropClass is NULL! Assign BP_Crop in the editor."));
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ PlayerController is NULL!"));
		return;
	}

	int32 ScreenX, ScreenY;
	PC->GetViewportSize(ScreenX, ScreenY);
	FVector2D ScreenCenter(ScreenX * 0.5f, ScreenY * 0.5f);

	FHitResult HitResult;
	bool bHit = PC->GetHitResultAtScreenPosition(ScreenCenter, ECC_Visibility, true, HitResult);

	if (!bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ No hit detected at screen center!"));
		return;
	}

	AActor* HitActor = HitResult.GetActor();

	// Check if the hit actor is a crop for harvesting
	if (HitActor && HitActor->ActorHasTag("Crop"))
	{
		ACrop* Crop = Cast<ACrop>(HitActor);
		if (Crop)
		{
			Crop->Harvest();
			return;
		}
	}

	// If not a crop, check if it's a cultivation area
	if (HitActor && HitActor->ActorHasTag("CultivationArea"))
	{
		UE_LOG(LogTemp, Warning, TEXT("🔍 Hit Cultivation Area: %s"), *HitActor->GetName());

		ACultivationArea* CultivationArea = Cast<ACultivationArea>(HitActor);
		if (!CultivationArea)
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Failed to cast to CultivationArea!"));
			return;
		}

		if (CultivationArea->HasCrop())  // Ensure only one crop per area
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ A crop is already planted in this Cultivation Area!"));
			return;
		}

		// Get crop price
		ACrop* CropTemplate = Cast<ACrop>(CropClass->GetDefaultObject());
		if (!CropTemplate)
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Could not get default crop instance!"));
			return;
		}

		float CropCost = CropTemplate->CropCost;

		if (Budget < CropCost)
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ Not enough budget to plant this crop!"));
			return;
		}

		// Spawn the crop
		FVector SpawnLocation = CultivationArea->GetActorLocation();  // Centered on cultivation area
		FRotator SpawnRotation = FRotator::ZeroRotator;

		AActor* SpawnedCrop = GetWorld()->SpawnActor<AActor>(CropClass, SpawnLocation, SpawnRotation);
		if (SpawnedCrop)
		{
			ModifyBudget(-CropCost);  // Deduct budget
			CultivationArea->PlantCrop(SpawnedCrop);  // Mark as planted
			SpawnedCrop->AttachToActor(CultivationArea, FAttachmentTransformRules::KeepWorldTransform);
			UE_LOG(LogTemp, Warning, TEXT("✅ Crop planted successfully!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Hit object is not a Cultivation Area!"));
	}
}

