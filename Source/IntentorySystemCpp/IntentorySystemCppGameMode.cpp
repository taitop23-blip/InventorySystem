// Copyright Epic Games, Inc. All Rights Reserved.

#include "IntentorySystemCppGameMode.h"
#include "IntentorySystemCppCharacter.h"
#include "UObject/ConstructorHelpers.h"

AIntentorySystemCppGameMode::AIntentorySystemCppGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
