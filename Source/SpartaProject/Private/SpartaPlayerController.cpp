#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "SpartaGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SpartaCharacter.h"
#include "Components/ProgressBar.h"

ASpartaPlayerController::ASpartaPlayerController()
	:InputMappingContext(nullptr)
	, MoveAction(nullptr)
	, JumpAction(nullptr)
	, LookAction(nullptr)
	, SprintAction(nullptr)
	, HUDWidgetClass(nullptr)
	, HUDWidgetInstance(nullptr)
	, MainMenuWidgetClass(nullptr)
	, MainMenuWidgetInstance(nullptr)
    , GameOverWidgetClass(nullptr)
    , GameOverWidgetInstance(nullptr)
    , LevelClearWidgetClass(nullptr)
    , LevelClearWidgetInstance(nullptr)
    , PlayerUIWidgetClass(nullptr)
    , PlayerUIWidgetInstance(nullptr)
{
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	// 게임 실행 시 메뉴 레벨에서 메뉴 UI 먼저 표시
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{

	return HUDWidgetInstance;
}

void ASpartaPlayerController::ShowGameHUD()
{
	// HUD가 켜져 있다면 닫기
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	// 이미 메뉴가 떠 있으면 제거
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

    // 이미 게임오버가 떠 있으면 제거
    if (GameOverWidgetInstance)
    {
        GameOverWidgetInstance->RemoveFromParent();
        GameOverWidgetInstance = nullptr;
    }
    if (PlayerUIWidgetInstance)
    {
        PlayerUIWidgetInstance->RemoveFromParent();
        PlayerUIWidgetInstance = nullptr;
    }

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());

			ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
			if (SpartaGameState)
			{
				SpartaGameState->UpdateHUD();
			}
		}
	}

    if (PlayerUIWidgetClass)
    {
        PlayerUIWidgetInstance = CreateWidget<UUserWidget>(this, PlayerUIWidgetClass);
        if (PlayerUIWidgetInstance)
        {
            PlayerUIWidgetInstance->AddToViewport();

            bShowMouseCursor = false;
            SetInputMode(FInputModeGameOnly());

            // 생성 직후 데이터 반영
            UpdatePlayerStat();
        }
    }
}

void ASpartaPlayerController::ShowMainMenu(bool bIsRestart)
{
	// HUD가 켜져 있다면 닫기
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}
    if (PlayerUIWidgetInstance)
    {
        PlayerUIWidgetInstance->RemoveFromParent();
        PlayerUIWidgetInstance = nullptr;
    }

	// 이미 메뉴가 떠 있으면 제거
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

    // 이미 게임오버가 떠 있으면 제거
    if (GameOverWidgetInstance)
    {
        GameOverWidgetInstance->RemoveFromParent();
        GameOverWidgetInstance = nullptr;
    }

    // 레벨 클리어가 떠 있으면 제거
    if (LevelClearWidgetInstance)
    {
        LevelClearWidgetInstance->RemoveFromParent();
        LevelClearWidgetInstance = nullptr;
    }


	// 메뉴 UI 생성
	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
		if (PlayAnimFunc)
		{
			MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
		}
		
	}
}

void ASpartaPlayerController::StartGame()
{
	if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SpartaGameInstance->CurrentLevelIndex = 0;
		SpartaGameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);

    ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
    if (SpartaGameState)
    {
        SpartaGameState->StartLevel();
    }
}

void ASpartaPlayerController::ShowGameOverMenu()
{
    // HUD가 켜져 있다면 닫기
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }

    // 이미 메뉴가 떠 있으면 제거
    if (MainMenuWidgetInstance)
    {
        MainMenuWidgetInstance->RemoveFromParent();
        MainMenuWidgetInstance = nullptr;
    }

    // 이미 게임오버가 떠 있으면 제거
    if (GameOverWidgetInstance)
    {
        GameOverWidgetInstance->RemoveFromParent();
        GameOverWidgetInstance = nullptr;
    }

    // 레벨 클리어가 떠 있으면 제거
    if (LevelClearWidgetInstance)
    {
        LevelClearWidgetInstance->RemoveFromParent();
        LevelClearWidgetInstance = nullptr;
    }


    // 게임오버 UI 생성
    if (GameOverWidgetClass)
    {
        GameOverWidgetInstance = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
        if (GameOverWidgetInstance)
        {
            GameOverWidgetInstance->AddToViewport();

            bShowMouseCursor = true;
            SetInputMode(FInputModeUIOnly());
        }

        UFunction* PlayAnimFunc = GameOverWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
        if (PlayAnimFunc)
        {
            GameOverWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
        }

        if (UTextBlock* TotalScoreText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName("TotalScoreText")))
        {
            if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
            {
                TotalScoreText->SetText(FText::FromString(
                    FString::Printf(TEXT("Total Score: %d"), SpartaGameInstance->TotalScore)
                ));
            }
        }
        
    }
}

void ASpartaPlayerController::EndGame()
{
    // 프로그램 종료
    UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
}

void ASpartaPlayerController::NextLevelStage()
{
    // state에게 다음레벨로 가라고 말하기
    ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
    if (SpartaGameState)
    {
        SpartaGameState->NextLevel();
    }
}

void ASpartaPlayerController::ShowLevelClear()
{
    // HUD가 켜져 있다면 닫기
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }

    // 이미 메뉴가 떠 있으면 제거
    if (MainMenuWidgetInstance)
    {
        MainMenuWidgetInstance->RemoveFromParent();
        MainMenuWidgetInstance = nullptr;
    }

    // 이미 게임오버가 떠 있으면 제거
    if (GameOverWidgetInstance)
    {
        GameOverWidgetInstance->RemoveFromParent();
        GameOverWidgetInstance = nullptr;
    }

    // 레벨 클리어가 떠 있으면 제거
    if (LevelClearWidgetInstance)
    {
        LevelClearWidgetInstance->RemoveFromParent();
        LevelClearWidgetInstance = nullptr;
    }

    // 레벨 클리어 UI 생성
    if (LevelClearWidgetClass)
    {
        LevelClearWidgetInstance = CreateWidget<UUserWidget>(this, LevelClearWidgetClass);
        if (LevelClearWidgetInstance)
        {
            LevelClearWidgetInstance->AddToViewport();

            bShowMouseCursor = true;
            SetInputMode(FInputModeUIOnly());
        }

        UFunction* PlayAnimFunc = LevelClearWidgetInstance->FindFunction(FName("PlayLevelClearAnim"));
        if (PlayAnimFunc)
        {
            LevelClearWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
        }

        // 클리어 한 맵, 다음맵
        ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
        if (SpartaGameState)
        {
            if (UTextBlock* TotalScoreText = Cast<UTextBlock>(LevelClearWidgetInstance->GetWidgetFromName("LevelText")))
            {
                if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
                {
                    TotalScoreText->SetText(FText::FromString(
                        FString::Printf(TEXT("Next Level: %d"), SpartaGameState->CurrentLevelIndex + 1)
                    ));
                }
            }
        }
    }
}

void ASpartaPlayerController::UpdatePlayerStat()
{
    if (!PlayerUIWidgetInstance)
        return;

    ASpartaCharacter* Pcharact = Cast<ASpartaCharacter>(GetPawn());
    if (!Pcharact)
        return;

    // HP바
    if (UProgressBar* HPBar = Cast<UProgressBar>(PlayerUIWidgetInstance->GetWidgetFromName(TEXT("HPBar"))))
    {
        float HPPercent = (Pcharact->GetMaxHealth() > 0) ? ((float)Pcharact->GetHealth() / (float)Pcharact->GetMaxHealth()) : 0.0f;
        HPBar->SetPercent(HPPercent);
    }

    // HP텍스트
    if (UTextBlock* HPText = Cast<UTextBlock>(PlayerUIWidgetInstance->GetWidgetFromName(TEXT("HPText"))))
    {
        HPText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"),
            Pcharact->GetHealth(), Pcharact->GetMaxHealth())));
    }

    // 상태이상
    if (UTextBlock* StatusText = Cast<UTextBlock>(PlayerUIWidgetInstance->GetWidgetFromName(TEXT("StatusText"))))
    {
        FString Msg = "";
        if (Pcharact->GetSlowStackCount() > 0)
            Msg += FString::Printf(TEXT("[Slow] stack :%d (%.1fs)\n"), Pcharact->GetSlowStackCount(), Pcharact->GetSlowStackMaxRemainTime());

        if (Pcharact->IsReverseControl())
            Msg += FString::Printf(TEXT("[Reverse Control] (%.1fs)\n"), Pcharact->GetReverseControlRemainTime());

        StatusText->SetText(FText::FromString(Msg));
    }
}


void ASpartaPlayerController::ReturnMainMenu()
{
    // 레벨 변경 후 UI표기
    UGameplayStatics::OpenLevel(GetWorld(), "MenuLevel");
    ShowMainMenu(false);
}
