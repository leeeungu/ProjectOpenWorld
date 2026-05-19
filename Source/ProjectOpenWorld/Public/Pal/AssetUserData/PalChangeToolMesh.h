#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "PalChangeToolMesh.generated.h"

class UStaticMesh;
class ACharacter;
enum class EPalJobType : uint8;


UCLASS(BlueprintType, EditInlineNew, CollapseCategories)
class PROJECTOPENWORLD_API UPalChangeToolMesh : public UAssetUserData
{
    GENERATED_BODY()
protected:
    UPROPERTY(EditAnywhere, Category = "ToolMesh")
    TObjectPtr<UStaticMesh> ToolMesh{};

    UPROPERTY(EditAnywhere, Category = "ToolMesh")
    FName ToolSocketName{};

    UPROPERTY(EditAnywhere, Category = "ToolMesh")
    FTransform ToolTransform{};
public:
    UFUNCTION(BlueprintPure, Category = "ToolMesh")
    UStaticMesh* GetToolMesh() const { return ToolMesh; }
    UFUNCTION(BlueprintPure, Category = "ToolMesh")
    FName GetToolSocketName() const { return ToolSocketName; }
    UFUNCTION(BlueprintPure, Category = "ToolMesh")
    FTransform GetToolSocketTransform() const{ return ToolTransform; }


#if WITH_EDITOR
    virtual void PostEditChangeOwner() override;
    virtual void PostLoad() override;

    /** Persona 프리뷰에 도구 메쉬 설치/제거 */
    void InstallPreviewMesh();
    void UninstallPreviewMesh();
#endif

private:
#if WITH_EDITORONLY_DATA
    UPROPERTY(Transient)
    TWeakObjectPtr<UStaticMeshComponent> PreviewMeshComp;
#endif
};