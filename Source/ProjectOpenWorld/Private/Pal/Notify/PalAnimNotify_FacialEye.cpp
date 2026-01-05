#include "Pal/Notify/PalAnimNotify_FacialEye.h"

FVector UPalAnimNotify_FacialEye::GetEyeOffset() const
{
	// Implement logic to return different eye offsets based on EyeType
	uint8 Value = static_cast<uint8>(EyeType);
	FVector Offset(0.0f, 0.0f, 0.0f);
	Offset.X = (Value / 4) * 0.5f; // Example calculation
	Offset.Y = (Value % 4) * 0.25f; // Example calculation
	return Offset;
}

void UPalAnimNotify_FacialEye::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// Implement facial eye notify logic here
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp)
		MeshComp->SetVectorParameterValueOnMaterials(TEXT("EyeOffset"), GetEyeOffset());
}
