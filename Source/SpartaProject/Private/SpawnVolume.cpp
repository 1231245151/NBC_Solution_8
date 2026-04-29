// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawning"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}
	return nullptr;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if(!ItemDataTable)
		return nullptr;

	TArray<FItemSpawnRow*> AllRows;
	static const FString contextString(TEXT("ItemSpawnContext"));
	ItemDataTable->GetAllRows(contextString, AllRows);

	if (AllRows.IsEmpty())
		return nullptr;

	float TotalChance = 0.f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	const float RandValue = FMath::FRandRange(0.f, TotalChance);
	float AccumarateChance = 0.f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumarateChance += Row->SpawnChance;
		if (RandValue <= AccumarateChance)
		{
			return Row;
		}
	}

	return nullptr;
}


FVector ASpawnVolume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();
	// (200, 100, 50) (2, 1, 1) -> (400, 100, 50)
	// 중심 -> 끝

	return BoxOrigin + FVector(FMath::FRandRange(-BoxExtent.X, BoxExtent.X), FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y), FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z));
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) 
		return nullptr;

	AActor* SpawnedItem = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator
	);

	return SpawnedItem;
}

void ASpawnVolume::SpawnTraps(TSubclassOf<AActor> TrapClass, int32 Count)
{
    if (!TrapClass)
        return;

    // 스폰위치 저장용(겹치기 방지)
    TArray<FVector> SpawnedLocations;

    // 트랩 겹치기 방지 최소 거리
    float MinDistance = 200.f;

    // 트랩 스폰 루프
    for (int32 i = 0; i < Count; ++i)
    {
        FVector SpawnLocation;
        bool bTooClose = true;

        // 겹치지 않는 위치를 찾을 때까지 반복
        while (bTooClose)
        {
            SpawnLocation = GetRandomPointInVolume();

            // z축은 랜덤이 아닌 0.5로 고정
            SpawnLocation.Z = 0.5f;

            bTooClose = false;

            // 이미 스폰된 트랩들과의 거리 비교 (수학적 체크)
            for (FVector& OtherLoc : SpawnedLocations)
            {
                if (FVector::DistXY(SpawnLocation, OtherLoc) < MinDistance)
                {
                    bTooClose = true;
                    break;
                }
            }
        }

        // 최종 결정된 위치에 스폰
        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
            TrapClass,
            SpawnLocation,
            FRotator::ZeroRotator
        );

        // 스폰겹치지 않게 배열에 저장
        if (SpawnedActor)
        {
            SpawnedLocations.Add(SpawnLocation);
        }
    }

}

