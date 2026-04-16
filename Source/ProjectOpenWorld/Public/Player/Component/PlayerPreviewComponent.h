#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerPreviewComponent.generated.h"

class ABasePlayer;
class ABasePlayerController;
class APlayerPreviewPawn;
//class APlayerPreviewAnchor;
class UTextureRenderTarget2D;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UPlayerPreviewComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview")
	TSubclassOf<APlayerPreviewPawn> PreviewPawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	TObjectPtr<UTextureRenderTarget2D> PreviewRenderTarget;

	//UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Preview")
	//TObjectPtr<APlayerPreviewAnchor> PreviewAnchor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	FName PreviewAnchorTag = TEXT("PlayerPreview");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	bool bDestroyPreviewPawnOnClose = false;

	UPROPERTY(Transient)
	TObjectPtr<APlayerPreviewPawn> PreviewPawn;

	UPROPERTY(Transient)
	TWeakObjectPtr<ABasePlayer> SourcePlayer;

public:
	UPlayerPreviewComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Preview")
	bool OpenPreview(ABasePlayer* InSourcePlayer = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void ClosePreview();

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void RefreshPreview();

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void RotatePreview(float InYawDelta);

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void SetSourcePlayer(ABasePlayer* InSourcePlayer);

	UFUNCTION(BlueprintPure, Category = "Preview")
	APlayerPreviewPawn* GetPreviewPawn() const { return PreviewPawn; }

	UFUNCTION(BlueprintPure, Category = "Preview")
	UTextureRenderTarget2D* GetPreviewRenderTarget() const { return PreviewRenderTarget; }

	UFUNCTION(BlueprintPure, Category = "Preview")
	ABasePlayer* GetSourcePlayer() const { return SourcePlayer.Get(); }

protected:
	bool ResolveSourcePlayer();
	bool ResolvePreviewAnchor();
	bool SpawnPreviewPawnIfNeeded();
	void DestroyPreviewPawn();

	ABasePlayerController* GetOwnerController() const;

	void BindSourceDelegates();
	void UnbindSourceDelegates();

	UFUNCTION()
	void HandleSourceAppearanceChanged();
};