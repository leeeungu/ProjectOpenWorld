#pragma once

#include "CoreMinimal.h"
#include "GameBase/Animation/BaseAnimInstance.h"
#include "PalCreatureAnimInstance.generated.h"

class APalBaseCreature;
class UPalJobComponent;
enum class EPalJobType : uint8;
struct FPalWorkCommand;
class UStaticMesh;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnToolMeshChanged, UStaticMesh*, NewMesh,  FName, SocketName, FTransform, SocketTransform);

UCLASS()
class PROJECTOPENWORLD_API UPalCreatureAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "PalAnim")
	TObjectPtr <APalBaseCreature> OwnerPalCreature{};
	UPROPERTY(VisibleAnywhere, Category = "PalAnim")
	TObjectPtr<UPalJobComponent> JobComponent{};

	UPROPERTY(EditAnywhere, Category = "PalAnim")
	EPalJobType CurrenJobType{};
	UPROPERTY(EditAnywhere, Category = "PalAnim")
	bool bActionStarted{};

	UPROPERTY(EditAnywhere, Category = "PalAnim")
	TObjectPtr<UAnimSequence> ArchitectureAnim{};
	UPROPERTY(EditAnywhere, Category = "PalAnim")
	TObjectPtr<UAnimSequence> MiningAnim{};
	UPROPERTY(EditAnywhere, Category = "PalAnim")
	TObjectPtr<UAnimSequence> TransportAnim{};
	UPROPERTY(EditAnywhere, Category = "PalAnim")
	TObjectPtr<UAnimSequence> WorkAnimation{};

public:
	UPROPERTY(BlueprintAssignable)
	FOnToolMeshChanged OnToolMeshChanged{};
	UFUNCTION(BlueprintPure, Category = "Pal|Anim", meta = (BlueprintThreadSafe))
	FORCEINLINE UAnimSequence* GetWorkanimation() { return WorkAnimation; }

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "PalAnim", meta = (BlueprintThreadSafe))
	EPalJobType GetCurrentJobType() const { return CurrenJobType; }
	UFUNCTION(BlueprintPure, Category = "PalAnim", meta = (BlueprintThreadSafe))
	bool IsActionStarted() const { return bActionStarted; }

	UFUNCTION()
	void OnChangeWorkCommand(const FPalWorkCommand& Job);
	UFUNCTION()
	void OnStartWork();
	UFUNCTION()
	void OnEndWork();
private:
};
