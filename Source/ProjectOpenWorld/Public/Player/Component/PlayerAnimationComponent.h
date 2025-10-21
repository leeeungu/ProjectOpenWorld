#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAnimationComponent.generated.h"

class ACharacter;

UENUM(BlueprintType)
enum class EAnimationState : uint8
{
	Travel,
	Climb
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UPlayerAnimationComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	struct SClimbRayData
	{
		enum ESocketName
		{
			ERoot,
			ELHand,
			ERHand,
			ELFoot,
			ERFoot,
			EHead,
			ELHNext,
			ELHUp,
			ERHNext,
			ERHUp,
			ELFNext,
			ELFDown,
			ERFNext,
			ERFDown,
		};

		FName Root = TEXT("pelvis"); 
		FName LHand = TEXT("hand_l");
		FName RHand = TEXT("hand_r");
		FName LFoot= TEXT("foot_l");
		FName RFoot= TEXT("foot_r");
		FName Head = TEXT("Head");
		FName LHNext = TEXT("LHClimbNext");
		FName LHUp	 = TEXT("LHClimbUp");
		FName RHNext = TEXT("RHClimbNext");
		FName RHUp	 = TEXT("RHClimbUp");
		FName LFNext = TEXT("LFClimbNext");
		FName LFDown = TEXT("LFClimbDown");
		FName RFNext = TEXT("RFClimbNext");
		FName RFDown = TEXT("RFClimbDown");

		SClimbRayData()
		{
			arrSocekt[0] = nullptr;
			arrSocekt[1] = &LHand;
			arrSocekt[2] = &RHand;
			arrSocekt[3] = &LFoot;
			arrSocekt[4] = &RFoot;
			arrSocekt[5] = &Head;
			arrSocekt[6] = &LHNext;
			arrSocekt[7] = &LHUp;
			arrSocekt[8] = &RHNext;
			arrSocekt[9] = &RHUp;
			arrSocekt[10] = &LFNext;
			arrSocekt[11] = &LFDown;
			arrSocekt[12] = &RFNext;
			arrSocekt[13] = &RFDown;
		}

		FHitResult arrHitResult[14]{};
		FName* arrSocekt[14]{};
		int ArraySize = 14;
		float StartOffset = 40;
		float EndOffset = 150;
		FVector AvgNormal{};
		FVector AvgPosition{};
	};

	EAnimationState AnimationState{};
	TObjectPtr<ACharacter> OwnerCharacter{};

	SClimbRayData ClimbData{};
	bool bCanClimb{ false };
	TArray<AActor*> IgnoreArray{};
public:
	UPlayerAnimationComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "Animation")
	bool IsClimbing() const { return EAnimationState::Climb == AnimationState; }
	UFUNCTION(BlueprintPure, Category = "Animation")
	EAnimationState GetAnimationState() const { return AnimationState; }

	UFUNCTION(BlueprintPure, Category = "Animation")
	bool CanClimb() const { return bCanClimb; }
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool StartClimb();
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool StartTravel();

	//void MoveClimb(const FInputActionValue& Value);

	FVector GetLeftNoraml() const;
	FVector GetCenterNoraml() const;
	FVector GetRightNoraml() const;
	bool CanLeftHandClimb() const { return ClimbData.arrHitResult[SClimbRayData::ELHand].bBlockingHit; }
	bool CanRightHandClimb() const { return ClimbData.arrHitResult[SClimbRayData::ERHand].bBlockingHit; }
	bool CanLeftFootClimb() const { return ClimbData.arrHitResult[SClimbRayData::ELFoot].bBlockingHit; }
	bool CanRightFootClimb() const { return ClimbData.arrHitResult[SClimbRayData::ERFoot].bBlockingHit; }
	const FHitResult*  GetPelvisHit() const { return &ClimbData.arrHitResult[0];}
	FVector  GetAVGPosition() const;
	bool ClimbLineCheck();
protected:
	virtual void BeginPlay() override;


};

//////////////////////////////////////////////////////// v1 //////////////////////////////////////////////

//#pragma once
//
//#include "CoreMinimal.h"
//#include "Components/ActorComponent.h"
//#include "PlayerAnimationComponent.generated.h"
//
//class ACharacter;
//
//UENUM(BlueprintType)
//enum class EAnimationState : uint8
//{
//	Travel,
//	Climb
//};
//
//
//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
//class PROJECTOPENWORLD_API UPlayerAnimationComponent : public UActorComponent
//{
//	GENERATED_BODY()
//protected:
//	EAnimationState AnimationState{};
//	TObjectPtr<ACharacter> OwnerCharacter{};
//
//	FHitResult ClimbHeadHit{};
//	FHitResult ClimbForwardHit{};
//	FHitResult ClimbDownHit{};
//	FHitResult ClimbUpHit{};
//	bool bHeadHit{ false };
//	bool bCanClimb{ false };
//	bool bCanClimbDown{ false };
//	bool bCanClimbUp{ false };
//	TArray<AActor*> IgnoreArray{};
//public:	
//	UPlayerAnimationComponent();
//
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//
//	UFUNCTION(BlueprintPure, Category = "Animation")
//	bool IsClimbing() const { return EAnimationState::Climb == AnimationState; }
//	UFUNCTION(BlueprintPure, Category = "Animation")
//	EAnimationState GetAnimationState() const { return AnimationState; }
//
//	UFUNCTION(BlueprintPure, Category = "Animation")
//	bool CanClimb() const { return bCanClimb; }
//	UFUNCTION(BlueprintCallable, Category = "Animation")
//	bool StartClimb();
//	UFUNCTION(BlueprintCallable, Category = "Animation")
//	bool StartTravel();
//
//	//void MoveClimb(const FInputActionValue& Value);
//
//protected:
//	virtual void BeginPlay() override;
//
//	bool ClimbTick(float DeltaTime);
//
//};
