// Fill out your copyright notice in the Description page of Project Settings.


#include "FarmingGameState.h"
#include "Net/UnrealNetwork.h"

AFarmingGameState::AFarmingGameState()
{
    FarmBudget = 1000.0f; // Starting budget
    if (FarmBudget < 0)
    {
        FarmBudget = 0;
    }
    
    RiceCropCount = 0;
    WheatCropCount = 0;
    SetReplicates(true);
}


void AFarmingGameState::OnRep_FarmBudget()
{
    UE_LOG(LogTemp, Warning, TEXT("Farm Budget Updated: %f"), FarmBudget);
}

void AFarmingGameState::Server_ModifyBudget_Implementation(float Amount)
{
    FarmBudget -= Amount;
    UE_LOG(LogTemp, Warning, TEXT("Server_ModifyBudget_Implementation: %f"), FarmBudget);
}

/// <summary>
/// 
/// </summary>
void AFarmingGameState::OnRep_RiceCropCount()
{
    UE_LOG(LogTemp, Warning, TEXT("Harvested Crops Updated: %d"), RiceCropCount);
}

void AFarmingGameState::Server_AddRiceCropCount_Implementation(int32 Amount)
{
    RiceCropCount += Amount;
    UE_LOG(LogTemp, Warning, TEXT("Server_AddHarvestedCrops_Implementation: %d"), RiceCropCount);
}

bool AFarmingGameState::Server_AddRiceCropCount_Validate(int32 Amount)
{
    return true;
}


void AFarmingGameState::OnRep_WheatCropCount()
{
    UE_LOG(LogTemp, Warning, TEXT("Wheat Crop Crops Updated: %d"), WheatCropCount);
}

void AFarmingGameState::Server_AddWheatCropCount_Implementation(int32 Amount)
{
    WheatCropCount += Amount;
    UE_LOG(LogTemp, Warning, TEXT("Server_AddWheatCropCount_Implementation: %d"), WheatCropCount);
}

bool AFarmingGameState::Server_AddWheatCropCount_Validate(int32 Amount)
{
    return true;
}

void AFarmingGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AFarmingGameState, FarmBudget);
    DOREPLIFETIME(AFarmingGameState, RiceCropCount);
    DOREPLIFETIME(AFarmingGameState, WheatCropCount);
}


