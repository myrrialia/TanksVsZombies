// Copyright 2019 Nicky Argiropoulos

#include "Mushroom.h"

// Sets default values
AMushroom::AMushroom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create ZombieBody collision box and attach it to the root component
	MushroomBody = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MushroomBody"));
	MushroomBody->SetupAttachment(RootComponent);
	MushroomBody->SetCapsuleRadius(12.0f);
	MushroomBody->SetCapsuleHalfHeight(80.0f);

	Health = 1;
}

// Called when the game starts or when spawned
void AMushroom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMushroom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AMushroom::ReceiveDamage(int32 IncomingDamage, EDamageType DamageType)
{
	if (DamageType == EDamageType::HitWithMissile || DamageType == EDamageType::Crushed)
	{
		if (Health > 0)
		{
			Health = 0;
			MushroomDie(DamageType);
		}
	} 
}

int32 AMushroom::GetHealthRemaining()
{
	return Health;
}
