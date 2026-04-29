#include "SpartaFloatingText.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"


ASpartaFloatingText::ASpartaFloatingText()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    TextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TextWidget"));
    SetRootComponent(TextWidget);
    TextWidget->SetWidgetSpace(EWidgetSpace::Screen);

}

void ASpartaFloatingText::InitializeText(const FString& Text, FColor Color)
{
    if (UUserWidget* widgetInstance = TextWidget->GetUserWidgetObject())
    {
        UTextBlock* widgetText = Cast<UTextBlock>(widgetInstance->GetWidgetFromName(TEXT("FloatingText")));
        if (widgetText)
        {
            widgetText->SetText(FText::FromString(Text));
            widgetText->SetColorAndOpacity(FSlateColor(Color));
        }
    }

}

// Called when the game starts or when spawned
void ASpartaFloatingText::BeginPlay()
{
	Super::BeginPlay();

    FTimerHandle DestroyTimerHandle;
    GetWorldTimerManager().SetTimer(
        DestroyTimerHandle,
        this,
        &ASpartaFloatingText::DestroyText,
        2.0f,                             
        false                             
    );
}

void ASpartaFloatingText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // 위로 서서히 뜸
    if (RunningTime <= EndRunningTime)
    {
        RunningTime += DeltaTime;
        AddActorWorldOffset(FVector(0.f, 0.f, 10.f * DeltaTime));
    }
}

void ASpartaFloatingText::DestroyText()
{
    Destroy();
}

