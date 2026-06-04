#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PalSaveable.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPalDataType : uint8
{
	None = 0,
	String = 1,
	StringArray = 1 << 1,
	Binary = 1 << 2,
};

UINTERFACE(MinimalAPI)
class UPalSaveable : public UInterface
{
	GENERATED_BODY()
};

// 데이터 저장/로드 대상 객체가 구현하는 인터페이스
class PROJECTOPENWORLD_API IPalSaveable
{
	GENERATED_BODY()
public:
	virtual EPalDataType getDataType() = 0;
	virtual FString getFilePath(EPalDataType eType) = 0;

	// 로드된 데이터를 받아가는 콜백
	virtual void loadStringData(FString& Data) {}
	virtual void loadStringArrayData(TArray<FString>& arData) {}
	virtual void loadBinaryData(TArray<uint8>& arData) {}

	// 저장할 데이터를 제공
	virtual FString getStringData() { return FString{}; }
	virtual TArray<FString> getStringArrayData() { return TArray<FString>{}; }
	virtual TArray<uint8> getBinaryData() { return TArray<uint8>{}; }
};