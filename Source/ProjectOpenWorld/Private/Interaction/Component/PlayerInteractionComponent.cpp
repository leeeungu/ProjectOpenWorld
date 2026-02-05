#include "Interaction/Component/PlayerInteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/InteractionInterface.h"

UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bCanAiming = true;
}

void UPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
}

void UPlayerInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bCanAiming)
		return;
	if (!IsSetTarget())
	{
		OnDetectEnd();
		OnInteractionCompleted();
	}
	if (!OwnerCharacter || IsInteracting())
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
		UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(),
			CameraManager->GetCameraLocation(),
			UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()) * DetectionDistance + CameraManager->GetCameraLocation(),
			InteractionRadius,
			buildPointObjectTypes, true, buildPointIgnore, EDrawDebugTrace::Type::None, HitResult, true);
		if (HitResult.GetActor())
		{
			TScriptInterface< IInteractionInterface> Other = TScriptInterface< IInteractionInterface>(HitResult.GetActor());
			if (!Other)
				Other = TScriptInterface< IInteractionInterface>(HitResult.GetComponent());
			if (!CheckSameTarget(Other))
			{
				OnDetectEnd(); // 이전 타겟에 대한 감지 종료
				SetTarget(Other);
				OnDetectBegin(); // 새로운 타겟에 대한 감지 시작
			}
			else if (!Other)
			{
				OnDetectEnd();
			}
		}
		else if (!HitResult.GetActor() && IsSetTarget())
		{
			OnDetectEnd();
		}
	}
}

