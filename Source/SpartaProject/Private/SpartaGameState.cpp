#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "WaveDataRow.h"
#include "SpikeTrap.h"
#include "ExplosionTrap.h"
#include "GameFramework/Character.h"

ASpartaGameState::ASpartaGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    CurrentLevelIndex = 0;
    MaxLevels = 3;

    CurrentWaveIndex = 0;
}

void ASpartaGameState::BeginPlay()
{
    Super::BeginPlay();

    if (UWorld* World = GetWorld())
    {
        FString MapName = World->GetMapName();
        if (!MapName.Contains("MenuLevel"))
        {
            StartLevel();

            GetWorldTimerManager().SetTimer(
                HUDUpdateTimerHandle,
                this,
                &ASpartaGameState::UpdateHUD,
                0.1f,
                true
            );
        }
    }
}

void ASpartaGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorld()->GetTimerManager().ClearTimer(HUDUpdateTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(LevelTimerHandle);
    Super::EndPlay(EndPlayReason);
}

int32 ASpartaGameState::GetScore() const
{
    return Score;
}

TArray<FName>& ASpartaGameState::GetLevelMapNames()
{
    return LevelMapNames;
}

void ASpartaGameState::AddScore(int32 Amount)
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            SpartaGameInstance->AddToScore(Amount);
        }

    }
}

void ASpartaGameState::StartLevel()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->ShowGameHUD();
        }
    }


    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
        }

    }

    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;

    // 레벨 시작 시 웨이브도 시작
    StartWave();

    UE_LOG(LogTemp, Warning, TEXT("Level %d Start!, Spawned %d coin"), CurrentLevelIndex + 1, SpawnedCoinCount);
}

void ASpartaGameState::OnLevelTimeUp()
{
    EndWave();
}

void ASpartaGameState::OnCoinCollected()
{
    CollectedCoinCount++;
    UE_LOG(LogTemp, Warning, TEXT("Con Collected: %d / %d"), CollectedCoinCount, SpawnedCoinCount)

    if (SpawnedCoinCount > 0 &&CollectedCoinCount >= SpawnedCoinCount)
    {
        EndWave();
    }
}

void ASpartaGameState::EndLevel()
{
    GetWorldTimerManager().ClearTimer(LevelTimerHandle);

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            AddScore(Score);
            CurrentLevelIndex++;
            SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
        }

    }

    if (CurrentLevelIndex >= MaxLevels)
    {
        OnGameOver();
        return;
    }

    if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
    {
        //UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
        LevelClear();
    }
    else
    {
        OnGameOver();
    }
}

void ASpartaGameState::LevelClear()
{
    UE_LOG(LogTemp,Warning, TEXT("LevelChangeUI!"))
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->SetPause(true);
            SpartaPlayerController->ShowLevelClear();
        }
    }
}

void ASpartaGameState::NextLevel()
{
    UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
}

void ASpartaGameState::OnGameOver()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->SetPause(true);
            SpartaPlayerController->ShowGameOverMenu();
        }
    }
}

void ASpartaGameState::StartWave()
{
    // 데이터 테이블에서 웨이브 꺼냄
    TArray<FWaveDataRow*> AllRows;
    static const FString contextString(TEXT("WaveContext"));
    WaveDataTable->GetAllRows(contextString, AllRows);

    // 비어잇으면안되는데...? 어케왔냐
    if (AllRows.IsEmpty())
    {

    }


    // 스폰
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
    
    const int32 ItemToSpawn = 40;
    
    for (int32 i = 0; i < ItemToSpawn; ++i)
    {
        for (int32 j = 0; j < AllRows[CurrentWaveIndex]->ItemSpawnNum; ++j)
        {
            if (FoundVolumes.Num() > 0)
            {
                ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
                if (SpawnVolume)
                {
                    AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();

                    // 점수아이템 확인
                    ACoinItem* CoinItem = Cast<ACoinItem>(SpawnedActor);
                    if (CoinItem && CoinItem->IsGoodCoin())
                    {
                        SpawnedCoinCount++;
                    }
                }
            }
        }
    }

    // 웨이브에 따른 함정 스폰
    if (FoundVolumes.Num() > 0 && CurrentWaveIndex == 1)
    {
        ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
        SpawnVolume->SpawnTraps(SpikeTrapClass, 15);
    }
    if (FoundVolumes.Num() > 0 && CurrentWaveIndex == 2)
    {
        GetWorldTimerManager().SetTimer(ExplosionSpawnTimerHandle, this, &ASpartaGameState::SpawnExplosionAtPlayer, 5.0f, true, 0.1f);
    }

    // HUD갱신
    UpdateHUD();

    // 다음 웨이브를 위한 타이머 설정
    GetWorldTimerManager().SetTimer(LevelTimerHandle, this, &ASpartaGameState::OnLevelTimeUp, AllRows[CurrentWaveIndex]->WaveTimer, false);
}

void ASpartaGameState::SpawnExplosionAtPlayer()
{
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerCharacter)
    {
        FVector SpawnLocation = PlayerCharacter->GetActorLocation();
        SpawnLocation.Z = 0.f; // 바닥 높이 고정

        GetWorld()->SpawnActor<AExplosionTrap>(ExplosionTrapClass, SpawnLocation, FRotator::ZeroRotator);
    }
}

void ASpartaGameState::EndWave()
{
    GetWorldTimerManager().ClearTimer(LevelTimerHandle);

    // 데이터 테이블 검색
    TArray<FWaveDataRow*> AllRows;
    static const FString contextString(TEXT("WaveContext"));
    WaveDataTable->GetAllRows(contextString, AllRows);

    // 비어있다고? 그럼그냥 다음 레벨로가지뭐
    if (AllRows.IsEmpty())
    {
        EndLevel();
        return;
    }

    // 현재 웨이브증가
    ++CurrentWaveIndex;

    // 데이터 테이블에 다음 웨이브가 없다면 레벨 종료
    if (AllRows.Num() <= CurrentWaveIndex)
    {
        EndLevel();
        return;
    }
    else
    {
        WaveChange();
    }

}

void ASpartaGameState::WaveChange()
{
    // 웨이브 변경 알람
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT("Next Wave")));

    // 웨이브 변경
    StartWave();
}

void ASpartaGameState::UpdateHUD()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
            {
                if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
                    TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
                }

                if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
                {
                    if (UGameInstance* GameInstance = GetGameInstance())
                    {
                        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
                        if (SpartaGameInstance)
                        {
                            ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
                        }
                    }
                }

                if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
                {
                    LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
                }

                if (UTextBlock* WaveIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
                {
                    WaveIndexText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWaveIndex + 1)));
                }
            }
        }
    }
}
