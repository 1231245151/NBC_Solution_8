// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TrapInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTrapInterface : public UInterface
{
	GENERATED_BODY()
};


class SPARTAPROJECT_API ITrapInterface
{
	GENERATED_BODY()


public:
    UFUNCTION()
    virtual void OnTrapOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult) = 0;
    UFUNCTION()
    virtual void OnTrapEndOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex) = 0;

    virtual void ActivateTrap() = 0;
    virtual void DeactivateTrap() = 0;
    virtual void SpawnFloatingText(FString Message, FColor Color) = 0;

};
