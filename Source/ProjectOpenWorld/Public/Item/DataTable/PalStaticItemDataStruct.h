#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalStaticItemDataStruct.generated.h"
//
//"Money": {
//    "OverrideName": "None",
//        "OverrideDescription" : "None",
//        "IconName" : "Money",
//        "TypeA" : "EPalItemTypeA::Material",
//        "TypeB" : "EPalItemTypeB::Money",
//        "Rank" : 1,
//        "Rarity" : 0,
//        "MaxStackCount" : 99999999,
//        "Weight" : 0.0,
//        "Price" : 1,
//        "SortID" : 0,
//        "bInTreasureBox" : false,
//        "bNotConsumed" : false,
//        "bEnableHandcraft" : false,
//        "bLegalInGame" : true,
//        "TechnologyTreeLock" : 0,
//        "ItemStaticClass" : "None",
//        "ItemDynamicClass" : "None",
//        "ItemActorClass" : "None",
//        "ItemStaticMeshName" : "None",
//        "VisualBlueprintClassName" : "BP_Item_CoinGold",
//        "VisualBlueprintClassSoft" : {
//        "AssetPathName": "/Game/Pal/Blueprint/Item/VisualModel/BP_Item_CoinGold.BP_Item_CoinGold_C",
//            "SubPathString" : ""
//    },
//        "DropItemType" : "EPalDropItemType::None",
//        "Editor_RowNameHash" : -177975809,
//        "RestoreSatiety" : 0,
//        "RestoreConcentration" : 0,
//        "RestoreSanity" : 0,
//        "RestoreHealth" : 0,
//        "GrantEffect1Id" : 0,
//        "GrantEffect1Time" : 0.0,
//        "GrantEffect2Id" : 0,
//        "GrantEffect2Time" : 0.0,
//        "GrantEffect3Id" : 0,
//        "GrantEffect3Time" : 0.0,
//        "Durability" : 0,
//        "ElementType" : "EPalElementType::None",
//        "bSleepWeapon" : false,
//        "MagazineSize" : 0,
//        "SneakAttackRate" : 1.0,
//        "PhysicalAttackValue" : 0,
//        "HPValue" : 0,
//        "PhysicalDefenseValue" : 0,
//        "ShieldValue" : 0,
//        "MagicAttackValue" : 0,
//        "MagicDefenseValue" : 0,
//        "PassiveSkillName" : "None",
//        "PassiveSkillName2" : "None",
//        "PassiveSkillName3" : "None",
//        "PassiveSkillName4" : "None",
//        "WazaID" : "EPalWazaID::None",
//        "CorruptionFactor" : 0.0,
//        "FloatValue1" : 0.0
//},

class AItemActor;

USTRUCT(BlueprintType)
struct FPalStaticItemDataStruct : public FTableRowBase
{
	GENERATED_BODY()
public:
	// Override Name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	FString OverrideName{};
	// Override Description
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	FString OverrideDescription{};
	// Icon Name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	FString IconName{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	int32 MaxStackCount = 0;
	// Weight
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	float Weight = 0.0f;
	// Price
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	int32 Price = 0;
	// Sort ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	int32 SortID = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	bool bNotConsumed = false;
	// b Enable Handcraft
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	bool bEnableHandcraft = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	FString VisualBlueprintClassName{};
	//	VisualBlueprintClassSoft
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	TSubclassOf<UObject> VisualBlueprintClassSoft{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalStaticItemDataStruct")
	int64 Editor_RowNameHash{};
};

// LOCTEXT 에 사용할 네임스페이스 정의
	// 단일 파일 안에서만 유효하며, 파일이 끝나기 전 정의해야 합니다.
