#include "Pal/Component/PalPatrolComponent.h"
#include "Pal/DataTable/PalMonsterData.h"
#include "NavigationSystem.h"
#include "BrainComponent.h"
#include "AIController.h"
#include "GameBase/BaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UPalPatrolComponent::UPalPatrolComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UPalPatrolComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UPalPatrolComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPalPatrolComponent::SetPatrolData(const FPalMonsterPatrolData* InPatrolData)
{
	PatrolDataPtr = InPatrolData;
	ACharacter* OnwerCharacter = Cast<ACharacter>(GetOwner());
	if (OnwerCharacter && PatrolDataPtr)
	{
		if (AAIController* AIController = Cast< AAIController>(OnwerCharacter->GetController()))
		{
			AIController->GetBrainComponent()->GetBlackboardComponent()->SetValueAsBool(TEXT("bRandomPatrol"), PatrolDataPtr->bRandom);
		}
	}

}

void UPalPatrolComponent::UpdatePatrolIndex()
{
	if (!PatrolDataPtr || PatrolDataPtr->PatrolPoint.Num() == 0)
	{
		return;
	}
	else if (!PatrolDataPtr->bRandom)
	{
		CurrentPatrolIndex++;
		if (CurrentPatrolIndex >= PatrolDataPtr->PatrolPoint.Num())
		{
			if (PatrolDataPtr->bLoop)
			{
				CurrentPatrolIndex = 0;
			}
			else
			{
				CurrentPatrolIndex = PatrolDataPtr->PatrolPoint.Num() - 1; // Stay at last point
			}
		}
	}

}

FVector UPalPatrolComponent::GetCurrentPatrolPoint() const
{
	if (PatrolDataPtr && PatrolDataPtr->bRandom)
	{
		FNavLocation NavLocation{};
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		while (NavSys && NavSys->GetRandomPointInNavigableRadius(GetOwner()->GetActorLocation(), PatrolDataPtr->RandomRadius, NavLocation) && NavLocation.Location.Z <= 0)
		{
			//DrawDebugBox(GetWorld(), NavLocation.Location, FVector(25.0f), FColor::Green, false, 2.0f);
		}
		DrawDebugBox(GetWorld(), NavLocation.Location, FVector(25.0f), FColor::Black, false, 2.0f);
		return NavLocation.Location;
	}
	else if (PatrolDataPtr && PatrolDataPtr->PatrolPoint.IsValidIndex(CurrentPatrolIndex))
	{
		return PatrolDataPtr->PatrolPoint[CurrentPatrolIndex];
	}
	//else if (!PatrolDataPtr)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Patrol Data is nullptr"));
	//}
	if (GetOwner())
	{
		return GetOwner()->GetActorLocation();
	}
	return FVector::ZeroVector;
}

