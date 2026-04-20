#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPreviewPawn.generated.h"

class ABasePlayer;
class USceneComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;

UCLASS()
class PROJECTOPENWORLD_API APlayerPreviewPawn : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Preview")
	TObjectPtr<USceneComponent> Root{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Preview")
	TObjectPtr<USkeletalMeshComponent> PreviewMesh{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Preview")
	TObjectPtr<USpringArmComponent> SpringArm{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Preview")
	TObjectPtr<USceneCaptureComponent2D> SceneCapture{};

	UPROPERTY()
	TWeakObjectPtr<ABasePlayer> SourcePlayer{};
public:
	APlayerPreviewPawn();

	virtual void OnConstruction(const FTransform& Transform) override;

	void InitializePreview(ABasePlayer* InSourcePlayer);
	void StartPreview();
	void EndPreview();
	void RefreshFromSource();
	void RotatePreview(float InYawDelta);

	FORCEINLINE USceneCaptureComponent2D* GetSceneCapture() const { return SceneCapture; }
	FORCEINLINE USkeletalMeshComponent* GetPreviewMesh() const { return PreviewMesh; }

protected:
	virtual void BeginPlay() override;
};