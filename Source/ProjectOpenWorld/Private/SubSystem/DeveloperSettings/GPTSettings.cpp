#include "SubSystem/DeveloperSettings/GPTSettings.h"
#include "VaRestJsonValue.h"

UGPTSettings::UGPTSettings(const FObjectInitializer& ObjectInitializer)
{
    CategoryName = TEXT("Project"); // 상단 "Project" 그룹에 표시
    SectionName = TEXT("GPT");
	GPTKey.ValueType = EVaJson::String;
    GPTKey.Value = TEXT("YOUR_API_KEY_HERE");
}

TArray<FString> UGPTSettings::GetResponsePath(EResponseType Type)
{
    const UGPTSettings* Setting = GetDefault<UGPTSettings>();
    if (const FGPTResponsePath* Found = Setting->ResponsePaths.Find(Type))
        return Found->Keys;
    return TArray<FString>();
}

FGPTImagePostData UGPTSettings::GetImagePostData()
{
    if (const UGPTSettings* Setting = GetDefault<UGPTSettings>())
    {
        return Setting->ImagePostData;
    }
    return FGPTImagePostData();
}

FGPTField UGPTSettings::GetGPTKey()
{
    if (const UGPTSettings* Setting = GetDefault<UGPTSettings>())
    {
        return Setting->GPTKey;
    }
    return FGPTField();
}

//FString UGPTSettings::GetRequestURL(ERequestURLType Type)
//{
//    if (const UGPTSettings* Setting = GetDefault<UGPTSettings>())
//    {
//        if (const FString* Result = Setting->RequestURL.Find(Type))
//            return *Result;
//    }
//    return FString();
//}
//
//FString UGPTSettings::GetRequestHeader(ERequestHeader Type)
//{
//    if (const UGPTSettings* Setting = GetDefault<UGPTSettings>())
//    {
//        if (const FString* Result = Setting->ImageRequestHeader.Find(Type))
//            return *Result;
//    }
//    return FString();
//}
//
//bool UGPTSettings::GetRequestPrompt(FGPTRequestPrompt& Result)
//{
//    return false;
//}

//bool UGPTSettings::GetRequestPrompt(FGPTRequestPrompt& Result)
//{
//    Result = FGPTRequestPrompt();
//    if (const UGPTSettings* Setting = GetDefault<UGPTSettings>())
//    {
//        Result = Setting->RequestPrompt;
//        return true;
//    }
//    return false;
//}

UGPTValue_String::UGPTValue_String()
{
   ValueType = EVaJson::String;
}

UGPTValue_Array::UGPTValue_Array()
{
    ValueType = EVaJson::Array;
}

UGPTValue_Number::UGPTValue_Number()
{
    ValueType = EVaJson::Number;
}

UGPTValue_Object::UGPTValue_Object()
{
    ValueType = EVaJson::Object;
}
