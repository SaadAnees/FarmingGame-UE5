// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Crop.generated.h"

UENUM(BlueprintType)
enum class ECropType : uint8
{
	None      UMETA(DisplayName = "None"),
	Wheat     UMETA(DisplayName = "Wheat"),
	Rice      UMETA(DisplayName = "Rice"),
	Corn      UMETA(DisplayName = "Corn")
};

UENUM(BlueprintType)
enum class ECropState : uint8
{
	Sowing      UMETA(DisplayName = "Sowing"),     
	Growing     UMETA(DisplayName = "Growing"),   
	Ripened     UMETA(DisplayName = "Ripened"),  
	Harvesting  UMETA(DisplayName = "Harvesting"),
	NeedsWater  UMETA(DisplayName = "Needs Water"),
	Withered    UMETA(DisplayName = "Withered")
};

UCLASS()
class FARMINGGAME_API ACrop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrop();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CropState();

	void SetCropState(ECropState NewState);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	float CropCost;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crop")
	ECropType CropType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CropState, Category = "Crop")
	ECropState CropState;

	void CheckWateringStatus();
	void WaterCrop();
	void StartGrowing();
	void GrowCrop();
	void RipenCrop();
	void Harvest();
	void Wither();
	void UpdateCropScale();

private:
	FTimerHandle GrowthTimer;
	FTimerHandle WateringTimer;
	float WateringInterval = 30.0f;
};
