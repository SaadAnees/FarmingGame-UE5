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

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(ReplicatedUsing = OnRep_FarmBudget)
    float FarmBudget;

    UFUNCTION()
    void OnRep_FarmBudget();

    void ModifyBudget(float Amount);
};