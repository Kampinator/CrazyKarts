// Fill out your copyright notice in the Description page of Project Settings.

#include "public/CGoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"



// Sets default values
ACGoKart::ACGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MovementComp = CreateDefaultSubobject<UCGoKartMovementComponent>(TEXT("MovementComponent"));
	ReplicatorComp = CreateDefaultSubobject<UCtoKartMovementReplicator>(TEXT("ReplicatorComponent"));
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
	
 
	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this, FColor::Blue, DeltaTime);
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
	if (MovementComp == nullptr) return;
	MovementComp->SetThrottle(Value);
}

void ACGoKart::MoveRight(float Value)
{
	if (MovementComp == nullptr) return;
	MovementComp->SetSteeringThrow(Value);
}


