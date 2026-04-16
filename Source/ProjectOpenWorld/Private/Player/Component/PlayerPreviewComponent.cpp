#include "Player/Component/PlayerPreviewComponent.h"
#include "Player/Character/BasePlayer.h"
#include "Player/Controller/BasePlayerController.h"
#include "Player/Character/PlayerPreviewPawn.h"
//#include "Player/Preview/PlayerPreviewAnchor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UPlayerPreviewComponent::UPlayerPreviewComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerPreviewComponent::BeginPlay()
{
	Super::BeginPlay();

	ResolveSourcePlayer();
	ResolvePreviewAnchor();
}

void UPlayerPreviewComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindSourceDelegates();
	DestroyPreviewPawn();

	Super::EndPlay(EndPlayReason);
}

bool UPlayerPreviewComponent::OpenPreview(ABasePlayer* InSourcePlayer)
{
	ABasePlayerController* OwnerController = GetOwnerController();
	if (!IsValid(OwnerController) || !OwnerController->IsLocalController())
	{
		return false;
	}

	if (IsValid(InSourcePlayer))
	{
		SetSourcePlayer(InSourcePlayer);
	}
	else if (!ResolveSourcePlayer())
	{
		return false;
	}

	if (!SpawnPreviewPawnIfNeeded())
	{
		return false;
	}

	if (!IsValid(PreviewPawn) || !SourcePlayer.IsValid())
	{
		return false;
	}

	PreviewPawn->SetActorHiddenInGame(false);
	PreviewPawn->SetActorEnableCollision(false);

	PreviewPawn->InitializePreview(SourcePlayer.Get(), PreviewRenderTarget);

	return true;
}

void UPlayerPreviewComponent::ClosePreview()
{
	if (!IsValid(PreviewPawn))
	{
		return;
	}

	if (bDestroyPreviewPawnOnClose)
	{
		DestroyPreviewPawn();
		return;
	}

	PreviewPawn->SetActorHiddenInGame(true);
}

void UPlayerPreviewComponent::RefreshPreview()
{
	if (!IsValid(PreviewPawn) || !SourcePlayer.IsValid())
	{
		return;
	}

	PreviewPawn->InitializePreview(SourcePlayer.Get(), PreviewRenderTarget);
}

void UPlayerPreviewComponent::RotatePreview(float InYawDelta)
{
	if (!IsValid(PreviewPawn))
	{
		return;
	}

	PreviewPawn->RotatePreview(InYawDelta);
}

void UPlayerPreviewComponent::SetSourcePlayer(ABasePlayer* InSourcePlayer)
{
	if (SourcePlayer.Get() == InSourcePlayer)
	{
		return;
	}

	UnbindSourceDelegates();
	SourcePlayer = InSourcePlayer;
	BindSourceDelegates();

	if (IsValid(PreviewPawn) && SourcePlayer.IsValid())
	{
		PreviewPawn->InitializePreview(SourcePlayer.Get(), PreviewRenderTarget);
	}
}

bool UPlayerPreviewComponent::ResolveSourcePlayer()
{
	if (SourcePlayer.IsValid())
	{
		return true;
	}

	ABasePlayerController* OwnerController = GetOwnerController();
	if (IsValid(OwnerController))
	{
		if (ABasePlayer* Player = Cast<ABasePlayer>(OwnerController->GetPawn()))
		{
			SourcePlayer = Player;
			return true;
		}
	}

	return false;
}

bool UPlayerPreviewComponent::ResolvePreviewAnchor()
{
	/*if (IsValid(PreviewAnchor))
	{
		return true;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	for (TActorIterator<APlayerPreviewAnchor> It(World); It; ++It)
	{
		APlayerPreviewAnchor* FoundAnchor = *It;
		if (!IsValid(FoundAnchor))
		{
			continue;
		}

		if (PreviewAnchorTag.IsNone() || FoundAnchor->ActorHasTag(PreviewAnchorTag))
		{
			PreviewAnchor = FoundAnchor;
			return true;
		}
	}*/

	return false;
}

bool UPlayerPreviewComponent::SpawnPreviewPawnIfNeeded()
{
	if (IsValid(PreviewPawn))
	{
		return true;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World) || !PreviewPawnClass)
	{
		return false;
	}

	FTransform SpawnTransform = FTransform::Identity;

	//if (ResolvePreviewAnchor() && IsValid(PreviewAnchor))
	//{
	//	SpawnTransform = PreviewAnchor->GetActorTransform();
	//}
	//else if (SourcePlayer.IsValid())
	//{
	//	SpawnTransform = SourcePlayer->GetActorTransform();
	//	SpawnTransform.AddToTranslation(FVector(0.f, 0.f, -10000.f));
	//}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PreviewPawn = World->SpawnActor<APlayerPreviewPawn>(
		PreviewPawnClass,
		SpawnTransform,
		SpawnParams
	);

	return IsValid(PreviewPawn);
}

void UPlayerPreviewComponent::DestroyPreviewPawn()
{
	if (!IsValid(PreviewPawn))
	{
		return;
	}

	PreviewPawn->Destroy();
	PreviewPawn = nullptr;
}

ABasePlayerController* UPlayerPreviewComponent::GetOwnerController() const
{
	return Cast<ABasePlayerController>(GetOwner());
}

void UPlayerPreviewComponent::BindSourceDelegates()
{
	if (!SourcePlayer.IsValid())
	{
		return;
	}

	// TODO:
	// 네 프로젝트의 외형/장비 변경 델리게이트에 여기서 바인딩해라.
	//
	// 예시:
	// SourcePlayer->OnAppearanceChanged.AddDynamic(this, &UPlayerPreviewComponent::HandleSourceAppearanceChanged);
	// SourcePlayer->GetEquipmentComponent()->OnEquipmentChanged.AddUObject(this, &UPlayerPreviewComponent::HandleSourceAppearanceChanged);
}

void UPlayerPreviewComponent::UnbindSourceDelegates()
{
	if (!SourcePlayer.IsValid())
	{
		return;
	}

	// TODO:
	// BindSourceDelegates()에서 묶은 델리게이트를 여기서 해제해라.
	//
	// 예시:
	// SourcePlayer->OnAppearanceChanged.RemoveDynamic(this, &UPlayerPreviewComponent::HandleSourceAppearanceChanged);
	// SourcePlayer->GetEquipmentComponent()->OnEquipmentChanged.RemoveAll(this);
}

void UPlayerPreviewComponent::HandleSourceAppearanceChanged()
{
	RefreshPreview();
}