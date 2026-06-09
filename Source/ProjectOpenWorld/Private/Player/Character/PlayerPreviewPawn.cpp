#include "Player/Character/PlayerPreviewPawn.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/Pawn.h"
#include "Player/Character/BasePlayer.h"
#include "Player/Subsystem/PlayerWorldSubsystem.h"

APlayerPreviewPawn::APlayerPreviewPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::Disabled;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
	PreviewMesh->SetupAttachment(Root);
	PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewMesh->SetGenerateOverlapEvents(false);
	PreviewMesh->SetCastShadow(false);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Root);
	SpringArm->TargetArmLength = 150.f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	if (SceneCapture)
	{
		SceneCapture->SetupAttachment(SpringArm);
		SceneCapture->bCaptureEveryFrame = false;
		SceneCapture->bCaptureOnMovement = false;
		SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
		SceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;
		SceneCapture->OrthoWidth = 210.0f;
	}
}

void APlayerPreviewPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (SceneCapture)
	{
		SceneCapture->ShowOnlyComponents.Empty();
		SceneCapture->ShowOnlyComponents.Add(PreviewMesh);
		TArray<USceneComponent*> ChildComponents;
		PreviewMesh->GetChildrenComponents(true, ChildComponents);
		for (USceneComponent* ChildComponent : ChildComponents)
		{
			if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ChildComponent))
				SceneCapture->ShowOnlyComponents.Add(PrimitiveComponent);
		}
		SceneCapture->CaptureScene();
	}
}

void APlayerPreviewPawn::BeginPlay()
{
	Super::BeginPlay();
	if (UPlayerWorldSubsystem* PlayerWorldSubsystem = GetWorld()->GetSubsystem<UPlayerWorldSubsystem>())
	{
		PlayerWorldSubsystem->RegisterPreviewActor(this);
	}
}

void APlayerPreviewPawn::InitializePreview(ABasePlayer* InSourcePlayer)
{
	SourcePlayer = InSourcePlayer;
}

void APlayerPreviewPawn::StartPreview()
{
	if (!SourcePlayer.IsValid())
		return;

	SetActorHiddenInGame(false);
	RefreshFromSource();
	if(SceneCapture)
		SceneCapture->bCaptureEveryFrame = true;
}

void APlayerPreviewPawn::EndPreview()
{
	SetActorHiddenInGame(true);
	if(SceneCapture)
		SceneCapture->bCaptureEveryFrame = false;
	SourcePlayer = nullptr;
}

void APlayerPreviewPawn::RefreshFromSource()
{
	if (!SourcePlayer.IsValid() || !SceneCapture || !PreviewMesh)
	{
		return;
	}

	ABasePlayer* Player = SourcePlayer.Get();
	if (!Player)
	{
		return;
	}

	USkeletalMeshComponent* SourceMesh = Player->GetMesh();
	if (!SourceMesh)
	{
		return;
	}
	// Mesh와 하위 매쉬 설정
	PreviewMesh->SetSkeletalMesh(SourceMesh->GetSkeletalMeshAsset());
	PreviewMesh->SetAnimInstanceClass(SourceMesh->GetAnimClass());

	const int32 MaterialCount = SourceMesh->GetNumMaterials();
	for (int32 Index = 0; Index < MaterialCount; ++Index)
	{
		PreviewMesh->SetMaterial(Index, SourceMesh->GetMaterial(Index));
	}

	SceneCapture->CaptureScene();
}

void APlayerPreviewPawn::RotatePreview(float InYawDelta)
{
	AddActorLocalRotation(FRotator(0.f, InYawDelta, 0.f));
	if(SceneCapture)
		SceneCapture->CaptureScene();
}