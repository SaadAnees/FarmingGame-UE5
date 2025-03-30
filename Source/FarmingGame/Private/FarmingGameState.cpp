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
    
    HarvestedCrops = 0;

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
    //OnRep_HarvestedCrops(); // Call manually on the server
}

bool AFarmingGameState::Server_ModifyBudget_Validate(float Amount)
{
    return true; 
}

void AFarmingGameState::OnRep_HarvestedCrops()
{
    UE_LOG(LogTemp, Warning, TEXT("Harvested Crops Updated: %d"), HarvestedCrops);
}

void AFarmingGameState::Server_AddHarvestedCrops_Implementation(int32 Amount)
{
    HarvestedCrops += Amount;
    UE_LOG(LogTemp, Warning, TEXT("Server_AddHarvestedCrops_Implementation: %d"), HarvestedCrops);
    //OnRep_HarvestedCrops(); // Call manually on the server
}

bool AFarmingGameState::Server_AddHarvestedCrops_Validate(int32 Amount)
{
    return true;
}

void AFarmingGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AFarmingGameState, HarvestedCrops);
    DOREPLIFETIME(AFarmingGameState, FarmBudget);
}



//
//void AFarmingGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//    DOREPLIFETIME(AFarmingGameState, FarmBudget);
//}
//
//void AFarmingGameState::ModifyBudget(float Amount)
//{
//    if (HasAuthority()) // Server updates the budget
//    {
//        FarmBudget -= Amount;
//        OnRep_FarmBudget();
//        UE_LOG(LogTemp, Warning, TEXT("Budget updated: %.2f"), FarmBudget);
//    }
//}
//
//void AFarmingGameState::OnRep_FarmBudget()
//{
//    UE_LOG(LogTemp, Warning, TEXT("Budget updated: %.2f"), FarmBudget);
//}
//
//void AFarmingGameState::Server_ModifyBudget_Implementation(float Amount)
//{
//    FarmBudget -= Amount;
//    if (FarmBudget < 0)
//    {
//        FarmBudget = 0; // Prevent negative budget
//    }
//    OnRep_FarmBudget();
//}
//
//bool AFarmingGameState::Server_ModifyBudget_Validate(float Amount)
//{
//    return true;
//}


