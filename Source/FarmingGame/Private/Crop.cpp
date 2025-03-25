// Fill out your copyright notice in the Description page of Project Settings.


#include "Crop.h"
#include "TimerManager.h"
#include "CultivationArea.h"

// Sets default values
ACrop::ACrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Add a Static Mesh Component
	UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

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
	Super::Tick(DeltaTime);

}

void ACrop::CheckWateringStatus()
{
	if (CropState == ECropState::Growing)
	{
		CropState = ECropState::NeedsWater;
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
	if (CropState == ECropState::Sowing)
	{
		CropState = ECropState::Growing;
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


