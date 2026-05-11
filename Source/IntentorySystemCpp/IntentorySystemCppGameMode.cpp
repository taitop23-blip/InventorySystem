// Copyright Epic Games, Inc. All Rights Reserved.

#include "IntentorySystemCppGameMode.h"
#include "IntentorySystemCppCharacter.h"
#include "IntentorySystemCppHUD.h"
#include "UObject/ConstructorHelpers.h"

AIntentorySystemCppGameMode::AIntentorySystemCppGameMode()
{
	HUDClass = AIntentorySystemCppHUD::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
