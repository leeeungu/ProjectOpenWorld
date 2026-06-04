#include "Pal/Subsystem/PalSaveSubsystem.h"
#include "Misc/FileHelper.h"

UPalSaveSubsystem* UPalSaveSubsystem::SingletonInstance{};

void UPalSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SingletonInstance = this;
}

void UPalSaveSubsystem::Deinitialize()
{
	if (SingletonInstance == this)
		SingletonInstance = nullptr;
	Super::Deinitialize();
}

bool UPalSaveSubsystem::Load(IPalSaveable* pInterface)
{
	if (!pInterface)
		return false;

	const uint8 Type = (uint8)pInterface->getDataType();
	bool bAny = false;
	bool bAllOk = true;

	// 단일 비트 단위로 순회 (1,2,4,... uint8 오버플로로 종료)
	for (uint8 Bit = 1; Bit != 0; Bit <<= 1)
	{
		if ((Type & Bit) == 0)
			continue;
		bAny = true;
		bAllOk &= Load(pInterface, (EPalDataType)Bit);
	}
	return bAny && bAllOk;
}

bool UPalSaveSubsystem::Load(IPalSaveable* pInterface, EPalDataType DataType)
{
	if (!pInterface)
		return false;

	const uint8 Type = (uint8)pInterface->getDataType();
	// 요청한 비트가 대상 타입에 포함되지 않으면 거부
	if ((Type & (uint8)DataType) == 0)
		return false;

	const FString Path = pInterface->getFilePath(DataType);
	switch (DataType)
	{
	case EPalDataType::String:
	{
		FString Data;
		if (!loadStringData(Path, Data))
			return false;
		pInterface->loadStringData(Data);
		break;
	}
	case EPalDataType::StringArray:
	{
		TArray<FString> Data;
		if (!loadStringArrayData(Path, Data))
			return false;
		pInterface->loadStringArrayData(Data);
		break;
	}
	case EPalDataType::Binary:
	{
		TArray<uint8> Data;
		if (!loadBinaryData(Path, Data))
			return false;
		pInterface->loadBinaryData(Data);
		break;
	}
	default:
		return false;
	}
	return true;
}

bool UPalSaveSubsystem::SaveString(IPalSaveable* pInterface)
{
	if (!pInterface)
		return false;
	return FFileHelper::SaveStringToFile(pInterface->getStringData(), *pInterface->getFilePath(EPalDataType::String));
}

bool UPalSaveSubsystem::SaveStringArray(IPalSaveable* pInterface)
{
	if (!pInterface)
		return false;
	return FFileHelper::SaveStringArrayToFile(pInterface->getStringArrayData(), *pInterface->getFilePath(EPalDataType::StringArray));
}

bool UPalSaveSubsystem::SaveBinary(IPalSaveable* pInterface)
{
	if (!pInterface)
		return false;
	return FFileHelper::SaveArrayToFile(pInterface->getBinaryData(), *pInterface->getFilePath(EPalDataType::Binary));
}

void UPalSaveSubsystem::RegisterSaveable(TScriptInterface<IPalSaveable> Saveable)
{
	if (SingletonInstance && Saveable)
		SingletonInstance->RegisteredSaveables.AddUnique(Saveable);
}

void UPalSaveSubsystem::UnregisterSaveable(TScriptInterface<IPalSaveable> Saveable)
{
	if (SingletonInstance)
		SingletonInstance->RegisteredSaveables.Remove(Saveable);
}

void UPalSaveSubsystem::SaveAll()
{
	if (!SingletonInstance)
		return;
	for (const TScriptInterface<IPalSaveable>& Entry : SingletonInstance->RegisteredSaveables)
	{
		IPalSaveable* p = Entry.GetInterface();
		if (!p)
			continue;
		const uint8 Type = (uint8)p->getDataType();
		if (Type & (uint8)EPalDataType::String)     
			SaveString(p);
		if (Type & (uint8)EPalDataType::StringArray) 
			SaveStringArray(p);
		if (Type & (uint8)EPalDataType::Binary)     
			SaveBinary(p);
	}
}

void UPalSaveSubsystem::LoadAll()
{
	if (!SingletonInstance)
		return;
	for (const TScriptInterface<IPalSaveable>& Entry : SingletonInstance->RegisteredSaveables)
	{
		if (IPalSaveable* p = Entry.GetInterface())
			Load(p);
	}
}

bool UPalSaveSubsystem::loadStringData(const FString& FilePath, FString& rResult)
{
	return FFileHelper::LoadFileToString(rResult, *FilePath); // 핵심 수정: Save → Load
}

bool UPalSaveSubsystem::loadStringArrayData(const FString& FilePath, TArray<FString>& rResult)
{
	rResult.Reset();
	return FFileHelper::LoadFileToStringArray(rResult, *FilePath);
}

bool UPalSaveSubsystem::loadBinaryData(const FString& FilePath, TArray<uint8>& rResult)
{
	rResult.Reset();
	return FFileHelper::LoadFileToArray(rResult, *FilePath); // 이중 호출 제거
}