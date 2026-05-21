#include "Pal/Actor/PalBaseCamp_V2.h"
#include "Pal/Component/PalWorkCommander.h"
#include "Pal/Component/PalWorkableSearchComponent.h"

APalBaseCamp_V2::APalBaseCamp_V2() : Super{}
{
	PrimaryActorTick.bCanEverTick = false;
	PalWorkableSearchComponent = CreateDefaultSubobject<UPalWorkableSearchComponent>(TEXT("PalWorkableSearchComponent"));
	PalWorkableSearchComponent->SetupAttachment(GetRootComponent());
	PalWorkCommander = CreateDefaultSubobject<UPalWorkCommander>(TEXT("PalWorkCommander"));
}

void APalBaseCamp_V2::BeginPlay()
{
	if (PalWorkableSearchComponent && PalWorkCommander)
	{
		PalWorkableSearchComponent->OnComponentBeginOverlap.AddUniqueDynamic(PalWorkCommander.Get(), &UPalWorkCommander::OnBeginSearch);
		PalWorkableSearchComponent->OnComponentEndOverlap.AddUniqueDynamic(PalWorkCommander.Get(), &UPalWorkCommander::OnEndSearch);
	}
	Super::BeginPlay();
}

