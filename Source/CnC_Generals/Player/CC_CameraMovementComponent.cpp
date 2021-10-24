// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_CameraMovementComponent.h"

#include "DrawDebugHelpers.h"

UCC_CameraMovementComponent::UCC_CameraMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MoveAccel = 30.f;
	MoveSpeedMax = 1000.f;

	BreakingTime = 1.f;  // 1 sec
}

void UCC_CameraMovementComponent::AddMovementInput(const FVector& MovementDirection)
{
	if (!MovementDirection.IsZero())
	{
		SetEnableBreaking(false);
		ComputeVelocity(MovementDirection * MoveAccel);
	}
}

void UCC_CameraMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsBreaking)
	{
		ComputeBreakingVelocity(DeltaTime);
	}

	if (!Velocity.IsZero())
	{
		ComputeMovementDelta(DeltaTime);
	}


#if WITH_EDITOR
	// debug
	if (GetWorld() && GetOwner())
	{
		FString Msg = "Velocity: " + Velocity.ToString() + (bIsBreaking ? " Breaking = true" : "Breaking = false");
		DrawDebugString(GetWorld(), GetOwner()->GetActorLocation(), Msg, 0, FColor::Cyan, 0.f, true);
	}

#endif
}

void UCC_CameraMovementComponent::ComputeVelocity(const FVector& MovementAcceleration)
{
	// v = v0 + a*t
	GEngine->AddOnScreenDebugMessage(-1, 1., FColor::Red, MovementAcceleration.ToString());
	auto World = GetWorld();
	float DeltaSeconds = World ? World->GetDeltaSeconds() : 0.f;
	FVector NewVelocity = Velocity + MovementAcceleration * DeltaSeconds;
	Velocity = MoveSpeedMax > 0.f ? NewVelocity.GetClampedToMaxSize(MoveSpeedMax) : NewVelocity;
}

void UCC_CameraMovementComponent::ComputeBreakingVelocity(float DeltaTime)
{
	if (BreakingTime <= 0.f)
	{
		Velocity = FVector::ZeroVector;
	}
	else
	{
		Velocity = FMath::VInterpConstantTo(Velocity, FVector::ZeroVector, DeltaTime, BreakingSpeed);
	}
}

void UCC_CameraMovementComponent::ComputeMovementDelta(float DeltaTime)
{
	// p = po + v*t

	if (AActor* Owner = GetOwner())
	{
		const FVector OldLocation = Owner->GetActorLocation();
		Owner->SetActorLocation(OldLocation + Velocity);
	}
}

void UCC_CameraMovementComponent::SetEnableBreaking(bool bBreak)
{
	if (bBreak)
	{
		bIsBreaking = true;
		BreakingSpeed = Velocity.Size() / BreakingTime;
	}
	else
	{
		bIsBreaking = false;
	}
}

