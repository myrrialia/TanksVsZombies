// Fill out your copyright notice in the Description page of Project Settings.

#include "Turret.h"
#include "PaperSpriteComponent.h"
#include "Tank.h"

const FName ATurret::MuzzleSocketName(TEXT("Muzzle"));

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = TurretDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TurretDirection"));

	TurretSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TurretSprite"));
	TurretSprite->SetupAttachment(TurretDirection);

	YawSpeed = 180.0f;
	Fire1Cooldown = 1.0f;
	Fire2Cooldown = 5.0f;
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	Tank = Cast<ATank>(GetParentComponent()->GetOwner());
	check(Tank);
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Aim
	check(TurretDirection);
	if (Tank != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(Tank->GetController()))
		{
			FVector2D AimLocation;
			if (PC->GetMousePosition(AimLocation.X, AimLocation.Y))
			{
				FVector2D TurretLocation = FVector2D::ZeroVector;
				UGameplayStatics::ProjectWorldToScreen(PC, TurretDirection->GetComponentLocation(), TurretLocation);
				float DesiredYaw;

				if (UTankStatics::FindLookAtAngle2D(TurretLocation, AimLocation, DesiredYaw))
				{
					//UE_LOG(LogTemp, Warning, TEXT("Mouse Location: (%f %f)"), AimLocation.X, AimLocation.Y);
					//UE_LOG(LogTemp, Warning, TEXT("Desired Rotation: (%f)"), DesiredYaw);
					FRotator CurrentRotation = TurretDirection->GetComponentRotation();
					float DeltaYaw = UTankStatics::FindDeltaAngleDegrees(CurrentRotation.Yaw, DesiredYaw);
					float MaxDeltaYawThisFrame = YawSpeed * DeltaTime;

					//Perform current frame's rotation
					if (MaxDeltaYawThisFrame > FMath::Abs(DeltaYaw))
					{
						//We can reach the desired rotation this frame, so go ahead
						CurrentRotation.Yaw = DesiredYaw;
					}
					else
					{
						//We can't reach the desired roation this frame, so turn as far as we can
						CurrentRotation.Yaw += (FMath::Sign(DeltaYaw) * MaxDeltaYawThisFrame);
					}

					TurretDirection->SetWorldRotation(CurrentRotation);
				}
			}
		}

		//Fire 1
		const FTankInput& CurrentInput = Tank->GetCurrentInput();
		if (CurrentInput.bFire1 && Projectiles1.Num())
		{
			if (UWorld* World = GetWorld())
			{
				float CurrentTime = World->GetTimeSeconds();
				if (Fire1ReadyTime <= CurrentTime)
				{
					UE_LOG(LogTemp, Warning, TEXT("Fire1!"));
					FVector Loc = TurretSprite->GetSocketLocation(MuzzleSocketName);
					FRotator Rot = TurretDirection->GetComponentRotation();

					for (TSubclassOf<AActor> Projectile : Projectiles1)
					{
						if (AActor* NewProjectile = World->SpawnActor(Projectile))
						{
							NewProjectile->SetActorLocation(Loc);
							NewProjectile->SetActorRotation(Rot);
						}
					}
					//Set the cooldown timer
					Fire1ReadyTime = CurrentTime + Fire1Cooldown;
				}
			}
		}

		//Fire 2
		if (CurrentInput.bFire2 && Projectiles2.Num())
		{
			if (UWorld* World = GetWorld())
			{
				float CurrentTime = World->GetTimeSeconds();
				if (Fire2ReadyTime <= CurrentTime && Fire1ReadyTime <= CurrentTime)
				{
					UE_LOG(LogTemp, Warning, TEXT("Fire2!"));
					FVector Loc = TurretSprite->GetSocketLocation(MuzzleSocketName);
					FRotator Rot = TurretDirection->GetComponentRotation();

					for (TSubclassOf<AActor> Projectile : Projectiles2)
					{
						if (AActor* NewProjectile = World->SpawnActor(Projectile))
						{
							NewProjectile->SetActorLocation(Loc);
							NewProjectile->SetActorRotation(Rot);
						}
					}
					//Set the cooldown timer
					Fire2ReadyTime = CurrentTime + Fire2Cooldown;
				}
			}
		}
	}
}

