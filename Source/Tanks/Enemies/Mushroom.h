// Copyright 2019 Nicky Argiropoulos

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageInterface.h"
#include "Components/CapsuleComponent.h"
#include "Mushroom.generated.h"

UCLASS()
class TANKS_API AMushroom : public AActor, public IDamageInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMushroom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Can handle zombie death from different sources
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void MushroomDie(EDamageType DamageType);

	//~ Begin IDamageInterface
	virtual void ReceiveDamage(int32 IncomingDamage, EDamageType DamageType);
	virtual int32 GetHealthRemaining();
	//~ End IDamageInterface

	//Collision body for the mushroom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mushroom", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* MushroomBody;

	UPROPERTY(EditAnywhere)
	int32 Health;
};
