// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpartaPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class SPARTAPROJECT_API ASpartaPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASpartaPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD")
	UUserWidget* HUDWidgetInstance;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu")
	UUserWidget* MainMenuWidgetInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameOver")
    TSubclassOf<UUserWidget> GameOverWidgetClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameOver")
    UUserWidget* GameOverWidgetInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelClear")
    TSubclassOf<UUserWidget> LevelClearWidgetClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelClear")
    UUserWidget* LevelClearWidgetInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    TSubclassOf<UUserWidget> PlayerUIWidgetClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    UUserWidget* PlayerUIWidgetInstance;

	UFUNCTION(BlueprintPure, Category = "HUD")
	UUserWidget* GetHUDWidget() const;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainMenu(bool bIsRestart);
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void ShowGameOverMenu();
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void ReturnMainMenu();
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void EndGame();
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void NextLevelStage();
    void ShowLevelClear();
    void UpdatePlayerStat();

protected:
	virtual void BeginPlay() override;

};
