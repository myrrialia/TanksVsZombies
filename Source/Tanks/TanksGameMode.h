// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TanksGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API ATanksGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATanksGameMode();

	//Actors should snap to this Z value during gameplay (Tanks, missiles, zombies). Default is 0.
	UPROPERTY(EditAnywhere)
	float PlayInZPlane;

	//Number of zombies in the map
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumOfZombies;

	//Number of mushrooms in the map
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumOfMushrooms;
};
