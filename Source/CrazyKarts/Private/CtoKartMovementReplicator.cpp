// Fill out your copyright notice in the Description page of Project Settings.

#include "CtoKartMovementReplicator.h"
#include "Net/UnrealNetwork.h"
#include "CGoKart.h"


// Sets default values for this component's properties
UCtoKartMovementReplicator::UCtoKartMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	// ...
}

void UCtoKartMovementReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCtoKartMovementReplicator, ServerState);
}

// Called when the game starts
void UCtoKartMovementReplicator::BeginPlay()
{
	Super::BeginPlay();
	MovementComponent = GetOwner()->FindComponentByClass<UCGoKartMovementComponent>();

	// ...
}


// Called every frame
void UCtoKartMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MovementComponent == nullptr) return;

	FGoKartMove LastMove = MovementComponent->GetLastMove();

	// We are client controlling our own pawn.
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		UnAcknowledgedMoves.Add(LastMove);
		Server_SendMove(LastMove);
	}

	//
	// We are server and in control of the pawn
	if (GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		UpdateServerState(LastMove);		
	}

	// Vehicle when playing that are seen when playing as a client.
	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		MovementComponent->SimulateMove(ServerState.LastMove);
	}
}

void UCtoKartMovementReplicator::UpdateServerState(const FGoKartMove& Move)
{
	ServerState.LastMove = Move;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();
}

// Implementation is called on the server.
void UCtoKartMovementReplicator::Server_SendMove_Implementation(FGoKartMove Move)
{
	if (MovementComponent == nullptr) return;
	MovementComponent->SimulateMove(Move);
	UpdateServerState(Move);
}

bool UCtoKartMovementReplicator::Server_SendMove_Validate(FGoKartMove Move)
{
	/// TODO: Make better validation
	return true;
}

void UCtoKartMovementReplicator::OnRep_ServerState()
{
	if (MovementComponent == nullptr) return;
	GetOwner()->SetActorTransform(ServerState.Transform);
	MovementComponent->SetVelocity(ServerState.Velocity);
	ClearAcknowledgedMoves(ServerState.LastMove);

	for (const FGoKartMove& Move : UnAcknowledgedMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}

void UCtoKartMovementReplicator::ClearAcknowledgedMoves(FGoKartMove LastMove)
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


