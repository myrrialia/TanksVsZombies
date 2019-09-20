// Fill out your copyright notice in the Description page of Project Settings.

#include "Missile.h"
#include "DamageInterface.h"
#include "WorldCollision.h"
#include "Engine/World.h"

// Sets default values
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Speed = 200.0f;
	Radius = 20.0f;
	DirectDamage = 5;
}

// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &AMissile::Explode, 1.0f);
}

// Called every frame
void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Loc = GetActorLocation();
	FVector DesiredEndLoc = Loc + (DeltaTime * Speed) * GetTransform().GetUnitAxis(EAxis::X);
	FHitResult HitResult;
	FCollisionShape CollisionShape;
	if (Radius > 0.0f)
	{
		if (UWorld* World = GetWorld())
		{
			CollisionShape.SetCapsule(Radius, 200.0f);
			if (World->SweepSingleByProfile(HitResult, Loc, DesiredEndLoc, FQuat::Identity, MovementCollisionProfile, CollisionShape))
			{
				SetActorLocation(HitResult.Location);
				if (IDamageInterface* DamageActor = Cast<IDamageInterface>(HitResult.Actor.Get()))
				{
					DamageActor->ReceiveDamage(DirectDamage, EDamageType::HitWithMissile);
				}
				Explode();
			}
			else
			{
				SetActorLocation(DesiredEndLoc);
			}
		}
	}
}

void AMissile::Explode()
{
	GetWorldTimerManager().ClearTimer(ExplodeTimerHandle);
	SetActorEnableCollision(false);
	OnExplode();
}

void AMissile::OnExplode_Implementation()
{
	Destroy();
}

