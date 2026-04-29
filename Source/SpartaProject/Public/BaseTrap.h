#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapInterface.h"
#include "BaseTrap.generated.h"

class USphereComponent;
class ASpartaFloatingText;

UCLASS()
class SPARTAPROJECT_API ABaseTrap : public AActor, public ITrapInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseTrap();

protected:
    virtual void BeginPlay() override;
    //virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // 트랩 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName TrapType;

    // 피해량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
    float DamageAmount = 10.0f;

    bool bIsActive = false;     // 활성화 여부

    // 루트 컴포넌트 (씬)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
    USceneComponent* Scene;
    // 충돌 컴포넌트 (플레이어 진입 범위 감지용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
    USphereComponent* Collision;
    // 아이템 시각 표현용 스태틱 메시
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
    UStaticMeshComponent* StaticMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<ASpartaFloatingText> FloatingTextClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


    // ITrapInterface을(를) 통해 상속됨
    virtual void OnTrapOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
    virtual void OnTrapEndOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex) override;
    virtual void ActivateTrap() override;
    virtual void DeactivateTrap() override;
    virtual void SpawnFloatingText(FString Message, FColor Color) override;

};
