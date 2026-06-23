#include "Building/BaseBuilding.h"
#include "Components/StaticMeshComponent.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Component/PalBuildingStaticMeshComponent.h"
#include "Building/Component/BuildingActionWidgetComponent.h"
#include "Pal/Component/ActionWidget/PalActionWidgetComponent.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "NavAreas/NavArea_Default.h"
#include "NavAreas/NavArea_Null.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"
#include "NavigationSystem.h"

ABaseBuilding::ABaseBuilding() :Super()
{
	PrimaryActorTick.bCanEverTick = false;
	//mobility(EComponentMobility::Static);
	PalBuildingStaticMeshComponent = CreateDefaultSubobject<UPalBuildingStaticMeshComponent>(TEXT("PalBuildingStaticMeshComponent"));

	SetRootComponent(PalBuildingStaticMeshComponent);
	PalBuildingStaticMeshComponent->SetMobility(EComponentMobility::Static);
	PalBuildingStaticMeshComponent->SetCanEverAffectNavigation(false);
	PalBuildingStaticMeshComponent->SetCollisionProfileName(TEXT("P_Building"));
	PalBuildingStaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	PalBuildingStaticMeshComponent->SetGenerateOverlapEvents(true);
	
	//buildingProgressComponent = CreateDefaultSubobject<UBuildingProgress>(TEXT("BuildingProgress"));
	BuildActionWidgetCom = CreateDefaultSubobject<UPalActionWidgetComponent>(TEXT("PalActionWidgetComponent"));
	//if (BuildActionWidget)
	//{

	//	BuildActionWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//	BuildActionWidget->SetCollisionProfileName(TEXT("NoCollision"));
	//}

	
	NavModifier = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifier"));
	NavModifier->CalculateBounds();
	NavModifier->SetAreaClass(UNavArea_Default::StaticClass());
	//Command = UPalCommandFunctionLibrary::CommandArchitecture(nullptr, this);
}

void ABaseBuilding::BeginPlay()
{
	Super::BeginPlay();

	NavModifier->CalculateBounds();
	const FVector Bottom = PalBuildingStaticMeshComponent->GetSocketLocation(TEXT("Bottom"));
	bool bHitGround = false;
	if (const UWorld* World = GetWorld())
	{
		const FVector Start{ Bottom.X, Bottom.Y, Bottom.Z + 100000.f };
		const FVector End{ Bottom.X, Bottom.Y, Bottom.Z - 100000.f };
	
		FCollisionQueryParams Params{ SCENE_QUERY_STAT(BuildingWorkGround), false, this };
		Params.AddIgnoredActor(this);   // 자기 자신(빌딩) 무시
	
		FHitResult Hit{};
		if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		{
			WorkLocation = Hit.ImpactPoint;
			bHitGround = true;
			FVector GroundPoint = WorkLocation;   // 라인트레이스로 구한 지형 표면(폴백)

			if (const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
			{
				FNavLocation NavLoc;
				// XY를 넉넉히: 빌딩이 네비를 파먹은 경우 가장자리(작업 가능 위치)로 스냅되도록
				const FVector Extent{ 600.f, 600.f, 400.f };
				if (NavSys->ProjectPointToNavigation(GroundPoint, NavLoc, Extent))
				{
					WorkLocation = NavLoc.Location;
//#if ENABLE_DRAW_DEBUG
//					DrawDebugSphere(World, WorkLocation, 20.f, 12, FColor::Cyan, false, 5.f, 0, 2.f); // 실제 네비 점
//#endif
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("GetWorkCommand :: 네비 투영 실패 - 해당 위치에 네비 타일 없음"));
				}
			}
		}
	
//	#if ENABLE_DRAW_DEBUG
//		// 트레이스 경로
//		DrawDebugLine(World, Start, End, FColor::Silver, false, 5.f, 0, 1.f);
//		// 작업 위치(지형 적중=초록 / 폴백=빨강)
//		DrawDebugSphere(World, WorkLocation, 30.f, 12,
//			bHitGround ? FColor::Green : FColor::Magenta, false, 5.f, 0, 2.f);
//	#endif
	}
}

UStaticMeshComponent* ABaseBuilding::GetBuildingMeshComponent() const
{
	return PalBuildingStaticMeshComponent;
}

void ABaseBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBuilding::OnConstruction(const FTransform& Transform)
{
	AActor::OnConstruction(Transform);
}

void ABaseBuilding::BeginDestroy()
{
	Super::BeginDestroy();
}

/*EPalCommandKind ABaseBuilding::GetCommandKind_Implementation()
{
	return Command.CommandKind;
}

uint8 ABaseBuilding::GetSubCommandType_Implementation()
{
	return Command.SubCommandType;
}

FPalCommand ABaseBuilding::GetCommand_Implementation()
{
	return Command;
}

bool ABaseBuilding::IsCommandFinished_Implementation()
{
	if (!PalBuildingStaticMeshComponent)
		return true;
	return PalBuildingStaticMeshComponent->IsBuildingEnd();
}
*/

void ABaseBuilding::NewGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	if (PalBuildingStaticMeshComponent->IsBuildingEnd())
		UpdateModifier();
}

void ABaseBuilding::DelGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	NoCollision();
	if (!PalBuildingStaticMeshComponent->IsBuildingEnd())
		PalBuildingStaticMeshComponent->StopAll();
}

void ABaseBuilding::UpdateModifier()
{
	if (!NavModifier)
		return;

///	UE_LOG(LogTemp, Warning, TEXT("ABaseBuilding::UpdateModifier"));
	PalBuildingStaticMeshComponent->SetCollisionProfileName(TEXT("P_Building"));
	PalBuildingStaticMeshComponent->SetMobility(EComponentMobility::Static);
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	NavModifier->SetAreaClass(UNavArea_Default::StaticClass());
	NavModifier->CalculateBounds();
	//if (Nav)
	//	Nav->AddDirtyArea(NavModifier->GetNavigationBounds(), 0);
	PalBuildingStaticMeshComponent->SetCanEverAffectNavigation(true);
}

void ABaseBuilding::NoCollision()
{
	//BuildActionWidget->SetCollisionProfileName(TEXT("NoCollision"));
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	NavModifier->SetAreaClass(UNavArea_Null::StaticClass());
	NavModifier->CalculateBounds();
	//if (Nav)
	//	Nav->AddDirtyArea(NavModifier->GetNavigationBounds(), 0);
	PalBuildingStaticMeshComponent->SetCanEverAffectNavigation(false);
}

void ABaseBuilding::ResiterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)
{
	if (PalBuildingStaticMeshComponent)
	{
		PalBuildingStaticMeshComponent->StartBuilding_V2(WorkerClass);
	}
}

void ABaseBuilding::UnregisterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)
{
	if (PalBuildingStaticMeshComponent)
	{
		PalBuildingStaticMeshComponent->StopBuilding_V2(WorkerClass);
	}
}

bool ABaseBuilding::IsWorkable() const
{	
	return !PalBuildingStaticMeshComponent->IsBuildingEnd();
}

FPalWorkCommand ABaseBuilding::GetWorkCommand(AActor* InstigatorActor, AActor* Target) const
{
	
	return UPalCommandFunctionLibrary::WorkArchitecture(InstigatorActor, Target, WorkLocation);
}
