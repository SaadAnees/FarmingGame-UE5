// Fill out your copyright notice in the Description page of Project Settings.


#include "CultivationArea.h"

// Sets default values
ACultivationArea::ACultivationArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlantedCrop = nullptr;

}

// Called when the game starts or when spawned
void ACultivationArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACultivationArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACultivationArea::PlantCrop(AActor* Crop)
{
	if (!PlantedCrop)
	{
		PlantedCrop = Crop;
		UE_LOG(LogTemp, Warning, TEXT("🌱 Crop planted in Cultivation Area!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ This Cultivation Area already has a crop!"));
	}
}

