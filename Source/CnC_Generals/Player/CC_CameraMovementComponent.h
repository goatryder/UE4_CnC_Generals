// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CC_CameraMovementComponent.generated.h"

// Deprecated
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CNC_GENERALS_API UCC_CameraMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCC_CameraMovementComponent();
	
	void AddMovementInput(const FVector& MovementDirection);
	void SetEnableBreaking(bool bBreak);

protected:
	UPROPERTY(Category = "CC_CameraMovementComp", BlueprintReadWrite)
	float MoveSpeedMax;

	UPROPERTY(Category = "CC_CameraMovementComp", BlueprintReadWrite)
	float MoveAccel;

	// on stop movement how much time it takes to stop, if zero - instant stop
	UPROPERTY(Category = "CC_CameraMovementComp", BlueprintReadWrite)
	float BreakingTime;

	bool bIsBreaking;
	float BreakingSpeed;

	FVector Velocity;

private:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// add velocity on input
	void ComputeVelocity(const FVector& MovementDirection);
	// velocity to zero when no input
	void ComputeBreakingVelocity(float DeltaTime);
	
	void ComputeMovementDelta(float DeltaTime);
};
