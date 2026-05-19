#pragma once

#include "CoreMinimal.h"
#include "GameBase/MetaData/AMD_MontageChangeEvent.h"
#include "AMD_PalChangeToolMesh.generated.h"

class UStaticMesh;
class UStaticMeshComponent;

UCLASS()
class PROJECTOPENWORLD_API UAMD_PalChangeToolMesh : public UAMD_MontageChangeEvent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditInstanceOnly, Category = "ChangeEvent")
	TObjectPtr<UStaticMesh> ToolMesh{};
	UPROPERTY(EditInstanceOnly, Category = "ChangeEvent")
	FName ToolSocketName{};

	UPROPERTY(EditInstanceOnly, Category = "ChangeEvent")
	FTransform ToolTransform{};

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	TWeakObjectPtr<UStaticMeshComponent> PreviewMeshComp;
#endif
public:
#if WITH_EDITOR
	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	void UpdatePreviewAttachment(UAnimSequence* Seq);
#endif // WITH_EDITOR

	virtual void StartEvent(UAnimInstance* Animinstance) override;
	virtual void EndEvent(UAnimInstance* Animinstance) override;
protected:
	UStaticMeshComponent* GetToolComponnet(UAnimInstance* Animinstance);

};
