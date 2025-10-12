#include "Interaction/Component/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/InteractionInterface.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!PlayerController || bIsInteraction || !PlayerCharacter)
		return;
	FHitResult HitResult{};
	if (CameraManager)
	{
		TArray<TEnumAsByte<EObjectTypeQuery> > buildPointObjectTypes{};
		TArray<AActor*> buildPointIgnore{};

		buildPointObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		buildPointObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

		buildPointIgnore.Add(PlayerCharacter.Get());
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
			CameraManager->GetCameraLocation(),
			UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()) * DetectionDistance + CameraManager->GetCameraLocation(),
			buildPointObjectTypes, true, buildPointIgnore, EDrawDebugTrace::Type::None, HitResult, true))
		{
			if (IInteractionInterface* Interaction = Cast<IInteractionInterface>(HitResult.GetActor()))
			{
				if (Interaction != InteractionTarget)
				{
					if (InteractionTarget)
						IInteractionInterface::Execute_OnEndDetected(InteractionTarget.GetObject(), PlayerController.Get());
						InteractionTarget = TScriptInterface< IInteractionInterface>(HitResult.GetActor());
						IInteractionInterface::Execute_OnBeginDetected(HitResult.GetActor(), PlayerController.Get());
				}
			}
			else if (InteractionTarget)
			{
				IInteractionInterface::Execute_OnEndDetected(InteractionTarget.GetObject(), PlayerController.Get());
				InteractionTarget = nullptr;
			}
		}
		else if (InteractionTarget)
		{
			IInteractionInterface::Execute_OnEndDetected(InteractionTarget.GetObject(), PlayerController.Get());
			InteractionTarget = nullptr;
		}
	}
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast< ACharacter>(GetOwner());
	if (PlayerCharacter)
		PlayerController = PlayerCharacter->GetController();
	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
}

void UInteractionComponent::OnInteractionStart()
{
	if (InteractionTarget)
	{
		bIsInteraction = true;
		IInteractionInterface::Execute_OnInteractionStart(InteractionTarget.GetObject(), PlayerController.Get());
	}
}

void UInteractionComponent::OnInteractionTriggered()
{
	if (InteractionTarget && bIsInteraction)
	{
		IInteractionInterface::Execute_OnInteraction(InteractionTarget.GetObject());
	}
}

void UInteractionComponent::OnInteractionCompleted()
{
	if (InteractionTarget)
	{
		bIsInteraction = false;
		IInteractionInterface::Execute_OnInteractionEnd(InteractionTarget.GetObject(), PlayerController.Get());
	}
}