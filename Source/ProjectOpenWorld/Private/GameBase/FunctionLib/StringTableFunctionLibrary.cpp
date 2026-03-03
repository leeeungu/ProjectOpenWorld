#include "GameBase/FunctionLib/StringTableFunctionLibrary.h"	
#include "Internationalization/StringTableRegistry.h"
#include "Internationalization/StringTableCore.h"

FText UStringTableFunctionLibrary::GetItemNameFromStringTable(const FString& InKey)
{
	FText TableData = FText::FromStringTable(GetItemNameStringTable(), InKey);
	if (TableData.ToString() == FStringTableEntry::GetPlaceholderSourceString())
	{
		TableData = FText::FromString("???");
	}
	return TableData;
}

FText UStringTableFunctionLibrary::GetItemDescFromStringTable(const FString& InKey)
{
	FText TableData = FText::FromStringTable(GetItemDescStringTable(), InKey);
	if (TableData.ToString() == FStringTableEntry::GetPlaceholderSourceString())
	{
		TableData = FText::FromStringTable(GetItemDescStringTable(), "???");
	}
	return TableData;
}

FText UStringTableFunctionLibrary::GetBuildingNameFromStringTable(const FString& InKey)
{
	FText TableData = FText::FromStringTable(GetBuildingNameStringTable(), InKey);
	if (TableData.ToString() == FStringTableEntry::GetPlaceholderSourceString())
	{
		TableData = FText::FromName(NAME_None);
	}
	return TableData;
}

FText UStringTableFunctionLibrary::GetBuildingDescFromStringTable(const FString& InKey)
{
	FText TableData = FText::FromStringTable(GetBuildingDescStringTable(), InKey);
	if (TableData.ToString() == FStringTableEntry::GetPlaceholderSourceString())
	{
		TableData = FText::FromName(NAME_None);
	}
	return TableData;
}
