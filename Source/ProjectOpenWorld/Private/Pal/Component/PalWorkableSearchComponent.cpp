#include "Pal/Component/PalWorkableSearchComponent.h"
#include "Pal/Interface/PalWorkable.h"

UPalWorkableSearchComponent::UPalWorkableSearchComponent() : USphereComponent{}
{
}

void UPalWorkableSearchComponent::PauseSearch()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UPalWorkableSearchComponent::StartSearch()
{
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UPalWorkableSearchComponent::BeginPlay()
{
	OnComponentEndOverlap.AddUniqueDynamic(this, &UPalWorkableSearchComponent::OnEndSearch);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UPalWorkableSearchComponent::OnEndSearch);
	Super::BeginPlay();
	for (const FOverlapInfo& OtherOverlap : OverlappingComponents)
	{
		UPrimitiveComponent* OtherComp = OtherOverlap.OverlapInfo.Component.Get();
		AActor* const OtherActor = OtherComp->GetOwner();
		OutPut.Add(OtherActor);
		OnComponentBeginOverlap.Broadcast(this, OtherActor, OtherComp, OtherOverlap.GetBodyIndex(), OtherOverlap.bFromSweep, OtherOverlap.OverlapInfo);
	}
}

void UPalWorkableSearchComponent::OnBeginSearch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (TScriptInterface<IPalWorkable > Workable = OtherActor)
	{
		OnAddWorkables.Broadcast(OtherActor);
	}
	OutPut.Add(OtherActor);
}

void UPalWorkableSearchComponent::OnEndSearch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	if (FVector::DistSquared(OtherActor->GetActorLocation(), GetOwner()->GetActorLocation()) <= SphereRadius * SphereRadius)
	{
		/*
		굉장히 특이한 상황 :
		StaticMeshComponent이 static object 로 변경되면서 actor의 위치가 변경이되고 actor의 위치 상관없이
		ONCOMPONENTENDOVERLAP이 호출 되지만 거리 상으로는 collision의 범위 내에 있음
		라고 생각했는데 제현이 안됨...
	*/
		FTimerHandle Handle{};
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
			{
				UE_LOG(LogTemp, Warning, TEXT("UPalWorkableSearchComponent::UpdateOverlaps"));
				UpdateOverlaps();  // 강제 갱신
				for (const FOverlapInfo& OtherOverlap : OverlappingComponents)
				{
					UPrimitiveComponent* OtherComp = OtherOverlap.OverlapInfo.Component.Get();
					AActor* const OtherActor = OtherComp->GetOwner();
					OutPut.Add(OtherActor);
					OnComponentBeginOverlap.Broadcast(this, OtherActor, OtherComp, OtherOverlap.GetBodyIndex(), OtherOverlap.bFromSweep, OtherOverlap.OverlapInfo);
				}
			});// , 0.5f, false, 0.5f);
		return;
	}

	if (TScriptInterface<IPalWorkable > Workable = OtherActor)
	{
		OnRemoveWorkables.Broadcast(OtherActor);
	}
	OutPut.Remove(OtherActor);
}