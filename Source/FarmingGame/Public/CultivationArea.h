// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Crop.h"
#include "CultivationArea.generated.h"

class AFarmingGameCharacter;
class UBoxComponent;

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

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* MeshComponent;

	// Reference to the planted crop
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Farming")
	ACrop* PlantedCrop;

	// Function to check if a crop exists
	UFUNCTION(BlueprintCallable, Category = "Farming")
	bool HasCrop() const { return PlantedCrop != nullptr; }

	// Function to plant a crop
	UFUNCTION(BlueprintCallable, Category = "Farming")
	void PlantCrop(ACrop* Crop);

	void ClearCrop();

	bool IsPlayerInside();

	ACrop* GetPlantedCrop();


	UPROPERTY(VisibleAnywhere, Category = "Collision")
	UBoxComponent* CollisionBox;

	UPROPERTY()
	TArray<AFarmingGameCharacter*> PlayersInArea;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
