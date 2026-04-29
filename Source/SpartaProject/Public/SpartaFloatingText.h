// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpartaFloatingText.generated.h"

UCLASS()
class SPARTAPROJECT_API ASpartaFloatingText : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpartaFloatingText();

    void InitializeText(const FString& Text, FColor Color);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "widget")
    class UWidgetComponent* TextWidget;

	virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void DestroyText();

    // 작동된 시간
    float RunningTime = 0.f;
    float EndRunningTime = 1.f;

    // 사라지는 시간
    float DestroyTime = 2.f;
};
