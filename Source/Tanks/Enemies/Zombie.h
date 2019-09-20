// Copyright 2019 Nicky Argiropoulos

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageInterface.h"
#include "Components/CapsuleComponent.h"
#include "Zombie.generated.h"

class ATank;

UCLASS()
class TANKS_API AZombie : public APawn, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AZombie();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Set this zombie's target. The base version of this function will handle updating TargetActor and TargetTank
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTarget(AActor* NewTarget);

	//Return the target actor. Returns NULL if no target
	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTarget();

	//Return the target actor as a Tank, if possible. Returns NULL if no target or target is not a tank
	UFUNCTION(BlueprintCallable, Category = "AI")
	ATank* GetTargetAsTank();

	// Zombies will call this on Tick.
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	void ZombieAI(float DeltaSeconds);
	virtual void ZombieAI_Implementation(float DeltaSeconds);

	//Asks the zombie if it is in position to attack its current target - Does not actually attack!
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	bool ZombieAIShouldAttack();
	virtual bool ZombieAIShouldAttack_Implementation();

	// This can be used to animate walking.
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void ZombieWalk(float DeltaSeconds, FVector DistanceWalkedThisFrame);

	// This can be used to start attack animations.
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void ZombieAttack(float DeltaSeconds);

	// Can handle zombie death from different sources
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void ZombieDie(EDamageType DamageType);

	//~ Begin IDamageInterface
	virtual void ReceiveDamage(int32 IncomingDamage, EDamageType DamageType);
	virtual int32 GetHealthRemaining();
	//~ End IDamageInterface

	//~
	//~ New Zombie Input
	//~

	/** Add rotation equal to the yaw (in degrees) provided. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "AddInput"))
	virtual void AddRotationInput(float DeltaYawDegrees);

	/** Get rotation input. Returns pre-clear value. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	virtual float GetRotationInput();

	/** Get (and clear) rotation input. Returns pre-clear value. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	virtual float ConsumeRotationInput();

	/** Press the attack button. The pawn will know what to do with this. No arguments because it's a bool and this sets it to true. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "AddInput"))
	virtual void AddAttackInput();

	/** Get the status of the attack button. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	virtual bool GetAttackInput();

	/** Get (and clear) the status of the attack button. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	virtual bool ConsumeAttackInput();

private:
	/* The actor we're targeting. NULL if no target */
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	AActor* TargetActor;

	/* The actor we're targeting, cast to a Tank and cached. NULL if no target or target is not a tank*/
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	ATank* TargetTank;

protected:
	/** Movement profile used when this zombie moves. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie")
	FName MovementProfileName;

	//Collision body for the zombie
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* ZombieBody;

	/** Current health value. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	int32 Health;

	/** Sight distance (when no target is present) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float SightDistance;

	/** Sight half-angle in degrees (when no target is present) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float SightAngle;

	/** Max yaw speed per second in degrees. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float YawSpeed;

	/** Speed when wandering around, or when facing away from target. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float WalkSpeed;

	/** Speed when rushing toward target. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float RunSpeed;

	/** Attack distance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float AttackDistance;

	/** Half-angle (in degrees) for our attack. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float AttackAngle;

	/** Minimum time between attacks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float AttackCooldown;

	/** Minimum time before receiving damage again. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float DamageCooldown;

	/** If this value is greater than the current game time, zombie can't take damage again yet. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (AllowPrivateAccess = "true"))
	float DamageReadyTime;

	/** Game time, in seconds, when the Zombie will be allowed to attack again. */
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite, Category = "Zombie")
	float AttackAvailableTime;

private:
	// Current rotation input.
	float YawInput;

	// Current attack button status.
	uint32 bAttackInput : 1;
};
