#include "CoinItem.h"
//#include "Engine/World.h"
#include "SpartaGameState.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

bool ACoinItem::IsGoodCoin()
{
	return bIsGoodCoin;
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
			{
				GameState->AddScore(PointValue);

                if(bIsGoodCoin)
				    GameState->OnCoinCollected();
			}
		}
		
		DestroyItem();
	}

}
