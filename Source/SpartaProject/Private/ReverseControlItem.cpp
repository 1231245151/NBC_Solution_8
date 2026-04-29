#include "ReverseControlItem.h"
#include "SpartaCharacter.h"

AReverseControlItem::AReverseControlItem()
{
    Duration = 5.f;
}

void AReverseControlItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
        {
            // 캐릭터 슬로우
            PlayerCharacter->TakeReverseControl(Duration);
        }
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Player Debuff : Reversecontrol")));
        SpawnFloatingText(FString::Printf(TEXT("Reversecontrol!")), FColor::Yellow);
        DestroyItem();
    }
}
