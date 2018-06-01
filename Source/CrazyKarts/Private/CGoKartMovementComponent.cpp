// Fill out your copyright notice in the Description page of Project Settings.

#include "CGoKartMovementComponent.h"
#include "CGoKart.h"


// Sets default values for this component's properties
UCGoKartMovementComponent::UCGoKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCGoKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCGoKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetOwnerRole() == ROLE_AutonomousProxy || GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		LastMove = CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}

}


FVector UCGoKartMovementComponent::GetRollingResistance()
{
	UWorld* ThisWorld = GetOwner()->GetWorld();
	// Get Gravity. Divide by 100 to change it to 9.81. We negative it to push upwards.	
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	// Normal Force is basically opposite of gravity.
	float NormalForce = Mass * AccelerationDueToGravity;
	// Coefficient has specific values. Can check them from wikipedia.
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

void UCGoKartMovementComponent::ApplyRotation(float DeltaTime, float SteeringThrow)
{
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MiniumTurningRadius * SteeringThrow;
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);

	// Rotate vector same amount as rotated the car. (basically what moves car forward)
	Velocity = RotationDelta.RotateVector(Velocity);
	GetOwner()->AddActorWorldRotation(RotationDelta);
}

void UCGoKartMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;
	FHitResult TranslationHitResult;
	GetOwner()->AddActorWorldOffset(Translation, true, &TranslationHitResult);
	if (TranslationHitResult.IsValidBlockingHit())
	{
		Velocity = FVector(0, 0, 0);
	}
}

FVector UCGoKartMovementComponent::GetAirResistance()
{
	// Calculate AirResistance.  AirResistance = -Speed^2 * DragCoefficient
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

void UCGoKartMovementComponent::SimulateMove(FGoKartMove Move)
{
	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceleration = Force / Mass;

	// Add Acceleration to Velocity.
	Velocity = Velocity + (Acceleration * Move.DeltaTime);

	// Rotates car.
	ApplyRotation(Move.DeltaTime, Move.SteeringThrow);

	// To meters / s
	UpdateLocationFromVelocity(Move.DeltaTime);
}

FGoKartMove UCGoKartMovementComponent::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.Throttle = Throttle;
	Move.Time = GetWorld()->TimeSeconds;
	return Move;
}

void UCGoKartMovementComponent::SetThrottle(float Value)
{
	Throttle = Value;
}

void UCGoKartMovementComponent::SetSteeringThrow(float Value)
{
	SteeringThrow = Value;
}

FVector UCGoKartMovementComponent::GetVelocity()
{
	return Velocity;
}

void UCGoKartMovementComponent::SetVelocity(FVector NewVelocity)
{
	Velocity = NewVelocity;
}