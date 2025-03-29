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
#include "FarmingGameState.h"
#include "Net/UnrealNetwork.h"
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
	
	SetReplicates(true);
	SetReplicateMovement(true);
	
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

		// Spawn crop
		//EnhancedInputComponent->BindAction(IA_SpawnCrop, ETriggerEvent::Started, this, &AFarmingGameCharacter::SpawnCrop);

		// Bind planting action
		//PlayerInputComponent->BindAction("PlantCrop", IE_Pressed, this, &AFarmingGameCharacter::TryPlantCrop);
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
	if (HasAuthority()) {
		Budget += Amount;

		// Ensure budget doesn't go below zero
		if (Budget < 0)
		{
			Budget = 0;
		}

		// Log updated budget
		UE_LOG(LogTemp, Warning, TEXT("💰 Updated Budget: %f"), Budget);
	}
}

void AFarmingGameCharacter::SpawnCrop(ECropType SelectedCropType)
{
	if (!HasAuthority()) return; // Ensure this only runs on the server

	if (!CultivationArea)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ CultivationArea is NULL! Cannot plant."));
		return;
	}

	// ✅ Prevent planting if a crop is already present
	if (CultivationArea->HasCrop())
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ This Cultivation Area already has a crop!"));
		return;
	}

	FVector SpawnLocation = CultivationArea->GetActorLocation();
	FRotator SpawnRotation = FRotator::ZeroRotator;

	TSubclassOf<ACrop> CropClass = (SelectedCropType == ECropType::Wheat) ? WheatCropClass : RiceCropClass;
	if (!CropClass)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ No valid crop class selected!"));
		return;
	}

	// ✅ Spawn crop at CultivationArea center
	ACrop* NewCrop = GetWorld()->SpawnActor<ACrop>(CropClass, SpawnLocation, SpawnRotation);
	if (NewCrop)
	{
		NewCrop->SetReplicates(true);
		CultivationArea->PlantCrop(NewCrop);  // ✅ Track the planted crop
		UE_LOG(LogTemp, Warning, TEXT("🌱 Crop planted successfully at (%s)!"), *SpawnLocation.ToString());
	}
}




void AFarmingGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFarmingGameCharacter, Budget);
}

void AFarmingGameCharacter::Server_HarvestCrop_Implementation(ACrop* CropToHarvest)
{
	if (CropToHarvest)
	{
		CropToHarvest->Destroy();
	}
}

bool AFarmingGameCharacter::Server_HarvestCrop_Validate(ACrop* CropToHarvest)
{
	return true; // Simple validation
}

void AFarmingGameCharacter::Server_SpawnCrop_Implementation(ECropType SelectedCropType)
{
	if (!HasAuthority()) return;

	if (!CultivationArea)  // ✅ Prevent calling IsPlayerInside on null CultivationArea
	{
		UE_LOG(LogTemp, Error, TEXT("❌ CultivationArea is NULL! Cannot plant."));
		return;
	}

	if (CultivationArea->IsPlayerInside())
	{
		UE_LOG(LogTemp, Warning, TEXT("✅ Player is inside, proceeding to plant crop."));
		SpawnCrop(SelectedCropType);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ You must be inside a Cultivation Area to plant crops!"));
	}

	
}

bool AFarmingGameCharacter::Server_SpawnCrop_Validate(ECropType SelectedCropType)
{
	return true; // Add validation logic if needed
}

void AFarmingGameCharacter::OnRep_Budget()
{
	UE_LOG(LogTemp, Warning, TEXT("💰 Budget updated on client: %f"), Budget);
}






