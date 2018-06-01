// Fill out your copyright notice in the Description page of Project Settings.

#include "public/CGoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"



void ACGoKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACGoKart, ServerState);
}

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
	bReplicates = true;
	if (HasAuthority())
	{

	}
	
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "Error";
	}
}


// Called every frame
void ACGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_AutonomousProxy)
	{
		FGoKartMove Move = CreateMove(DeltaTime);
		SimulateMove(Move);
		UnAcknowledgedMoves.Add(Move);
		Server_SendMove(Move);
	}

	// We are server and in control of the pawn
	if (Role == ROLE_Authority && GetRemoteRole() == ROLE_SimulatedProxy)
	{
		FGoKartMove Move = CreateMove(DeltaTime);
		Server_SendMove(Move);
	}



	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this, FColor::Blue, DeltaTime);




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





void ACGoKart::ApplyRotation(float DeltaTime, float SteeringThrow)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MiniumTurningRadius * SteeringThrow;
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);

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



// Implementation is called on the server.
void ACGoKart::Server_SendMove_Implementation(FGoKartMove Move)
{
	SimulateMove(Move);
	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = Velocity;
	/// TODO: Update last move
}

bool ACGoKart::Server_SendMove_Validate(FGoKartMove Move)
{
	/// TODO: Make better validation
	return true;
}
 
void ACGoKart::OnRep_ServerState()
{
	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;
	ClearAcknowledgedMoves(ServerState.LastMove);

	for (const FGoKartMove& Move : UnAcknowledgedMoves)
	{
		SimulateMove(Move);
	}
}


void ACGoKart::SimulateMove(FGoKartMove Move)
{
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Move.Throttle;
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

FGoKartMove ACGoKart::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.Throttle = Throttle;
	Move.Time = GetWorld()->TimeSeconds;
	return Move;
}

void ACGoKart::ClearAcknowledgedMoves(FGoKartMove LastMove)
{
	TArray<FGoKartMove> NewMoves;
	for (const FGoKartMove& Move : UnAcknowledgedMoves)
	{
		if (Move.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}

	UnAcknowledgedMoves = NewMoves;	
}

void ACGoKart::MoveForward(float Value)
{
	Throttle = Value;

}

void ACGoKart::MoveRight(float Value)
{
	SteeringThrow = Value;
}


