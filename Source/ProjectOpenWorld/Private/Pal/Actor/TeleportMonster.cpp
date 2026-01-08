#include "Pal/Actor/TeleportMonster.h"
#include "GameFramework/Character.h"

ATeleportMonster::ATeleportMonster()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void ATeleportMonster::BeginPlay()
{
	Super::BeginPlay();
}

void ATeleportMonster::OnInteractionEvent_Implementation(ACharacter* TargetMonster)
{
	if (TargetMonster)
		TargetMonster->SetActorLocation(GetActorLocation());
}