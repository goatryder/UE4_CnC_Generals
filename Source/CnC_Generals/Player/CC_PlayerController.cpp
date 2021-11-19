// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_PlayerController.h"

#include "CC_ArrowManager.h"

ACC_PlayerController::ACC_PlayerController()
{
	bShowMouseCursor = true;
}

void ACC_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ArrowManagerClass)
	{
		if (UWorld* World = GetWorld())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			
			ArrowManager = World->SpawnActor<ACC_ArrowManager>(ArrowManagerClass, SpawnParams);
		}
	}
}