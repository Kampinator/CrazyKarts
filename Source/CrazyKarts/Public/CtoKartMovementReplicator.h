// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CGoKartMovementComponent.h"
#include "CtoKartMovementReplicator.generated.h"

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRAZYKARTS_API UCtoKartMovementReplicator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCtoKartMovementReplicator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Server RPC function
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();



private:
	UPROPERTY()
	UCGoKartMovementComponent* MovementComponent;
	
	TArray<FGoKartMove> UnAcknowledgedMoves;
	void ClearAcknowledgedMoves(FGoKartMove Move);
};
