// Copyright Epic Games, Inc. All Rights Reserved.

#include "FarmingGameGameMode.h"
#include "FarmingGameCharacter.h"
#include "FarmingGameState.h"
#include "UObject/ConstructorHelpers.h"

AFarmingGameGameMode::AFarmingGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	GameStateClass = AFarmingGameState::StaticClass();
}
