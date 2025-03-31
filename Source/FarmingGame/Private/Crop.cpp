// Fill out your copyright notice in the Description page of Project Settings.


#include "Crop.h"
#include "TimerManager.h"
#include "CultivationArea.h"
#include "FarmingGame/FarmingGameCharacter.h"
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
	SetReplicates(true);
	SetReplicateMovement(true);

	CropCost = 100.0f; 
	
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
	if (HasAuthority()) 
	{
		CropState = NewState;
		OnRep_CropState();
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
	if (CropState == ECropState::Sowing)
	{
		CropState = ECropState::Growing;
		UpdateCropScale();
		UE_LOG(LogTemp, Warning, TEXT("Crop is now Growing!"));

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
			CultivationArea->ClearCrop();
		}
		Destroy();
	}
}

void ACrop::Wither()
{
	if (CropState == ECropState::Ripened)
	{
		CropState = ECropState::Withered;
		UE_LOG(LogTemp, Warning, TEXT("Crop has withered!"));
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
	if (!RootComponent)
	{
		UE_LOG(LogTemp, Error, TEXT(" RootComponent is NULL in %s!"), *GetName());
		return;
	}

	FVector NewScale(1.0f, 1.0f, 0.0f);

	switch (CropState)
	{
	case ECropState::Sowing:
		NewScale = FVector(1.0f, 1.0f, 0.3f);
		break;
	case ECropState::Growing:
		NewScale = FVector(1.0f, 1.0f, 0.6f);
		break;
	case ECropState::Ripened:
		NewScale = FVector(1.0f, 1.0f, 1.0f);
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("New Scale: %s"), *NewScale.ToString());

	SetActorScale3D(NewScale);
}



