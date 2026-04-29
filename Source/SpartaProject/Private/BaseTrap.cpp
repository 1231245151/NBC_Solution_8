// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTrap.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SpartaFloatingText.h"

// Sets default values
ABaseTrap::ABaseTrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    // 충돌 컴포넌트 생성 및 설정
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    // 겹침만 감지하는 프로파일 설정
    Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    // 루트 컴포넌트로 설정
    Collision->SetupAttachment(Scene);

    // 스태틱 메시 컴포넌트 생성 및 설정
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(Collision);
    // 메시가 불필요하게 충돌을 막지 않도록 하기 위해, 별도로 NoCollision 등으로 설정할 수 있음.

    // Overlap 이벤트 바인딩
    Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseTrap::OnTrapOverlap);
}

// Called when the game starts or when spawned
void ABaseTrap::BeginPlay()
{
	Super::BeginPlay();
    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("TrapEX by Trap")));
}

// Called every frame
void ABaseTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseTrap::OnTrapOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->ActorHasTag("Player"))
    {
        ActivateTrap();
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Trap is Player detected")));
    }
}

void ABaseTrap::OnTrapEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ABaseTrap::ActivateTrap()
{
    bIsActive = true;
}

void ABaseTrap::DeactivateTrap()
{
    bIsActive = false;
}

void ABaseTrap::SpawnFloatingText(FString Message, FColor Color)
{
    if (!FloatingTextClass)
        return;

    FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f);

    // 위젯액터 스폰
    ASpartaFloatingText* NewFloatingText = GetWorld()->SpawnActor<ASpartaFloatingText>(
        FloatingTextClass,
        SpawnLocation,
        FRotator::ZeroRotator
    );

    // 출력할 텍스트 보냄
    if (NewFloatingText)
    {
        NewFloatingText->InitializeText(Message, Color);
    }
}

