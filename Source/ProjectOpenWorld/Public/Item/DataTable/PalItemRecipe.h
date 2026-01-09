// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalItemRecipe.generated.h"

//"Rows": {
//    "PalSphere": {
//        "Product_Id": "PalSphere",
//            "Product_Count" : 1,
//            "WorkAmount" : 300.0,
//            "WorkableAttribute" : 1,
//            "UnlockItemID" : "None",
//            "Material1_Id" : "Pal_crystal_S",
//            "Material1_Count" : 1,
//            "Material2_Id" : "Wood",
//            "Material2_Count" : 3,
//            "Material3_Id" : "Stone",
//            "Material3_Count" : 3,
//            "Material4_Id" : "None",
//            "Material4_Count" : 0,
//            "Material5_Id" : "None",
//            "Material5_Count" : 0,
//            "EnergyType" : "EPalEnergyType::None",
//            "EnergyAmount" : 0,
//            "CraftExpRate" : 1.0,
//            "Editor_RowNameHash" : 781717301
//    },

USTRUCT(BlueprintType)
struct FMaterialData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaterialData")
	FString Material_Id{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaterialData")
	int32 Material_Count = 0;
};

USTRUCT(BlueprintType)
struct FPalItemRecipe : public FTableRowBase
{
	GENERATED_BODY()
public:
	static int GetMaxMaterialCount() { return 5; }
	FPalItemRecipe();
	// Product Id
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemRecipe")
	FString Product_Id{};
	// Product Count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemRecipe")
	int32 Product_Count = 0;
	// Work Amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemRecipe")
	float WorkAmount = 0.0f;
	// Unlock Item ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemRecipe")
	FString UnlockItemID{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemRecipe")
	TArray< FMaterialData> Materials{};

	// Workable Attribute
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemRecipe")
	//int32 WorkableAttribute = 0;
	// Energy Type
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemRecipe")
	//FString EnergyType{};
	//// Energy Amount
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemRecipe")
	//int32 EnergyAmount = 0;
	// Craft Exp Rate
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemRecipe")
	//float CraftExpRate{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalItemRecipe")
	int64 Editor_RowNameHash{};
};