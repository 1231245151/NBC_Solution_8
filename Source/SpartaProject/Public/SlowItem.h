// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowItem.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API ASlowItem : public ABaseItem
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debuff")
    float Duration;

    ASlowItem();

protected:
    virtual void ActivateItem(AActor* Activator) override;

};
