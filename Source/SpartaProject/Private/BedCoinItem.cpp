// Fill out your copyright notice in the Description page of Project Settings.


#include "BedCoinItem.h"

ABedCoinItem::ABedCoinItem()
{
    PointValue = -50;
    ItemType = "BedCoin";
    bIsGoodCoin = false;
}

void ABedCoinItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    SpawnFloatingText(FString::Printf(TEXT("%d"), PointValue), FColor::Red);
}
