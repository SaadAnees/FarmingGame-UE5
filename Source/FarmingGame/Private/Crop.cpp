// Fill out your copyright notice in the Description page of Project Settings.


#include "Crop.h"

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
	
}

// Called every frame
void ACrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

