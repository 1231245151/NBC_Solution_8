// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()

public:
    ASpartaGameState();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // 전역 점수를 저장하는 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Score;
	// 현재 레벨에서 스폰된 코인 개수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	// 플레이어가 수집한 코인 개수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	// 현재 진행 중인 레벨 인덱스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	// 전체 레벨의 개수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	// 실제 레벨 맵 이름 배열. 여기 있는 인덱스를 차례대로 연동
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    FName MainMenuMapName;

    // 웨이브 데이터(각 웨이브 시간, 스폰횟수) 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    UDataTable* WaveDataTable;
    // 현재 진행 중인 웨이브 인덱스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 CurrentWaveIndex;

	// 매 레벨이 끝나기 전까지 시간이 흐르도록 관리하는 타이머
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

    // 현재 점수를 읽는 함수
    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const;
    UFUNCTION(BlueprintPure, Category = "LevelList")
    TArray<FName>& GetLevelMapNames();
    // 점수를 추가해주는 함수
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

    void StartWave();
    void EndWave();
    void WaveChange();

    void StartLevel();
	void OnLevelTimeUp();
	void OnCoinCollected();
	void EndLevel();
    void LevelClear();
    void NextLevel();

	void UpdateHUD();
};
