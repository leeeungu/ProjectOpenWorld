#include "Pal/AssetUserData/PalChangeToolMesh.h"
#include "Engine/StaticMesh.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

#if WITH_EDITOR
#include "Animation/AnimSequence.h"
#include "Editor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "IAnimationEditor.h"
#include "IPersonaToolkit.h"
#include "IPersonaPreviewScene.h"
#include "Animation/DebugSkelMeshComponent.h"
#endif

#if WITH_EDITOR

void UPalChangeToolMesh::PostEditChangeOwner()
{
    Super::PostEditChangeOwner();
    InstallPreviewMesh(); 
}

void UPalChangeToolMesh::PostLoad()
{
    Super::PostLoad();
    InstallPreviewMesh();
}

void UPalChangeToolMesh::InstallPreviewMesh()
{
    UAnimSequence* OwnerSeq = Cast<UAnimSequence>(GetOuter());
    if (!OwnerSeq || !GEditor)
        return;

    UAssetEditorSubsystem* Sub = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    if (!Sub) 
        return;

    TArray<IAssetEditorInstance*> Editors = Sub->FindEditorsForAsset(OwnerSeq);
    if (Editors.Num() == 0) 
        return; // 에디터 안 열려 있으면 스킵

    for (IAssetEditorInstance* Editor : Editors)
    {
        // Persona 에디터는 IAnimationEditor 또는 IAnimAssetEditor 캐스팅
        // (정확한 클래스는 5.x 버전마다 약간 다름 — 헤더 확인 필요)
        if (auto* AnimEditor = static_cast<IAnimationEditor*>(Editor))
        {
            TSharedRef<IPersonaPreviewScene> PreviewScene =
                AnimEditor->GetPersonaToolkit()->GetPreviewScene();

            UDebugSkelMeshComponent* SkelMesh = PreviewScene->GetPreviewMeshComponent();
            if (!SkelMesh)
                continue;

            UninstallPreviewMesh(); // 기존 거 정리

            UStaticMeshComponent* NewComp = NewObject<UStaticMeshComponent>(SkelMesh);
            NewComp->SetStaticMesh(GetToolMesh());
            NewComp->SetupAttachment(SkelMesh, GetToolSocketName());
            NewComp->RegisterComponent();
            NewComp->SetRelativeTransform(GetToolSocketTransform());

            PreviewMeshComp = NewComp;
            return;
        }
    }
}

void UPalChangeToolMesh::UninstallPreviewMesh()
{
    if (PreviewMeshComp.IsValid())
    {
        PreviewMeshComp->DestroyComponent();
        PreviewMeshComp = nullptr;
    }
}

#endif  // WITH_EDITOR