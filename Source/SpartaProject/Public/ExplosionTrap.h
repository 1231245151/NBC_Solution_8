// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTrap.h"
#include "ExplosionTrap.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API AExplosionTrap : public ABaseTrap
{
	GENERATED_BODY()

public:
    AExplosionTrap();

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap|Component")
    USphereComponent* ExplosionCollision;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap|Effects")
    UParticleSystem* ExplosionParticle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap|Effects")
    USoundBase* ExplosionSound;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap|Effects")
    USoundBase* ActiveSound;

    bool bIsExploded;

    // 폭발까지 걸리는 시간 (5초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
    float ExplosionDelay;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
    float ExplosionRadius;

    // 폭팔 타이머
    FTimerHandle ExplosionTimerHandle;
    // 파티클 타이머
    FTimerHandle DestroyParticleTimerHandle;


    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    // 등장하자마자 활성화
    virtual void ActivateTrap() override;

    // 폭팔
    void Explosion();

};
