// Copyright 2019 Nicky Argiropoulos

#include "Spawner.h"
#include "Kismet/KismetMathLibrary.h"
#include "Containers/Array.h"
#include "TanksGameMode.h"
#include "TankStatics.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 10.0f;

	RootComponent = SpawnerDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnerDirection"));
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	FVector BoxExtent = SpawnVolume->GetUnscaledBoxExtent();
	BoxExtent.Z = 100.0f;
	SpawnVolume->SetBoxExtent(BoxExtent, false);
	SpawnVolume->SetupAttachment(RootComponent);
	
	SpawnCollisionProfile = TEXT("Spawner:Block");
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	
	UTankStatics::PutInZPlane(this);
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActorToSpawn)
	{
		if (UWorld* World = GetWorld())
		{

			//Check how many zombies are in the world already
			ATanksGameMode* GameMode = (ATanksGameMode*)GetWorld()->GetAuthGameMode();
			float NumberOfZombies = GameMode->NumOfZombies;

			if (NumberOfZombies < 16.0f)
			{
				FCollisionShape CollisionShape;
				CollisionShape.SetBox(SpawnVolume->GetUnscaledBoxExtent());
				if (!World->OverlapAnyTestByProfile(SpawnVolume->GetComponentLocation(), SpawnVolume->GetComponentRotation().Quaternion(), SpawnCollisionProfile, CollisionShape))
				{
					if (AActor* SpawnedActor = World->SpawnActor(ActorToSpawn->GetDefaultObject()->GetClass()))
					{
						SpawnedActor->SetActorLocation(UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->GetComponentLocation(), SpawnVolume->GetUnscaledBoxExtent()));
						UTankStatics::PutInZPlane(SpawnedActor);

						SpawnedActor->SetActorRotation(SpawnerDirection->GetComponentRotation());
					}
				}
			}
		}
	}
}