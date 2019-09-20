// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieController.generated.h"

class AZombie;

/**
 * 
 */
UCLASS()
class TANKS_API AZombieController : public AAIController
{
	GENERATED_BODY()
	
public:

	virtual void Tick(float DeltaTime) override;

	virtual void Possess(APawn* InPawn) override;
	virtual void UnPossess() override;

protected:
	// Our pawn, pre-cast to a zombie. Will be NULL if the pawn is NULL or not a zombie
	UPROPERTY(BlueprintReadOnly, Category = "Zombie")
	AZombie* PawnAsZombie;

};
