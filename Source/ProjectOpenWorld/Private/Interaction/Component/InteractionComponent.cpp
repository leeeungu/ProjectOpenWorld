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
	if (!IsValid(InteractionTarget.GetObject()))
	{
		InteractionTarget = nullptr;
		bIsInteraction = false;
	}
	if (!PlayerController || !PlayerCharacter || bIsInteraction)
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
			//200.0f,
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
		IInteractionInterface::Execute_OnInteraction(InteractionTarget.GetObject(), PlayerController.Get());
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

void UInteractionComponent::OnActorCancel()
{
	if (InteractionTarget && InteractionTarget.GetObject())
	{
		IInteractionInterface::Execute_OnInteractionCanceled(InteractionTarget.GetObject(), PlayerController.Get());
		if (bIsInteraction)
			IInteractionInterface::Execute_OnInteractionEnd(InteractionTarget.GetObject(), PlayerController.Get());
		else
			IInteractionInterface::Execute_OnEndDetected(InteractionTarget.GetObject(), PlayerController.Get());
		//Cast<AActor>(InteractionTarget.GetObject())->Destroy();
		bIsInteraction = false;
		InteractionTarget = nullptr;
	}
}
AActor* UInteractionComponent::GetTargetActor() 
{ 
	if (!InteractionTarget || !InteractionTarget.GetObject()) 
		return nullptr; 
	return Cast<AActor>(InteractionTarget.GetObject()); 
}