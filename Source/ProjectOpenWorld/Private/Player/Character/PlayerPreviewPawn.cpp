#include "Player/Character/PlayerPreviewPawn.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/Pawn.h"
#include "Player/Character/BasePlayer.h"

APlayerPreviewPawn::APlayerPreviewPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
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
	SceneCapture->SetupAttachment(SpringArm);
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
}

void APlayerPreviewPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (SceneCapture)
	{
		SceneCapture->ShowOnlyActors.Empty();
		SceneCapture->ShowOnlyActors.Add(this);
		SceneCapture->CaptureScene();
	}

}

void APlayerPreviewPawn::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerPreviewPawn::InitializePreview(ABasePlayer* InSourcePlayer, UTextureRenderTarget2D* InRenderTarget)
{
	SourcePlayer = InSourcePlayer;
	if (SceneCapture)
	{
		SceneCapture->TextureTarget = InRenderTarget;
	}
	RefreshFromSource();
}

void APlayerPreviewPawn::RefreshFromSource()
{
	if (!SourcePlayer.IsValid())
	{
		return;
	}

	ABasePlayer* Player = SourcePlayer.Get();
	if (!Player)
	{
		return;
	}

	USkeletalMeshComponent* SourceMesh = Player->FindComponentByClass<USkeletalMeshComponent>();
	if (!SourceMesh)
	{
		return;
	}

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
	SceneCapture->CaptureScene();
}