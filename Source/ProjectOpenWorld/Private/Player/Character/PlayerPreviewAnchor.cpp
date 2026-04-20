#include "Player/Character/PlayerPreviewAnchor.h"

APlayerPreviewAnchor::APlayerPreviewAnchor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APlayerPreviewAnchor::BeginPlay()
{
	Super::BeginPlay();
}
