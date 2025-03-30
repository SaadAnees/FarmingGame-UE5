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


    /** Harvested crop count shared among all players */
    UPROPERTY(ReplicatedUsing = OnRep_HarvestedCrops, BlueprintReadOnly, Category = "Farming")
    int32 HarvestedCrops;

    /** Called when HarvestedCrops is updated on clients */
    UFUNCTION()
    void OnRep_HarvestedCrops();

    /** Modify harvested crops on the server */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_AddHarvestedCrops(int32 Amount);
    void Server_AddHarvestedCrops_Implementation(int32 Amount);
    bool Server_AddHarvestedCrops_Validate(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "StateEconomy")
    float GetHarvestedCrops() const { return HarvestedCrops; }

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

  /*  virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(ReplicatedUsing = OnRep_FarmBudget)
    float FarmBudget;

    UFUNCTION()
    void OnRep_FarmBudget();

    void ModifyBudget(float Amount);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
    void Server_ModifyBudget(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    float GetFarmBudget() const { return FarmBudget; }*/