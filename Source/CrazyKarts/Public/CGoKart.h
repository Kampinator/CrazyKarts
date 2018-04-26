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
	FVector GetAirResistance();
	FVector GetRollingResistance();

	// Max driving force (newtowns)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	// Minium radius of the car turning circle at full lock (meters)
	UPROPERTY(EditAnywhere)
	float MiniumTurningRadius = 10;

	// Higher means more drag. kg/minutes
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	// Higher means more rolling resistance
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.115;

	// The mass of the car (kg).
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	// Server RPC function
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);
	void MoveForward(float Value);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);
	void MoveRight(float Value);
	
	FVector Velocity;
	UPROPERTY(Replicated)
	FVector ReplicatedLocation;

	UPROPERTY(Replicated)
	FRotator ReplicatedRotation;

	// PEDAL
	float Throttle;
	float SteeringThrow;

public:





	
	
};
