// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CultivationArea.generated.h"

UCLASS()
class FARMINGGAME_API ACultivationArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACultivationArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Reference to the planted crop
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Farming")
	AActor* PlantedCrop;

	// Function to check if a crop exists
	UFUNCTION(BlueprintCallable, Category = "Farming")
	bool HasCrop() const { return PlantedCrop != nullptr; }

	// Function to plant a crop
	UFUNCTION(BlueprintCallable, Category = "Farming")
	void PlantCrop(AActor* Crop);

	void ClearCrop();

};
