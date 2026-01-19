#include "Landscape/Component/GemerateStaticObjectComponent.h"
#include "Landscape/Actor/WorldGenerator.h"

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
	//GetWorld()->GetLevel()->Actors GetActorCount
}
void UGemerateStaticObjectComponent::NewGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	if (TSet<TObjectPtr<AActor>>* ActorSet = SectionStaticObjectMap.Find(SectionID))
	{
		for (TObjectPtr<AActor> Actor : *ActorSet)
		{
			if (Actor)
			{
				Actor->SetActorTickEnabled(true);
				Actor->SetActorHiddenInGame(false);
			}
		}
	}
}
void UGemerateStaticObjectComponent::DelGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	if (TSet<TObjectPtr<AActor>>* ActorSet = SectionStaticObjectMap.Find(SectionID))
	{
		for (TObjectPtr<AActor> Actor : *ActorSet)
		{
			if (Actor)
			{
				Actor->SetActorTickEnabled(false);
				Actor->SetActorHiddenInGame(true);
			}
		}
	}
}
void UGemerateStaticObjectComponent::CommandActorSpawned(AActor* SpawnActor)
{
	if (GeneratorSectionComponent)
	{
		FIntPoint SectionID = GeneratorSectionComponent->GetSectionIndex(SpawnActor->GetActorLocation());
		TSet<TObjectPtr<AActor>>& ActorSet = SectionStaticObjectMap.FindOrAdd(SectionID);
		ActorSet.Add(SpawnActor);
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
