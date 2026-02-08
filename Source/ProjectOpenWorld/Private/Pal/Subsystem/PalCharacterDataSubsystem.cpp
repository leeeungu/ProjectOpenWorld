#include "Pal/Subsystem/PalCharacterDataSubsystem.h"

UPalCharacterDataSubsystem* UPalCharacterDataSubsystem::SingletonInstance{};

void UPalCharacterDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SingletonInstance = this;

	//Script/Engine.DataTable'/Game/Pal/DataTable/DT_PalCharacterIconDataTable.DT_PalCharacterIconDataTable'
	if (!LoadAndSaveDataTableToMap(PalCharacterIconDataTable, TEXT("/Game/Pal/DataTable/DT_PalCharacterIconDataTable.DT_PalCharacterIconDataTable")))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load PalCharacterIconData DataTable"));
	}
}

bool UPalCharacterDataSubsystem::GetPalCharacterIconData(FName RowName, const FPalCharacterIconDataRow*& Data)
{
	if (!SingletonInstance)
		return false;
	if (const FPalCharacterIconDataRow* const* FoundData = SingletonInstance->PalCharacterIconDataTable.DataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &SingletonInstance->PalCharacterIconDataTable.Dummy;
	return false;
}

UTexture2D* UPalCharacterDataSubsystem::GetPalCharacterIconByName(FName RowName)
{
	const FPalCharacterIconDataRow* Data = nullptr;
	if (GetPalCharacterIconData(RowName, Data))
	{
		return Data->Icon;
	}
	return nullptr;
}
