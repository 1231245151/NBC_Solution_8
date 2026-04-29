// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTrap.h"
#include "SpikeTrap.generated.h"

class UBoxComponent;

UCLASS()
class SPARTAPROJECT_API ASpikeTrap : public ABaseTrap
{
	GENERATED_BODY()

public:
    ASpikeTrap();

protected:
    virtual void BeginPlay() override;

    bool bCanActive;        // 활성 유예기간
    bool bHasDamage;        // 현재 솟아있는 동안 대미지 부여 여부

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap|Effects")
    USoundBase* SpikeSound;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
    UStaticMeshComponent* SpikeMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
    UBoxComponent* SpikeBoxCollision;

    FTimerHandle GraceTimerHandle;
    FTimerHandle DeactivateTimerHandle;

    // 스폰되자마자 활성화되어서 억까 방지용
    void EnableDamage();

    UFUNCTION()
    void OnSpikeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    virtual void ActivateTrap() override;
    virtual void DeactivateTrap() override;
};
