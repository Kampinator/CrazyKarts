// Fill out your copyright notice in the Description page of Project Settings.

#include "public/CGoKart.h"
#include "Components/InputComponent.h"


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
	FVector Acceleration = Force / Mass;

	// Add Acceleration to Velocity.
	Velocity = Velocity + (Acceleration * DeltaTime);

	// To meters / s
	UpdateLocationFromVelocity(DeltaTime);

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
}

void ACGoKart::MoveForward(float Value)
{
	Throttle = Value;

}

