// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieController.h"
#include "Zombie.h"

void AZombieController::Tick(float DeltaTime)
{
	if (PawnAsZombie)
	{
		if (AActor* Target = PawnAsZombie->GetTarget())
		{
			//We have a target. Approach and attempt violence!
			FVector DirectionToTarget = (Target->GetActorLocation() - PawnAsZombie->GetActorLocation()).GetSafeNormal2D();
			float DotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsZombie->GetActorForwardVector()); //How far target dir is from forward vector
			float SidewaysDotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsZombie->GetActorRightVector());	//How far target dir is from right vector
			float DeltaYawDesired = FMath::Atan2(SidewaysDotToTarget, DotToTarget);

			if (PawnAsZombie->ZombieAIShouldAttack())
			{
				PawnAsZombie->AddAttackInput();
			}
			else
			{
				// Move faster when facing toward the target so that we turn more accurately/don't orbit.
				PawnAsZombie->AddMovementInput(FVector(1.0f, 0.0f, 0.0f), FMath::GetMappedRangeValueClamped(FVector2D(-0.707f, 0.707f), FVector2D(0.0f, 1.0f), DotToTarget));
				// Attempt the entire turn in one frame. Expresses our desired turn amount, Zombie will cap this itself
				PawnAsZombie->AddRotationInput(DeltaYawDesired);
			}
		}
	}
}

void AZombieController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	PawnAsZombie = Cast<AZombie>(GetPawn());
}

void AZombieController::UnPossess()
{
	Super::UnPossess();
	PawnAsZombie = nullptr;
}