#include "Landscape/Component/GemerateStaticObjectComponent.h"
#include "Landscape/Actor/WorldGenerator.h"
#include "GameBase/Interface/GenerateWorldInterface.h"
#include "Kismet/GameplayStatics.h"

UGemerateStaticObjectComponent::UGemerateStaticObjectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGemerateStaticObjectComponent::BeginPlay()
{
	Super::BeginPlay();
	SectionStaticObjectMap.Empty();
	GeneratorSectionComponent = Cast<AWorldGenerator>(GetOwner())->GetGeneratorSectionComponent();
	FOnActorSpawned::FDelegate del{};
	del.BindUObject(this, &UGemerateStaticObjectComponent::CommandActorSpawned);
	if (GetWorld())
	{
		GetWorld()->AddOnActorSpawnedHandler(del);
	}
	TArray<AActor*> Array{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Array);
	for (AActor* Actor : Array)
	{
		if (Cast<IGenerateWorldInterface>(Actor))
		{
			CommandActorSpawned(Actor);
		}
	}
}
void UGemerateStaticObjectComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (TSet<TScriptInterface<IGenerateWorldInterface>>* ActorSet = SectionStaticObjectMap.Find(SectionData.SectionID))
	{
		for (TScriptInterface<IGenerateWorldInterface> Actor : *ActorSet)
		{
			if (Actor && Actor.GetObject())
			{
				if (Cast<AActor>(Actor.GetObject()))
				{
					Cast<AActor>(Actor.GetObject())->SetActorTickEnabled(true);
					Cast<AActor>(Actor.GetObject())->SetActorHiddenInGame(false);
				}
				Actor->NewGenerateWorldEvent(SectionData);
			}
		}
	}
}
void UGemerateStaticObjectComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (TSet<TScriptInterface<IGenerateWorldInterface>>* ActorSet = SectionStaticObjectMap.Find(SectionData.SectionID))
	{
		for (TScriptInterface<IGenerateWorldInterface> Actor : *ActorSet)
		{
			if (Actor && Actor.GetObject())
			{
				if (Cast<AActor>(Actor.GetObject()))
				{
					Cast<AActor>(Actor.GetObject())->SetActorTickEnabled(false);
					Cast<AActor>(Actor.GetObject())->SetActorHiddenInGame(true);
				}
				Actor->DelGenerateWorldEvent(SectionData);
			}
		}
	}
}
void UGemerateStaticObjectComponent::CommandActorSpawned(AActor* SpawnActor)
{
	if (GeneratorSectionComponent)
	{
		if (Cast<IGenerateWorldInterface>(SpawnActor) || SpawnActor->Implements<UGenerateWorldInterface>())
		{
			FIntPoint SectionID = GeneratorSectionComponent->GetSectionIndex(SpawnActor->GetActorLocation());
			TSet<TScriptInterface<IGenerateWorldInterface>>& ActorSet = SectionStaticObjectMap.FindOrAdd(SectionID);
			ActorSet.Add(SpawnActor);
		}
		else 
		{
			//UE_LOG(LogTemp, Warning, TEXT("Spawn Actor Not  Implement  IGenerateWorldInterface (cpp Only) class : %s name : %s"), *SpawnActor->StaticClass()->GetName(), *SpawnActor->GetName());
		}
	}
}
//
//// Called every frame
//void UGemerateStaticObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}
//
