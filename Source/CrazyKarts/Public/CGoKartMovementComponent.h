// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CGoKartMovementComponent.generated.h"

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
	float Time;
	UPROPERTY()
	float TimeStamp;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRAZYKARTS_API UCGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCGoKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	void SimulateMove(FGoKartMove Move);
	FGoKartMove CreateMove(float DeltaTime);

	void SetVelocity(FVector NewVelocity);
	FVector GetVelocity();

	void SetThrottle(float Value);
	void SetSteeringThrow(float Value);

	FGoKartMove GetLastMove() { return LastMove;  }

private:
	FVector Velocity;

	// PEDAL
	float Throttle;
	float SteeringThrow;

	FVector GetAirResistance();
	FVector GetRollingResistance();

	void ApplyRotation(float DeltaTime, float SteeringThrow);
	void UpdateLocationFromVelocity(float DeltaTime);

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
	
	FGoKartMove LastMove;
};
