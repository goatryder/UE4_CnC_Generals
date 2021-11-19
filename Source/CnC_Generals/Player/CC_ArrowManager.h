// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CC_ArrowManager.generated.h"

UCLASS()
class CNC_GENERALS_API ACC_ArrowManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ACC_ArrowManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
