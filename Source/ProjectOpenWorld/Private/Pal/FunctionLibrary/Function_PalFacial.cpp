#include "Pal/FunctionLibrary/Function_PalFacial.h"
#include "Components/SkeletalMeshComponent.h"

FVector UFunction_PalFacial::GetEyeOffset(EPalFacialEyeType EyeType)
{
	// Implement logic to return different eye offsets based on EyeType
	uint8 Value = static_cast<uint8>(EyeType);
	FVector Offset(0.0f, 0.0f, 0.0f);
	Offset.X = (Value / 4) * 0.5f; // Example calculation
	Offset.Y = (Value % 4) * 0.25f; // Example calculation
	return Offset;
}

FVector UFunction_PalFacial::GetMouthOffset(EPalFacialMouthType MouthType)
{
	uint8 Value = static_cast<uint8>(MouthType);
	FVector Offset(0.0f, 0.0f, 0.0f);
	Offset.X = (Value / 2) * 0.5f; // Example calculation
	Offset.Y = (Value % 2) * 0.5F; // Example calculation
	return Offset;
}

void UFunction_PalFacial::SetPalFacialEyeOffset(USkeletalMeshComponent* MeshComp, EPalFacialEyeType EyeType)
{
	if (MeshComp)
		MeshComp->SetVectorParameterValueOnMaterials(*GetFacialEyeParameterName(), GetEyeOffset(EyeType));
}

void UFunction_PalFacial::SetPalFacialMouthOffset(USkeletalMeshComponent* MeshComp, EPalFacialMouthType MouthType)
{
	if (MeshComp)
		MeshComp->SetVectorParameterValueOnMaterials(*GetFacialMouthParameterName(), GetMouthOffset(MouthType));
}
