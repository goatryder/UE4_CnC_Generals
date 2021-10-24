// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CC_PlayerCameraPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class CNC_GENERALS_API ACC_PlayerCameraPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootComp;

	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComp;

	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

	// movement ------------------------------------------------ /

	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float CameraMoveSpeedMax;

	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float CameraMoveStopTime;

	// rotation ------------------------------------------------ /

	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float CameraRotationSpeed;

	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float CameraPitchMin;

	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float CameraPitchMax;

	// pitch value on start
	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float CameraPitchInitial;

	// if button is released and time sinse hold rotation <= restore delay, restore camera rotation to default
	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float CameraRotationRelease_RestoreDelay;

	// zoom ---------------------------------------------------- /

	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float CameraZoomSpeed;

	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float OnZoomSpringArmLenMin;
	
	UPROPERTY(Category = Camera, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float OnZoomSpringArmLenMax;
	


public:
	ACC_PlayerCameraPawn();

protected:

public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaTime) override;

private:

	// input --------------------------------------------------- /

	bool bCameraShouldRotate;
	bool bCameraShouldMove;
	bool bSelectIsPressed;

	bool MouseXYInputIsUsed() { return bCameraShouldMove || bCameraShouldRotate || bSelectIsPressed; }

	void CameraUpInput(float Val);
	void CameraRightInput(float Val);

	void OnSelect_Pressed();
	void OnSelect_Released();

	void OnRotateCamera_Pressed();
	void OnRotateCamera_Released();

	void OnMoveCamera_Pressed();
	void OnMoveCamera_Released();

	void OnCameraZoomIn();
	void OnCameraZoomOut();

	// movement ------------------------------------------------ /
	
	FVector Velocity;
	float BreakingSpeed;
	FVector2D CameraMoveCursorAnchor;
	void CalcMoveVelocity(float DeltaTime);

	// rotation ------------------------------------------------ /
	
	float CamRotButtonPressedTime;
	void RestoreCameraRotation();

};
