// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoinItem.h"
#include "BedCoinItem.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API ABedCoinItem : public ACoinItem
{
	GENERATED_BODY()

public:
    ABedCoinItem();

    virtual void ActivateItem(AActor* Activator) override;
};
