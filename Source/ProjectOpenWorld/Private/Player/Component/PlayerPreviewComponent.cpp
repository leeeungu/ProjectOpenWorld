#include "Player/Component/PlayerPreviewComponent.h"
#include "Player/Character/BasePlayer.h"
#include "Player/Controller/BasePlayerController.h"
#include "Player/Character/PlayerPreviewPawn.h"
#include "Player/Subsystem/PlayerWorldSubsystem.h"
#include "Kismet/GameplayStatics.h"

UPlayerPreviewComponent::UPlayerPreviewComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerPreviewComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerPreviewComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindSourceDelegates();
	Super::EndPlay(EndPlayReason);
}

bool UPlayerPreviewComponent::OpenPreview()
{
	ABasePlayerController* OwnerController = GetOwnerController();
	if (!IsValid(OwnerController) || !OwnerController->IsLocalController())
	{
		return false;
	}
	if (!SetPreviewPawnIfNeeded())
	{
		return false;
	}
	RefreshPreview();
	return true;
}

void UPlayerPreviewComponent::ClosePreview()
{
	if (!PreviewPawn.IsValid())
	{
		return;
	}
	PreviewPawn->EndPreview();
	PreviewPawn = nullptr;
}

void UPlayerPreviewComponent::RefreshPreview()
{
	if (!PreviewPawn.IsValid() || !SourcePlayer.IsValid())
	{
		return;
	}
	PreviewPawn->InitializePreview(SourcePlayer.Get());
}

void UPlayerPreviewComponent::RotatePreview(float InYawDelta)
{
	if (!PreviewPawn.IsValid())
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
	SourcePlayer = InSourcePlayer;
}

APlayerPreviewPawn* UPlayerPreviewComponent::GetPreviewPawn() const
{
	if(PreviewPawn.IsValid())
		return PreviewPawn.Get();
	return nullptr;
}

ABasePlayer* UPlayerPreviewComponent::GetSourcePlayer() const
{
	return SourcePlayer.Get();
}

bool UPlayerPreviewComponent::SetPreviewPawnIfNeeded()
{
	if (PreviewPawn.IsValid())
	{
		return true;
	}
	if (UPlayerWorldSubsystem* PlayerWorldSubsystem = GetWorld()->GetSubsystem<UPlayerWorldSubsystem>())
	{
		PreviewPawn = PlayerWorldSubsystem->GetPreviewActor();
	}
	return PreviewPawn.IsValid();
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