#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "Pal/Interface/PalSaveable.h"
#include "PalSaveSubsystem.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UPalSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static bool Load(IPalSaveable* pInterface);
	static bool SaveString(IPalSaveable* pInterface);
	static bool SaveStringArray(IPalSaveable* pInterface);
	static bool SaveBinary(IPalSaveable* pInterface);

	static void RegisterSaveable(TScriptInterface<IPalSaveable> Saveable);
	static void UnregisterSaveable(TScriptInterface<IPalSaveable> Saveable);
	static void SaveAll();
	static void LoadAll();

private:
	static bool Load(IPalSaveable* pInterface, EPalDataType DataType);

	static bool loadStringData(const FString& FilePath, FString& rResult);
	static bool loadStringArrayData(const FString& FilePath, TArray<FString>& rResult);
	static bool loadBinaryData(const FString& FilePath, TArray<uint8>& rResult);

	static UPalSaveSubsystem* SingletonInstance;

	UPROPERTY()
	TArray<TScriptInterface<IPalSaveable>> RegisteredSaveables;

public:
	// 임의 직렬화 헬퍼 (포인터 역참조 수정)
	template<typename T>
	static bool saveBinaryFile(TArray<uint8>& BinaryArray, T* Content)
	{
		if (!Content)
			return false;
		FMemoryWriter Writer(BinaryArray, true);
		Writer << *Content;
		return true;
	}

	template<typename T>
	static bool readBinaryFile(const TArray<uint8>& BinaryArray, T* Content)
	{
		if (!Content)
			return false;
		FMemoryReader Reader(BinaryArray, true);
		Reader << *Content;
		return true;
	}
};