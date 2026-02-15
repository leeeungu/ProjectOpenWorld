#include "Interaction/Actor/BossTower.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TargetPoint.h"
#include "GameBase/Subsystem/SoundGameInstanceSubsystem.h"

ABossTower::ABossTower() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	TowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMeshComponent"));
	TowerMeshComponent->SetupAttachment(RootComponent);

	BossTowerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("BossTowerWidgetComponent"));
	BossTowerWidgetComponent->SetupAttachment(RootComponent);
}

void ABossTower::BeginPlay()
{
	Super::BeginPlay();
	if(BossTowerWidgetComponent)
		BossTowerWidgetComponent->SetVisibility(false);
	if (BoosLanscape)
	{
		BoosLanscape->SetActorHiddenInGame(true);
	}
}

void ABossTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossTower::OnBeginDetected_Implementation(ACharacter* pOther)
{
	if (BossTowerWidgetComponent)
		BossTowerWidgetComponent->SetVisibility(true);
}

void ABossTower::OnEndDetected_Implementation(ACharacter* pOther)
{
	if (BossTowerWidgetComponent)
		BossTowerWidgetComponent->SetVisibility(false);
}

void ABossTower::OnInteractionStart_Implementation(ACharacter* pOther)
{
	if (BoosLanscape)
	{
		BoosLanscape->SetActorHiddenInGame(false);
	}
	if (BossPoint && pOther)
	{
		USoundGameInstanceSubsystem::PlayMainBGMSound(EBGMSoundType::BGMST_BossTower);
		pOther->SetActorLocation(BossPoint->GetActorLocation());
		pOther->SetActorRotation(BossPoint->GetActorRotation());
		pOther->GetController()->SetControlRotation(BossPoint->GetActorRotation());
		//pOther->GetController()->SetActorLocation(BossPoint->GetActorLocation());
	}
}

void ABossTower::OnInteraction_Implementation(ACharacter* pOther)
{
}

void ABossTower::OnInteractionEnd_Implementation(ACharacter* pOther)
{
}

void ABossTower::OnInteractionCanceled_Implementation(ACharacter* pOther)
{
}

