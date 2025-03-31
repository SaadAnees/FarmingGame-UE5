// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FarmingGameState.generated.h"

/**
 * 
 */
UCLASS()
class FARMINGGAME_API AFarmingGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
    AFarmingGameState();

    //Farm Budget
    UPROPERTY(ReplicatedUsing = OnRep_FarmBudget, BlueprintReadOnly, Category = "Economy")
    float FarmBudget;

    UFUNCTION()
    void OnRep_FarmBudget();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_ModifyBudget(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    float GetFarmBudget() const { return FarmBudget; }

    // Rice Crop count
    UPROPERTY(ReplicatedUsing = OnRep_RiceCropCount, BlueprintReadOnly, Category = "Economy")
    int32 RiceCropCount;

    UFUNCTION()
    void OnRep_RiceCropCount();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_AddRiceCropCount(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    float GetRiceCropCount() const { return RiceCropCount; }

    // Wheat Crop count
    UPROPERTY(ReplicatedUsing = OnRep_WheatCropCount, BlueprintReadOnly, Category = "Economy")
    int32 WheatCropCount;

    UFUNCTION()
    void OnRep_WheatCropCount();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_AddWheatCropCount(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    float GetWheatCropCount() const { return WheatCropCount; }

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void SetWheatCropCount(int32 Amount);

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};