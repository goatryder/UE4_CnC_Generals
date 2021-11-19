// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_PlayerCameraPawn.h"

#include "CC_PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "DrawDebugHelpers.h"
#include "Widgets/SViewport.h"

// Sets default values
ACC_PlayerCameraPawn::ACC_PlayerCameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->TargetArmLength = 1200.f;
	SpringArmComp->AddLocalRotation(FRotator(310.f, 0.f, 0.f));
	SpringArmComp->SetupAttachment(RootComp);
	SpringArmComp->bDoCollisionTest = false;
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	CameraComp->SetupAttachment(SpringArmComp);

	// defaults
	CameraMoveSpeedMax = 16000.f;
	CameraMoveStopTime = 0.5f;

	CameraRotationSpeed = 9.f;
	CameraPitchMin = 280.f;
	CameraPitchMax = 340.f;
	CameraPitchInitial = 300.f;
	MouseClickSpeed = 0.15f;
	CameraRotationRestoreSpeed = 13.f;

	CameraZoomSpeed = 100.f;
	OnZoomSpringArmLenMin = 500.f;
	OnZoomSpringArmLenMax = 4000.f;

	// set default rotation
	if (SpringArmComp)
	{
		SpringArmComp->SetWorldRotation(FRotator(CameraPitchInitial, 0.f, 0.f));
	}
}

// Called to bind functionality to input
void ACC_PlayerCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("CameraUp", this, &ACC_PlayerCameraPawn::CameraUpInput);
	PlayerInputComponent->BindAxis("CameraRight", this, &ACC_PlayerCameraPawn::CameraRightInput);

	PlayerInputComponent->BindAction("RotateCamera", IE_Pressed, this, &ACC_PlayerCameraPawn::OnRotateCamera_Pressed);
	PlayerInputComponent->BindAction("RotateCamera", IE_Released, this, &ACC_PlayerCameraPawn::OnRotateCamera_Released);

	PlayerInputComponent->BindAction("MoveCamera", IE_Pressed, this, &ACC_PlayerCameraPawn::OnMoveCamera_Pressed);
	PlayerInputComponent->BindAction("MoveCamera", IE_Released, this, &ACC_PlayerCameraPawn::OnMoveCamera_Released);

	PlayerInputComponent->BindAction("CameraZoomIn", IE_Pressed, this, &ACC_PlayerCameraPawn::OnCameraZoomIn);
	PlayerInputComponent->BindAction("CameraZoomOut", IE_Pressed, this, &ACC_PlayerCameraPawn::OnCameraZoomOut);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &ACC_PlayerCameraPawn::OnSelect_Pressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &ACC_PlayerCameraPawn::OnSelect_Released);
}

void ACC_PlayerCameraPawn::PossessedBy(AController* NewController)
{
	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		PC->SetShowMouseCursor(true);
	}
}

void ACC_PlayerCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalcMoveVelocity_Tick(DeltaTime);

	RestoreCameraRotation_Tick(DeltaTime);
}

void ACC_PlayerCameraPawn::CameraUpInput(float Val)
{
	if (bCameraShouldRotate && ensure(SpringArmComp))
	{
		FRotator Rotation = SpringArmComp->GetComponentRotation().Clamp();
		float NewPitch = Rotation.Pitch + Val * CameraRotationSpeed;
		Rotation.Pitch = FMath::Clamp(NewPitch, CameraPitchMin, CameraPitchMax);
		SpringArmComp->SetWorldRotation(FRotator(Rotation));
	}
}

void ACC_PlayerCameraPawn::CameraRightInput(float Val)
{
	// todo: handle drop camera rotation if released and not moved
	if (bCameraShouldRotate && ensure(SpringArmComp))
	{
		SpringArmComp->AddWorldRotation(FRotator(0.f, Val * CameraRotationSpeed, 0.f));
	}
}

void ACC_PlayerCameraPawn::OnSelect_Pressed()
{
	if (MouseXYInputIsUsed())
	{
		return;
	}

	bSelectIsPressed = true;

	// check if should move camera on cursor double click
	const float SecondClickTime = GetWorld()->GetTimeSeconds() - SelectButtonReleasedTime;
	if (SecondClickTime < MouseClickSpeed)
	{
		if (const APlayerController* PC = GetController<APlayerController>())
		{
			FVector TraceStart;
			FVector TraceDirection;
			PC->DeprojectMousePositionToWorld(TraceStart, TraceDirection);
			const FVector TraceEnd = TraceStart + TraceDirection * 30000.f;
			FHitResult Hit;

			if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility))
			{
				CursorPointMoveTo = FVector(Hit.Location.X, Hit.Location.Y, GetActorLocation().Z);
			}
		}
	}
	
}

void ACC_PlayerCameraPawn::OnSelect_Released()
{
	bSelectIsPressed = false;

	// for moving camera on double click mouse
	SelectButtonReleasedTime = GetWorld()->GetTimeSeconds();
}

void ACC_PlayerCameraPawn::OnRotateCamera_Pressed()
{
	if (MouseXYInputIsUsed())
	{
		return;
	}

	bCameraShouldRotate = true;

	// for camera restore if button pressed and released quickly
	CamRotButtonPressedTime = GetWorld()->GetTimeSeconds();
}

void ACC_PlayerCameraPawn::OnRotateCamera_Released()
{
	bCameraShouldRotate = false;

	// check if should restore camera
	const float ButtonHoldTime = GetWorld()->GetTimeSeconds() - CamRotButtonPressedTime;
	if (ButtonHoldTime < MouseClickSpeed)
	{
		bRotationRestoreInProgress = true;
	}
}

void ACC_PlayerCameraPawn::OnMoveCamera_Pressed()
{
	if (MouseXYInputIsUsed())
	{
		return;
	}

	if (auto PC = Cast<APlayerController>(GetController()))
	{
		PC->GetMousePosition(CameraMoveCursorAnchor.X, CameraMoveCursorAnchor.Y);
	}

	bCameraShouldMove = true;

	CursorPointMoveTo = FVector::ZeroVector;  // interrupt move on double click
}

void ACC_PlayerCameraPawn::OnMoveCamera_Released()
{
	BreakingSpeed = Velocity.Size() / CameraMoveStopTime;
	bCameraShouldMove = false;
}

void ACC_PlayerCameraPawn::OnCameraZoomIn()
{
	if (ensure(SpringArmComp))
	{
		float NewSpringArmLen = SpringArmComp->TargetArmLength - CameraZoomSpeed;
		SpringArmComp->TargetArmLength = FMath::Max(NewSpringArmLen, OnZoomSpringArmLenMin);
	}
}

void ACC_PlayerCameraPawn::OnCameraZoomOut()
{
	if (ensure(SpringArmComp))
	{
		float NewSpringArmLen = SpringArmComp->TargetArmLength + CameraZoomSpeed;
		SpringArmComp->TargetArmLength = FMath::Min(NewSpringArmLen, OnZoomSpringArmLenMax);
	}
}

void ACC_PlayerCameraPawn::CalcMoveVelocity_Tick(float DeltaTime)
{
	// CASE calc movement on hold RMB
	if (bCameraShouldMove)  
	{
		FVector2D CursorLocation = 0;
		FVector2D ViewportSize = 0;
		float SpringArmLen = 0;

		if (auto PC = Cast<APlayerController>(GetController()))
		{
			PC->GetMousePosition(CursorLocation.X, CursorLocation.Y);
		}
		
		const auto Viewport = GetWorld()->GetGameViewport();
		Viewport->GetViewportSize(ViewportSize);

		const FVector ActorFwdVec = FRotator(0.f, SpringArmComp->GetComponentRotation().Yaw, 0.f).Vector();
		const float Dot = ActorFwdVec | FVector::RightVector;
		const FVector Cross = (ActorFwdVec ^ FVector::RightVector).GetSafeNormal();
		const float Sign = FMath::Sign(Cross | FVector::UpVector);
		const float Angle = acosf(Dot) * Sign;
		const float Sin = sinf(Angle);
		const float Cos = -cosf(Angle);

		const FVector2D CursorDelta = CursorLocation - CameraMoveCursorAnchor;
		const FVector2D CursorDirection = CursorDelta.GetSafeNormal();
		
		// 2d vector rotation:
		// X = Cos * X - Sin * Y; Y = Sin * X + Cos * Y; 
		const FVector MoveDirection = FVector(
			Cos * CursorDirection.X - Sin * CursorDirection.Y,
			Sin * CursorDirection.X + Cos * CursorDirection.Y,
			0.f
		);

		const float SpeedMult_Viewport = CursorDelta.Size() / ViewportSize.Y;
		const float SpeedMult_SpringArmLen = FMath::Max(0.2f, SpringArmComp->TargetArmLength / OnZoomSpringArmLenMax);
		const float CameraSpeed = CameraMoveSpeedMax * SpeedMult_Viewport * SpeedMult_SpringArmLen;

		Velocity = MoveDirection * FMath::Min(CameraMoveSpeedMax, CameraSpeed);
		AddActorWorldOffset(Velocity * DeltaTime);

		return;
	}

	// CASE movement on double LBM click
	if (!CursorPointMoveTo.IsZero())
	{
		const float SpeedScale = FMath::Max(0.2f, SpringArmComp->TargetArmLength / OnZoomSpringArmLenMax);
		const float Speed = CameraMoveSpeedMax * SpeedScale;
		FVector NewActorLoc = FMath::VInterpConstantTo(GetActorLocation(), CursorPointMoveTo, DeltaTime, Speed);
		SetActorLocation(NewActorLoc);

		// check if arrived to point destination
		if (NewActorLoc.Equals(CursorPointMoveTo))
		{
			CursorPointMoveTo = FVector::ZeroVector;
		}

		return;
	}

	// CASE calc breaking
	{
		Velocity = FMath::VInterpConstantTo(Velocity, FVector::ZeroVector, DeltaTime, BreakingSpeed);
		AddActorWorldOffset(Velocity * DeltaTime);
	}
}

FORCEINLINE void ACC_PlayerCameraPawn::RestoreCameraRotation_Tick(float DeltaTime)
{
	if (bRotationRestoreInProgress && SpringArmComp)
	{
		const FRotator OldCameraRot = SpringArmComp->GetComponentRotation();
		const FRotator NewCameraRot = FMath::RInterpTo(OldCameraRot, FRotator(CameraPitchInitial, 0.f, 0.f), 
			DeltaTime, CameraRotationRestoreSpeed);
		SpringArmComp->SetWorldRotation(NewCameraRot);

		if (FMath::IsNearlyEqual(NewCameraRot.Pitch, FRotator::NormalizeAxis(CameraPitchInitial), 0.5f) &&
			FMath::IsNearlyEqual(NewCameraRot.Yaw, 0.f, 0.5f))
		{
			bRotationRestoreInProgress = false;
		}
	}
}