// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CGoKart.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	float Throttle;
	UPROPERTY()
	float SteeringThrow;
	UPROPERTY()
	float DeltaTime;
	UPROPERTY()
	float TimeStamp;

};

USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FGoKartMove LastMove;
	UPROPERTY()
	FVector Velocity;
	UPROPERTY()
	FTransform Transform;
};


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
	void SimulateMove(FGoKartMove Move);
	void ApplyRotation(float DeltaTime, float SteeringThrow);
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
	void Server_SendMove(FGoKartMove Move);

	void MoveForward(float Value);
	void MoveRight(float Value);




	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	//FGoKartMove Server


	FVector Velocity;

	UFUNCTION()
	void OnRep_ServerState();


	// PEDAL
	float Throttle;
	float SteeringThrow;

public:





	
	
};
