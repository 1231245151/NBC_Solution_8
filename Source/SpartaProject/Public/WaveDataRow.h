// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaveDataRow.generated.h"

USTRUCT(BlueprintType)
struct FWaveDataRow : public FTableRowBase
{
    GENERATED_BODY()
public:
    // 웨이브 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString WaveName;
    // 이 웨이브의 아이템 스폰 횟수
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ItemSpawnNum = 0;
    // 이 웨이브의 타이머
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WaveTimer = 0.f;
};
