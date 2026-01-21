#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Function_PalFacial.generated.h"


UENUM(BlueprintType)
enum class EPalFacialEyeType : uint8
{
	Open,
	HalfClose,
	Close,
	A, // UMETA(Hidden),

	B, // UMETA(Hidden),
	Anger,
	Trouble,
	Pain
};

UENUM(BlueprintType)
enum class EPalFacialMouthType : uint8
{
	Default,
	Smile,
	Shock,
	Anger
};


UCLASS()
class PROJECTOPENWORLD_API UFunction_PalFacial : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "PalFacial")
	static FVector GetEyeOffset(EPalFacialEyeType EyeType);
	UFUNCTION(BlueprintPure, Category = "PalFacial")
	static FVector GetMouthOffset(EPalFacialMouthType MouthType);
	UFUNCTION(BlueprintCallable, Category = "PalFacial")
	static void SetPalFacialEyeOffset(USkeletalMeshComponent* MeshComp, EPalFacialEyeType EyeType);
	UFUNCTION(BlueprintCallable, Category = "PalFacial")
	static void SetPalFacialMouthOffset(USkeletalMeshComponent* MeshComp, EPalFacialMouthType MouthType);

	UFUNCTION(BlueprintPure, Category = "PalFacial")
	static FString GetFacialEyeParameterName() { return TEXT("EyeOffset"); }
	UFUNCTION(BlueprintPure, Category = "PalFacial")
	static FString GetFacialMouthParameterName() { return TEXT("MouseOffset"); }


};
