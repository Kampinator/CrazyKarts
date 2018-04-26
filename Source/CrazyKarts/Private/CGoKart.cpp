// Fill out your copyright notice in the Description page of Project Settings.

#include "public/CGoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"


// Sets default values
ACGoKart::ACGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceleration = Force / Mass;



	// Add Acceleration to Velocity.
	Velocity = Velocity + (Acceleration * DeltaTime);

	// Rotates car.
	ApplyRotation(DeltaTime);

	// To meters / s
	UpdateLocationFromVelocity(DeltaTime);

}

FVector ACGoKart::GetAirResistance()
{
	// Calculate AirResistance.  AirResistance = -Speed^2 * DragCoefficient
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector ACGoKart::GetRollingResistance()
{
	// Get Gravity. Divide by 100 to change it to 9.81. We negative it to push upwards.	
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	// Normal Force is basically opposite of gravity.
	float NormalForce = Mass * AccelerationDueToGravity;
	// Coefficient has specific values. Can check them from wikipedia.
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}



void ACGoKart::ApplyRotation(float DeltaTime)
{
	float RotationAngle = MaxDegreesPerSecond * DeltaTime * SteeringThrow;
	FQuat RotationDelta(GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));

	// Rotate vector same amount as rotated the car. (basically what moves car forward)
	Velocity = RotationDelta.RotateVector(Velocity);
	AddActorWorldRotation(RotationDelta);
}

void ACGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;
	FHitResult TranslationHitResult;
	AddActorWorldOffset(Translation, true, &TranslationHitResult);
	if (TranslationHitResult.IsValidBlockingHit())
	{
		Velocity = FVector(0, 0, 0);
	}
}

// Called to bind functionality to input
void ACGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACGoKart::MoveRight);
}

void ACGoKart::MoveForward(float Value)
{
	Throttle = Value;
}

void ACGoKart::MoveRight(float Value)
{
	SteeringThrow = Value;
}

