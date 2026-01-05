#include "Interaction/Component/PlayerInteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/InteractionInterface.h"

UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
}

void UPlayerInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!IsSetTarget())
	{
		InteractionTarget = nullptr;
		bIsInteraction = false;
	}
	else
		return;
	if (!OwnerCharacter || bIsInteraction)
		return;

	FHitResult HitResult{};
	if (CameraManager)
	{
		TArray<TEnumAsByte<EObjectTypeQuery> > buildPointObjectTypes{};
		TArray<AActor*> buildPointIgnore{};

		buildPointObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		buildPointObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
		buildPointObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));

		buildPointIgnore.Add(OwnerCharacter.Get());
		UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
			CameraManager->GetCameraLocation(),
			UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()) * DetectionDistance + CameraManager->GetCameraLocation(),
			//200.0f,
			buildPointObjectTypes, true, buildPointIgnore, EDrawDebugTrace::Type::None, HitResult, true);
		if (HitResult.GetActor())
		{
			if (IInteractionInterface* Interaction = Cast<IInteractionInterface>(HitResult.GetActor()))
			{
				if (Interaction != InteractionTarget)
				{
					if (InteractionTarget)
						IInteractionInterface::Execute_OnEndDetected(InteractionTarget.GetObject(), OwnerCharacter.Get());
					InteractionTarget = TScriptInterface< IInteractionInterface>(HitResult.GetActor());
					IInteractionInterface::Execute_OnBeginDetected(HitResult.GetActor(), OwnerCharacter.Get());
				}
			}
			else if (InteractionTarget)
			{
				IInteractionInterface::Execute_OnEndDetected(InteractionTarget.GetObject(), OwnerCharacter.Get());
				InteractionTarget = nullptr;
			}
		}
		else if (!HitResult.GetActor() && InteractionTarget)
		{
			IInteractionInterface::Execute_OnEndDetected(InteractionTarget.GetObject(), OwnerCharacter.Get());
			InteractionTarget = nullptr;
		}
	}
}

