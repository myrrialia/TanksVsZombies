// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TankStatics.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "Missile.h"
#include "Turret.generated.h"

class ATank;

UCLASS()
class TANKS_API ATurret : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurret();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE UArrowComponent* GetTurretDirection() { return TurretDirection; }

	/** Name of socket at the muzzle where missiles spawn */
	static const FName MuzzleSocketName;

protected:
	//Projectiles to spawn for Fire1
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret")
	TArray<TSubclassOf<AActor>> Projectiles1;
	
	//Projectiles to spawn for Fire2
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret")
	TArray<TSubclassOf<AActor>> Projectiles2;

private:
	// Helpful debug tool - which way is the turret facing?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* TurretDirection;

	// Sprite for the turret.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* TurretSprite;

	// Tank that owns this turret.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	ATank* Tank;

	//Max turn rate in degrees/second for the turret
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	float YawSpeed;

	//Time to delay between Fire1 commands
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	float Fire1Cooldown;

	//If this value is greater than the current game time, Fire1 is still on cooldown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	float Fire1ReadyTime;

	//Time to delay between Fire2 commands
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	float Fire2Cooldown;

	//If this value is greater than the current game time, Fire2 is still on cooldown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	float Fire2ReadyTime;
};

