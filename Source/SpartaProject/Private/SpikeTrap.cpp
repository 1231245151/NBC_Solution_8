// Fill out your copyright notice in the Description page of Project Settings.


#include "SpikeTrap.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

ASpikeTrap::ASpikeTrap()
{
    bCanActive = false;
    bHasDamage = false;

    // 스파이크 메시
    SpikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeMesh"));
    // 겹침만 감지하는 프로파일 설정
    SpikeMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    // 루트 컴포넌트에 저장
    SpikeMesh->SetupAttachment(Scene);

    // 스파이크 전용 충돌 컴포넌트
    SpikeBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SpikeBoxCollision"));
    SpikeBoxCollision->SetupAttachment(Scene);
    SpikeBoxCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    SpikeBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnTrapOverlap);

    // 부모의 충돌체 쓸일 없음
    if (Collision)
    {
        Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Collision->SetGenerateOverlapEvents(false);
    }

    // 스파이크 초기 위치
    SpikeMesh->SetRelativeLocation(FVector(0.f, 0.f, -100.f));
    SpikeMesh->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnSpikeOverlap);
}

void ASpikeTrap::BeginPlay()
{
    Super::BeginPlay();

    // 생성 직후에는 대미지 불가능 상태
    bCanActive = false;
    GetWorld()->GetTimerManager().SetTimer(GraceTimerHandle, this, &ASpikeTrap::EnableDamage, 1.0f, false);
}

void ASpikeTrap::EnableDamage()
{
    bCanActive = true;
}

void ASpikeTrap::OnSpikeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 조건 : 활성화 + 이번 주기에 데미지 준적 없음 + 플레이어임
    if (bIsActive && !bHasDamage && OtherActor && OtherActor->ActorHasTag("Player"))
    {
        UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, nullptr, this, nullptr);
        bHasDamage = true; // 또 데미지주는 일 방지
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Player damaged %d by SpikeTrap"), DamageAmount));

        SpawnFloatingText(FString::Printf(TEXT("-HP %.0f"), DamageAmount), FColor::Red);
    }
}

void ASpikeTrap::ActivateTrap()
{
    // 중복 활성화 방지
    if (!bCanActive || bIsActive)
        return;

    bIsActive = true;
    bHasDamage = false;

    // 스파이크 올라옴
    SpikeMesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

    // 올라오는 사운드 재생
    if (SpikeSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            SpikeSound,
            GetActorLocation()
        );
    }

    // 1초 뒤 비활성화
    GetWorld()->GetTimerManager().SetTimer(DeactivateTimerHandle, this, &ASpikeTrap::DeactivateTrap, 1.0f, false);
}

void ASpikeTrap::DeactivateTrap()
{
    bIsActive = false;
    bHasDamage = false;
    SpikeMesh->SetRelativeLocation(FVector(0.f, 0.f, -100.f));
    GetWorld()->GetTimerManager().ClearTimer(DeactivateTimerHandle);
}
