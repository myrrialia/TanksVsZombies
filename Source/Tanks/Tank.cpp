// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"
#include "PaperSpriteComponent.h"
#include "Components/InputComponent.h"

void FTankInput::Sanitize()
{
	MovementInput = RawMovementInput.ClampAxes(-1.0f, 1.0f);
	MovementInput = MovementInput.GetSafeNormal();
	RawMovementInput.Set(0.0f, 0.0f);
}

void FTankInput::MoveX(float AxisValue)
{
	RawMovementInput.X += AxisValue;
}

void FTankInput::MoveY(float AxisValue)
{
	RawMovementInput.Y += AxisValue;
}

void FTankInput::Fire1(bool bPressed)
{
	bFire1 = bPressed;
}

void FTankInput::Fire2(bool bPressed)
{
	bFire2 = bPressed;
}

// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//If there's no root component, create and attach one
	if (!RootComponent)				
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TankBase"));
	}

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//Create TankDirection arrow component and attach it to the root component
	TankDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TankDirection"));
	TankDirection->SetupAttachment(RootComponent);

	//Create TankBody collision box and attach it to the TankDirection arrow component
	TankBody = CreateDefaultSubobject<UBoxComponent>(TEXT("TankBody"));
	TankBody->SetupAttachment(TankDirection);
	TankBody->SetBoxExtent(FVector(20.0f, 20.0f, 100.0f));
	TankBody->SetCollisionProfileName(TEXT("Tank:Move"));	

	//Create TankSprite and attach it to the TankDirection arrow component
	TankSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TankSprite"));
	TankSprite->SetupAttachment(TankDirection);

	//Create ChildTurret and attach it to TankDirection arrow
	ChildTurret = CreateDefaultSubobject<UChildActorComponent>(TEXT("Turret"));
	ChildTurret->SetupAttachment(TankDirection);

	//Create and setup SpringArm and attach it to the root component
	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->CameraLagSpeed = 2.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));

	//Create Camera and attach it to SpringArm
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.0f;
	CameraComponent->AspectRatio = 3.0f / 4.0f;
	CameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CameraComponent->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));

	MoveSpeed = 100.0f;
	MoveAccel = 200.0f;
	YawSpeed = 180.0f;

	Health = 50;

	CrushCollisionProfile = TEXT("Tank:Crush");
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();

	UTankStatics::PutInZPlane(this);
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TankInput.Sanitize();
	//UE_LOG(LogTemp, Warning, TEXT("Movement: (%f %f)"), TankInput.MovementInput.X, TankInput.MovementInput.Y);

	// Respond to controls if we're not dead.
	if (GetHealthRemaining() >= 0)
	{
		FVector DesiredMovementDirection = FVector(TankInput.MovementInput.X, TankInput.MovementInput.Y, 0.0f);
		if (!DesiredMovementDirection.IsNearlyZero())
		{
			//Rotate the tank - Rotating the TankDirection component, not the RootComponent
			FRotator MovementAngle = DesiredMovementDirection.Rotation();
			float DeltaYaw = UTankStatics::FindDeltaAngleDegrees(TankDirection->GetComponentRotation().Yaw, MovementAngle.Yaw);
			bool bReverse = false;
			if (DeltaYaw != 0.0f)
			{
				float AdjustedDeltaYaw = DeltaYaw;

				if ((AdjustedDeltaYaw < -90.0f))
				{
					AdjustedDeltaYaw += 180.0f;
					bReverse = true;
				}
				else if (AdjustedDeltaYaw > 90.0f)
				{
					AdjustedDeltaYaw -= 180.0f;
					bReverse = true;
				}

				//Turn toward the desired angle. Stop if we can get there in one frame.
				float MaxYawThisFrame = YawSpeed * DeltaTime;
				if (MaxYawThisFrame >= FMath::Abs(AdjustedDeltaYaw))
				{
					if (bReverse)
					{
						//Move backward
						FRotator FacingAngle = MovementAngle;
						FacingAngle.Yaw = MovementAngle.Yaw + 180.0f;
						TankDirection->SetWorldRotation(FacingAngle);
					}
					else
					{
						TankDirection->SetWorldRotation(MovementAngle);
					}
				}
				else
				{
					//Can't reach our desired angle this frame, rotate part way
					TankDirection->AddLocalRotation(FRotator(0.0f, FMath::Sign(AdjustedDeltaYaw) * MaxYawThisFrame, 0.0f));
				}

			}
			//Move the Tank
			{
				FVector MovementDirection = TankDirection->GetForwardVector() * (bReverse ? -1.0f : 1.0f);
				FVector StartPos = GetActorLocation();
				FVector Pos = StartPos;
				Pos.X += MovementDirection.X * MoveSpeed * DeltaTime;
				Pos.Y += MovementDirection.Y * MoveSpeed * DeltaTime;

				if (UWorld* World = GetWorld())
				{
					TArray<FHitResult> HitResults;
					FVector BoxSize = TankBody->GetScaledBoxExtent();
					FCollisionShape CollisionShape;
					CollisionShape.SetBox(BoxSize);
					World->SweepMultiByProfile(HitResults, StartPos, Pos, TankBody->GetComponentRotation().Quaternion(), CrushCollisionProfile, CollisionShape);
					for (const FHitResult& HitResult : HitResults)
					{
						if (IDamageInterface* DamageTarget = Cast<IDamageInterface>(HitResult.Actor.Get()))
						{
							//Zombie is crushed by tank, regardless of remaining health
							int32 TargetHealth = DamageTarget->GetHealthRemaining();
							if (TargetHealth >= 0)
							{
								DamageTarget->ReceiveDamage(TargetHealth, EDamageType::Crushed);
							}
						}
						else
						{
							return;
						}
					}
				}
				SetActorLocation(Pos);
			}
		}

	}
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveX", this, &ATank::MoveX);
	InputComponent->BindAxis("MoveY", this, &ATank::MoveY);
	InputComponent->BindAction("Fire1", EInputEvent::IE_Pressed, this, &ATank::Fire1Pressed);
	InputComponent->BindAction("Fire1", EInputEvent::IE_Released, this, &ATank::Fire1Released);
	InputComponent->BindAction("Fire2", EInputEvent::IE_Pressed, this, &ATank::Fire2Pressed);
	InputComponent->BindAction("Fire2", EInputEvent::IE_Released, this, &ATank::Fire2Released);
}

void ATank::ReceiveDamage(int32 IncomingDamage, EDamageType DamageType)
{
	if (IncomingDamage >= Health)
	{
		if (Health >= 0)
		{
			Health = -1;
			TankDie(DamageType);
		}
		return;
	}
	Health -= IncomingDamage;

	if (Spill)
	{
		if (UWorld* World = GetWorld())
		{
				if (AActor* SpawnedActor = World->SpawnActor(Spill->GetDefaultObject()->GetClass()))
				{
					SpawnedActor->SetActorLocation((UGameplayStatics::GetPlayerPawn(this, 0))->GetActorLocation());
					UTankStatics::PutInZPlane(SpawnedActor);
				}
		}
	}
}

void ATank::TankDie_Implementation(EDamageType DamageType)
{
	UGameplayStatics::OpenLevel(GetWorld(), "WaitingMap");
}

void ATank::MoveX(float AxisValue)
{
	TankInput.MoveX(AxisValue);
}

void ATank::MoveY(float AxisValue)
{
	TankInput.MoveY(AxisValue);
}

void ATank::Fire1Pressed()
{
	TankInput.Fire1(true);
}

void ATank::Fire1Released()
{
	TankInput.Fire1(false);
}

void ATank::Fire2Pressed()
{
	TankInput.Fire2(true);
}

void ATank::Fire2Released()
{
	TankInput.Fire2(false);
}