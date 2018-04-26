// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CGoKart.generated.h"

UCLASS()
class CRAZYKARTS_API ACGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void ApplyRotation(float DeltaTime);
	void UpdateLocationFromVelocity(float DeltaTime);

	// Force applied to the car when the throttle is fully down(N). 10m/s / s
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	// Maxium degrees that can be turned in second
	UPROPERTY(EditAnywhere)
	float MaxDegreesPerSecond = 90;

	// The mass of the car (kg).
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	void MoveForward(float Value);
	void MoveRight(float Value);
	FVector Velocity;

	// PEDAL
	float Throttle;
	float SteeringThrow;

public:





	
	
};
