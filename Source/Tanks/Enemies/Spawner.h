// Copyright 2019 Nicky Argiropoulos

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Spawner.generated.h"

UCLASS()
class TANKS_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Helpful debug tool - which way is the spawner facing?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* SpawnerDirection;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorToSpawn;

	/** If anything matching this collision profile is present in the spawning volume, spawning will not be permitted. */
	UPROPERTY(EditAnywhere)
	FName SpawnCollisionProfile;

	UPROPERTY(EditAnywhere)
	UBoxComponent* SpawnVolume;
};