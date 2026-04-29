// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionTrap.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AExplosionTrap::AExplosionTrap()
{
    ExplosionDelay = 3.0f;
    ExplosionRadius = 300.f;
    DamageAmount = 30.0f;

    ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
    ExplosionCollision->InitSphereRadius(ExplosionRadius);
    ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ExplosionCollision->SetupAttachment(Scene);
}

void AExplosionTrap::BeginPlay()
{
    Super::BeginPlay();

    // 시작하자마자 트랩 활성화
    ActivateTrap();
}

void AExplosionTrap::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (EndPlayReason == EEndPlayReason::LevelTransition || EndPlayReason == EEndPlayReason::Quit)
    {
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().ClearTimer(DestroyParticleTimerHandle);
            GetWorld()->GetTimerManager().ClearTimer(ExplosionTimerHandle);
        }
    }
    Super::EndPlay(EndPlayReason);
}

void AExplosionTrap::ActivateTrap()
{
    // 한번만 활성화 됨
    if (bIsActive)
        return;

    bIsActive = true;
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("ExplosionTrap Activate")));

    // 폭파준비 사운드 재생
    if (ActiveSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            ActiveSound,
            GetActorLocation()
        );
    }

    // 타이머 활성화
    GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AExplosionTrap::Explosion, ExplosionDelay, false);
}

void AExplosionTrap::Explosion()
{
    UParticleSystemComponent* Particle = nullptr;
    if (ExplosionParticle)
    {
        Particle = UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ExplosionParticle,
            GetActorLocation(),
            GetActorRotation(),
            FVector(5.f,5.f,5.f),
            true
        );
    }

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            ExplosionSound,
            GetActorLocation()
        );
    }

    TArray<AActor*> OverlappingActors;
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Player"))
        {
            UGameplayStatics::ApplyDamage(Actor, DamageAmount, nullptr, this, UDamageType::StaticClass());
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Player damaged %d by ExplosionTrap"), DamageAmount));

            SpawnFloatingText(FString::Printf(TEXT("-HP %.0f"), DamageAmount), FColor::Red);
        }
    }

    Destroy();

    if (Particle)
    {
        TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

        GetWorld()->GetTimerManager().SetTimer(
            DestroyParticleTimerHandle,
            [WeakParticle]()
            {
                if (WeakParticle.IsValid())
                    WeakParticle->DestroyComponent();
            },
            3.0f,
            false
        );
    }
}
