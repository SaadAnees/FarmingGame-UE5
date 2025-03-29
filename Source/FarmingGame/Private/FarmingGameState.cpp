// Fill out your copyright notice in the Description page of Project Settings.


#include "FarmingGameState.h"
#include "Net/UnrealNetwork.h"

AFarmingGameState::AFarmingGameState()
{
    FarmBudget = 1000.0f; // Starting budget
}

void AFarmingGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AFarmingGameState, FarmBudget);
}

void AFarmingGameState::ModifyBudget(float Amount)
{
    if (HasAuthority()) // Server updates the budget
    {
        FarmBudget += Amount;
        OnRep_FarmBudget();
    }
}

void AFarmingGameState::OnRep_FarmBudget()
{
    UE_LOG(LogTemp, Warning, TEXT("Budget updated: %.2f"), FarmBudget);
}


