// Fill out your copyright notice in the Description page of Project Settings.


#include "FarmingGameState.h"
#include "Net/UnrealNetwork.h"

AFarmingGameState::AFarmingGameState()
{
    //FarmBudget = 1000.0f; // Starting budget
    //if (FarmBudget < 0)
    //{
    //    FarmBudget = 0;
    //}
    SetReplicates(true);
    HarvestedCrops = 0;
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
    return true; // You can add validation if needed
}

void AFarmingGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AFarmingGameState, HarvestedCrops);
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


