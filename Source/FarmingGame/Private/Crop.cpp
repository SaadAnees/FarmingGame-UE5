// Fill out your copyright notice in the Description page of Project Settings.


#include "Crop.h"
#include "TimerManager.h"
#include "CultivationArea.h"
#include "FarmingGameState.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACrop::ACrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Add a Static Mesh Component
	UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	bAlwaysRelevant = true;
	bReplicates = true;
	/*SetReplicates(true);
	SetReplicateMovement(true);*/
	CropCost = 100.0f; // Default price for crops
	
}

// Called when the game starts or when spawned
void ACrop::BeginPlay()
{
	Super::BeginPlay();
	StartGrowing();
	GetWorldTimerManager().SetTimer(WateringTimer, this, &ACrop::CheckWateringStatus, WateringInterval, true);
}

// Called every frame
void ACrop::Tick(float DeltaTime)
{
	

}

void ACrop::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACrop, CropState);
}

void ACrop::OnRep_CropState()
{
	// This function is automatically called when CropState changes
	UE_LOG(LogTemp, Warning, TEXT("Crop state updated to: %d"), (int32)CropState);
}

void ACrop::SetCropState(ECropState NewState)
{
	if (HasAuthority()) // Only the server should update state
	{
		CropState = NewState;
		OnRep_CropState(); // Call manually on the server
	}
}

void ACrop::CheckWateringStatus()
{
	if (CropState == ECropState::Growing)
	{
		CropState = ECropState::NeedsWater;
		UpdateCropScale();
		UE_LOG(LogTemp, Warning, TEXT("Crop needs water!"));
	}
}

void ACrop::StartGrowing()
{
	// Start with Sowing -> Growing
	GetWorldTimerManager().SetTimer(GrowthTimer, this, &ACrop::GrowCrop, 5.0f, false);
}

void ACrop::GrowCrop()
{
	AFarmingGameState* GameState = GetWorld()->GetGameState<AFarmingGameState>();
	if (GameState)
	{
		GameState->Server_AddHarvestedCrops(5); // Increase harvested crops by 1
	}

	if (CropState == ECropState::Sowing)
	{
		CropState = ECropState::Growing;
		UpdateCropScale();
		UE_LOG(LogTemp, Warning, TEXT("Crop is now Growing!"));

		// Schedule the next state change (Growing -> Ripened)
		GetWorldTimerManager().SetTimer(GrowthTimer, this, &ACrop::RipenCrop, 10.0f, false);
	}
}

void ACrop::RipenCrop()
{
	if (CropState == ECropState::Growing)
	{
		CropState = ECropState::Ripened;
		UpdateCropScale();
		UE_LOG(LogTemp, Warning, TEXT("Crop is now Ripened and ready to harvest!"));
	}
}

void ACrop::Harvest()
{
	if (CropState == ECropState::Ripened)
	{
		CropState = ECropState::Harvesting;
		UE_LOG(LogTemp, Warning, TEXT("Harvesting Crop!"));
		ACultivationArea* CultivationArea = Cast<ACultivationArea>(GetAttachParentActor());
		if (CultivationArea)
		{
			CultivationArea->ClearCrop();  // Notify the cultivation area
		}
		// Destroy after harvesting or reset for replanting
		Destroy();
	}
}

void ACrop::Wither()
{
	if (CropState == ECropState::Ripened)
	{
		CropState = ECropState::Withered;
		//GetWorldTimerManager().ClearTimer(GrowthTimer);
		UE_LOG(LogTemp, Warning, TEXT("☠️ Crop has withered!"));
	}
}

void ACrop::WaterCrop()
{
	if (CropState == ECropState::NeedsWater)
	{
		CropState = ECropState::Growing;
		GetWorldTimerManager().ClearTimer(WateringTimer);
		GetWorldTimerManager().SetTimer(WateringTimer, this, &ACrop::CheckWateringStatus, WateringInterval, true);
		UE_LOG(LogTemp, Warning, TEXT("Crop watered!"));
	}
}

void ACrop::UpdateCropScale()
{
	/*FVector NewScale;

	switch (CropState)
	{
	case ECropState::Sowing:
		NewScale = FVector(0.3f);
		break;
	case ECropState::Growing:
		NewScale = FVector(0.6f);
		break;
	case ECropState::Ripened:
		NewScale = FVector(1.0f);
		break;
	}
	UE_LOG(LogTemp, Warning, TEXT("📐 New Scale: %s"), *NewScale.ToString());
	RootComponent->SetRelativeScale3D(NewScale);*/
}


