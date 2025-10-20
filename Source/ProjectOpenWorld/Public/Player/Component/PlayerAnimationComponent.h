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
		FName Root = TEXT("pelvis"); 
		FName LHand = TEXT("hand_l");
		FName RHand = TEXT("hand_r");
		FName LFoot= TEXT("foot_l");
		FName RFoot= TEXT("foot_r");
		FName Head = TEXT("head");

		SClimbRayData()
		{
			arrSocekt[0] = &Root;
			arrSocekt[1] = &LHand;
			arrSocekt[2] = &RHand;
			arrSocekt[3] = &LFoot;
			arrSocekt[4] = &RFoot;
			arrSocekt[5] = &Head;
		}
		FHitResult arrHitResult[6]{};
		FName* arrSocekt[6]{};
		int ArraySize = 6;
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

	const FHitResult*  GetPelvisHit() const { return &ClimbData.arrHitResult[0];}
	FVector  GetAVGPosition() const { return ClimbData.AvgPosition;}
protected:
	virtual void BeginPlay() override;

	bool ClimbTick(float DeltaTime);

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
