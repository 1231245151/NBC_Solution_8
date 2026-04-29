// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowItem.h"
#include "SpartaCharacter.h"

ASlowItem::ASlowItem()
{
    Duration = 10.f;
}

void ASlowItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
        {
            // 캐릭터 슬로우
            PlayerCharacter->TakeSlowning(Duration);
        }
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Player Debuff : Slow")));
        SpawnFloatingText(FString::Printf(TEXT("Slow!")), FColor::Yellow);
        DestroyItem();
    }
}
