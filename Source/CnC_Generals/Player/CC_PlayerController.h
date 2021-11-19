// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CC_PlayerController.generated.h"

class ACC_ArrowManager;

/**
 * 
 */
UCLASS()
class CNC_GENERALS_API ACC_PlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACC_ArrowManager> ArrowManagerClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ACC_ArrowManager* ArrowManager;

protected:
	void BeginPlay() override;

public:
	ACC_PlayerController();

	ACC_ArrowManager* GetArrowManager() const { return ArrowManager; }
	
};
