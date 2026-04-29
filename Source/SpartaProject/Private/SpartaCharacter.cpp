#include "SpartaCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
    OverheadWidget->SetupAttachment(GetMesh());
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

    NormalSpeed = 600.0f;
    SprintSpeedMultiplier = 1.5f;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

    MaxHealth = 100.f;
    Health = MaxHealth;
}

int32 ASpartaCharacter::GetHealth() const
{
    return Health;
}

int32 ASpartaCharacter::GetMaxHealth() const
{
    return MaxHealth;
}

void ASpartaCharacter::AddHealth(float Amount)
{
    // 체력을 회복시킴. 최대 체력을 초과하지 않도록 제한함
    Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
    UE_LOG(LogTemp, Log, TEXT("Health increased to: %f"), Health);
    UpdateOverheadHP();
    UpdatePlayerUI();
}

void ASpartaCharacter::TakeSlowning(float _DurationTime)
{
    // 슬로우 스택 추가
    SlowStack.Add(_DurationTime);

    // 타이머가 활성 안되어있다 = 이제 처음 켜졌다.
    if (!GetWorldTimerManager().IsTimerActive(SlowningRemainTimer))
    {
        // 0.1초마다 갱신
        GetWorldTimerManager().SetTimer(SlowningRemainTimer, this, &ASpartaCharacter::CheckSlowOver, 0.1f, true);
    }

    // 상태이상 ui갱신을 위함
    if (!GetWorldTimerManager().IsTimerActive(StatusTimerHandle))
        GetWorldTimerManager().SetTimer(StatusTimerHandle, this, &ASpartaCharacter::CheckStatusEffects, 0.1f, true);
}

void ASpartaCharacter::CheckSlowOver()
{
    // TArray를 돌며 끝난 타이머 스택은 삭제
    for (int32 i = 0; i < SlowStack.Num();)
    {
        // 시간 감소(갱신 시간이 0.1초임)
        SlowStack[i] -= 0.1;

        if (SlowStack[i] <= 0.0f)
        {
            SlowStack.RemoveAt(i);
        }
        else
        {
            ++i; // 삭제 안 했을 때만 다음 요소로
        }
    }

    // TArray가 비었다면 타이머 종료
    if (SlowStack.IsEmpty())
    {
        GetWorldTimerManager().ClearTimer(SlowningRemainTimer);
    }
}

void ASpartaCharacter::TakeReverseControl(float _DurationTime)
{
    bIsReverseControl = true;

    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(ReverseControlTimer);
    if (RemainingTime < 0.0f)
    {
        RemainingTime = 0.0f;
    }
    GetWorldTimerManager().SetTimer(ReverseControlTimer, this, &ASpartaCharacter::ReverseControlOver, _DurationTime + RemainingTime, false);

    // 상태이상 ui갱신을 위함
    if (!GetWorldTimerManager().IsTimerActive(StatusTimerHandle))
        GetWorldTimerManager().SetTimer(StatusTimerHandle, this, &ASpartaCharacter::CheckStatusEffects, 0.1f, true);
}

void ASpartaCharacter::ReverseControlOver()
{
    bIsReverseControl = false;
}

int32 ASpartaCharacter::GetSlowStackCount() const
{
    return SlowStack.Num();
}

float ASpartaCharacter::GetSlowStackMaxRemainTime() const
{
    float MaxTime = 0.0f;
    for (float RemainTime : SlowStack)
    {
        if (RemainTime > MaxTime)
        {
            MaxTime = RemainTime;
        }
    }
    return MaxTime;
}

bool ASpartaCharacter::IsReverseControl() const
{
    return bIsReverseControl;
}

float ASpartaCharacter::GetReverseControlRemainTime() const
{
    float Remaining = GetWorldTimerManager().GetTimerRemaining(ReverseControlTimer);
    if (Remaining < 0)
        Remaining = 0;

    return Remaining;
}

void ASpartaCharacter::OnDeath()
{
    ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
    if (SpartaGameState)
    {
        SpartaGameState->OnGameOver();
    }
    UE_LOG(LogTemp, Error, TEXT("Character is Dead!"));
}

float ASpartaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 체력을 데미지만큼 감소시키고, 0 이하로 떨어지지 않도록 Clamp
    Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
    UE_LOG(LogTemp, Warning, TEXT("Health decreased to: %f"), Health);
    UpdateOverheadHP();
    UpdatePlayerUI();

    // 체력이 0 이하가 되면 사망 처리
    if (Health <= 0.0f)
    {
        OnDeath();
    }


    return ActualDamage;
}

// Called when the game starts or when spawned
void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
    UpdateOverheadHP();
}

// Called every frame
void ASpartaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 캐스팅 작업
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// GetController 캐스팅 작업
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			// 이동액션 바인딩
            if (PlayerController->MoveAction)
            {
                // IA_Move 액션 키를 "키를 누르고 있는 동안" Move() 호출
                EnhancedInput->BindAction(
                    PlayerController->MoveAction,
                    ETriggerEvent::Triggered,
                    this,
                    &ASpartaCharacter::Move
                );
            }

            // 점프액션 바인딩
            if (PlayerController->JumpAction)
            {
                // IA_Jump 액션 키를 "키를 누르고 있는 동안" StartJump() 호출
                EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Triggered,
                    this,
                    &ASpartaCharacter::StartJump
                );

                // IA_Jump 액션 키에서 "손을 뗀 순간" StopJump() 호출
                EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Completed,
                    this,
                    &ASpartaCharacter::StopJump
                );
            }

            // 마우스 액션 바인딩
            if (PlayerController->LookAction)
            {
                // IA_Look 액션 마우스가 "움직일 때" Look() 호출
                EnhancedInput->BindAction(
                    PlayerController->LookAction,
                    ETriggerEvent::Triggered,
                    this,
                    &ASpartaCharacter::Look
                );
            }

            // 달리기 액션 바인딩
            if (PlayerController->SprintAction)
            {
                // IA_Sprint 액션 키를 "누르고 있는 동안" StartSprint() 호출
                EnhancedInput->BindAction(
                    PlayerController->SprintAction,
                    ETriggerEvent::Triggered,
                    this,
                    &ASpartaCharacter::StartSprint
                );
                // IA_Sprint 액션 키에서 "손을 뗀 순간" StopSprint() 호출
                EnhancedInput->BindAction(
                    PlayerController->SprintAction,
                    ETriggerEvent::Completed,
                    this,
                    &ASpartaCharacter::StopSprint
                );
            }
		}
	}
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
    // 컨트롤러가 있어야 방향 계산이 가능
    // 컨트롤러의 위치는 카메라 기준
    // 애초에 컨트롤러가 잇어야 여기 진입가능. 예외상황방어용
    if (!Controller)
        return;

    FVector2D MoveInput = value.Get<FVector2D>();
    MoveInput.Normalize();

    // 슬로우 배수 적용(최소 0.2배)
    int32 StackCount = SlowStack.Num();
    float Multiple = FMath::Clamp(1.0f - (StackCount * 0.2f), 0.2f, 1.0f);
    MoveInput.X *= Multiple;
    MoveInput.Y *= Multiple;

    // 아주 미세한 입력값을 걸러내기 위한 방어 코드
    // MoveInput.X가 거의 0이라면 사실상 이동할 의사가 없다는 뜻
    if (!FMath::IsNearlyZero(MoveInput.X))
    {
        if(bIsReverseControl)
            AddMovementInput(GetActorForwardVector(), -MoveInput.X);
        else
            AddMovementInput(GetActorForwardVector(), MoveInput.X);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        // 캐릭터의 오른쪽 방향으로 Y축 이동
        if (bIsReverseControl)
            AddMovementInput(GetActorRightVector(), -MoveInput.Y);
        else
            AddMovementInput(GetActorRightVector(), MoveInput.Y);
    }
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
    if (value.Get<bool>())
    {
        Jump();
        //UE_LOG(LogTemp, Log, TEXT("StartJump!!"));
    }
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
    if (!value.Get<bool>())
    {
        StopJumping();
        //UE_LOG(LogTemp, Log, TEXT("StopJump!!"));
    }
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{
    FVector2D LookInput = value.Get<FVector2D>();

    AddControllerYawInput(LookInput.X);
    AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}

void ASpartaCharacter::UpdateOverheadHP()
{
    if (!OverheadWidget)
        return;

    UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
    if (!OverheadWidgetInstance)
        return;

    if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverheadHP"))))
    {
        HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
    }
}

void ASpartaCharacter::CheckStatusEffects()
{
    UpdatePlayerUI();

    // 아무 상태 이상도 없으면 타이머 종료
    if (SlowStack.IsEmpty() && !bIsReverseControl)
    {
        GetWorldTimerManager().ClearTimer(StatusTimerHandle);
    }
}

void ASpartaCharacter::UpdatePlayerUI()
{
    if (ASpartaPlayerController* PC = Cast<ASpartaPlayerController>(GetController()))
    {
        PC->UpdatePlayerStat();
    }
}
