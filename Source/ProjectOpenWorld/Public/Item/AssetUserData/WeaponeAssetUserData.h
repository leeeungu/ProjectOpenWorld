#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "Item/DataTable/WeaponeData.h"
#include "WeaponeAssetUserData.generated.h"

enum class EPlayerState : uint8;

UCLASS()
class PROJECTOPENWORLD_API UWeaponeAssetUserData : public UAssetUserData
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapone")
	FName SocketName{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapone")
	EWeapone WeaponeData{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapone")
	EPlayerState EquipPlayerState{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapone")
	EPlayerState UnEquipPlayerState{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapone")
	FName WeaponeID{};
public:
	FName GetSocketName() const { return SocketName; }
	EWeapone GetWeaponeData() const { return WeaponeData; }
	EPlayerState GetEquipPlayerState() const { return EquipPlayerState; }
	EPlayerState GetUnEquipPlayerState() const { return UnEquipPlayerState; }
	FName GetWeaponeID() const { return WeaponeID; }
};
