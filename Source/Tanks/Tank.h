// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TankStatics.h"
#include "DamageInterface.h"
#include "Tank.generated.h"

//This struct covers all possible input schemes
//What the inputs do can vary by tank, but the same inputs will always exist
USTRUCT(BlueprintType)
struct FTankInput
{
	GENERATED_BODY()

public:
	//Sanitized movement input, ready to be used by game logic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	FVector2D MovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	uint32 bFire1 : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	uint32 bFire2 : 1;

	void Sanitize();
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void Fire1(bool bPressed);
	void Fire2(bool bPressed);

private:
	//Private because it's code only. Blueprints should not need to know about this. This should not be used for game logic.
	FVector2D RawMovementInput;
};

UCLASS(abstract)
class TANKS_API ATank : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE const FTankInput& GetCurrentInput() const { return TankInput; }

	//~ Begin IDamage interface
	virtual void ReceiveDamage(int32 IncomingDamage, EDamageType DamageType);
	virtual int32 GetHealthRemaining() { return Health; }
	//~ End IDamage interface
	
	UFUNCTION(BlueprintNativeEvent, Category = "Tank")
	void TankDie(EDamageType DamageType);
	virtual void TankDie_Implementation(EDamageType DamageType);

private:
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void Fire1Pressed();
	void Fire1Released();
	void Fire2Pressed();
	void Fire2Released();

	//Helps find which way the tank is facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* TankDirection;

	//Collision body for the tank
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TankBody;

	//Sprite for tank body
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* TankSprite;

	//The actor used as the turret
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* ChildTurret;

	//Our in-game camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	//Current input for our tank, sanitized in Tick()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	FTankInput TankInput;

protected:
	// Maximum turn rate (degrees/second) of the tank.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
	float YawSpeed;

	// Maximum movement rate (units/second) of the tank.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
	float MoveSpeed;

	// Acceleration for our tank, when player is trying to move or change directions.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
	float MoveAccel;

	// Current Health
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Tank", meta = (ClampMin = "0.0"))
	int32 Health;

	// Collision profile for detecting (via overlaps)the things we crush
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank")
	FName CrushCollisionProfile;

public:
	//Actor to spawn when tank receives damage
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Spill;

};
