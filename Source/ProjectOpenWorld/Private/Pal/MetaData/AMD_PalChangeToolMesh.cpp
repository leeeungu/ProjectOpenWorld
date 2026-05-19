#include "Pal/MetaData/AMD_PalChangeToolMesh.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/UnrealType.h"
#include "IAnimationEditor.h"
#include "IPersonaPreviewScene.h"
#include "IPersonaToolkit.h"
#include "Animation/DebugSkelMeshComponent.h"

#if WITH_EDITOR
void UAMD_PalChangeToolMesh::PostLoad()
{
	Super::PostLoad();
	if (UAnimSequence* OwnerSeq = Cast<UAnimSequence>(GetOuter()))
	{
		UpdatePreviewAttachment(OwnerSeq);
	}
}

void UAMD_PalChangeToolMesh::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UAnimSequence* Animinstance = Cast< UAnimSequence>(GetOuter());
	if (PropertyChangedEvent.HasArchetypeInstanceChanged(this) && Animinstance)
	{
		UpdatePreviewAttachment(Animinstance);
	}
}
#endif

void UAMD_PalChangeToolMesh::StartEvent(UAnimInstance* Animinstance)
{
	UStaticMeshComponent* ToolCom = GetToolComponnet(Animinstance);
	if (!ToolCom)
	{

		return;
	}
	ToolCom->SetStaticMesh(ToolMesh);
	ToolCom->AttachToComponent(Animinstance->GetSkelMeshComponent(), FAttachmentTransformRules::KeepRelativeTransform, ToolSocketName);
	ToolCom->SetRelativeTransform(ToolTransform);
}

void UAMD_PalChangeToolMesh::EndEvent(UAnimInstance* Animinstance)
{
	UStaticMeshComponent* ToolCom = GetToolComponnet(Animinstance);
	if (!ToolCom)
	{

		return;
	}
	ToolCom->SetStaticMesh(nullptr);
	ToolCom->SetRelativeTransform(FTransform{});
}

UStaticMeshComponent* UAMD_PalChangeToolMesh::GetToolComponnet(UAnimInstance* Animinstance)
{
	if (!Animinstance)
		return nullptr;
	APalBaseCreature* Owner =  Cast<APalBaseCreature>(	Animinstance->TryGetPawnOwner());
	if (!Owner)
		return nullptr;
	return Owner->GetPalToolMeshComponent();
}


#if WITH_EDITOR
void UAMD_PalChangeToolMesh::UpdatePreviewAttachment(UAnimSequence* Seq)
{
	UAssetEditorSubsystem* Sub = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	if (!Sub) return;

	TArray<IAssetEditorInstance*> Editors = Sub->FindEditorsForAsset(Seq);
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

			UDebugSkelMeshComponent* Mesh = PreviewScene->GetPreviewMeshComponent();
			if (PreviewMeshComp.IsValid())
			{
				PreviewMeshComp->DestroyComponent();
				PreviewMeshComp = nullptr;
			}
			UStaticMeshComponent* NewComp = NewObject<UStaticMeshComponent>(SkelMesh);
			if (NewComp)
			{
				NewComp->SetStaticMesh(ToolMesh);
				NewComp->SetupAttachment(SkelMesh, ToolSocketName);
				NewComp->RegisterComponent();
				NewComp->SetRelativeTransform(ToolTransform);
				PreviewMeshComp = NewComp;
			}

			// 여기서 자식 StaticMeshComponent 만들어 소켓에 어태치
			// 또는 기존 컴포넌트 찾아서 SetStaticMesh
		}
	}
}
#endif