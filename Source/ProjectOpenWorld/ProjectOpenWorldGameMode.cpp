// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectOpenWorldGameMode.h"
#include "ProjectOpenWorldCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectOpenWorldGameMode::AProjectOpenWorldGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
