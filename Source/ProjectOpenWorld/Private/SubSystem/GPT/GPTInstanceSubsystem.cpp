#include "SubSystem/GPT/GPTInstanceSubsystem.h"
#include "VaRestSubsystem.h"
#include "VaRestRequestJSON.h"
#include "VaRestTypes.h"
#include "VaRestJsonObject.h"
#include "VaRestJsonValue.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Subsystem/GPT/GPTResponseInterface.h"
#include "SubSystem/DeveloperSettings/GPTSettings.h"

UVaRestRequestJSON* UGPTInstanceSubsystem::GetRequest(EVaRestRequestVerb verb, EVaRestRequestContentType contentType, UObject* ResponseTarget,
	const TArray< FGPTField>* PostFields)
{
	UVaRestRequestJSON* Request = VaRestSubsystem->ConstructVaRestRequestExt(verb, contentType); // );
	if (Request)
	{
		FScriptDelegate Delegate{};
		Delegate.BindUFunction(ResponseTarget, TEXT("OnRequestComplete"));
		Request->OnRequestComplete.Add(Delegate);
		Delegate.Clear();

		Delegate.BindUFunction(ResponseTarget, TEXT("OnRequestFail"));
		Request->OnRequestFail.Add(Delegate);

		FGPTField KeyFeild = UGPTSettings::GetGPTKey();
		Request->SetHeader(KeyFeild.Header, KeyFeild.Value);
		if (PostFields)
		{
			for (const FGPTField& Feild : *PostFields)
			{
				Request->SetHeader(Feild.Header, Feild.Value);
			}
		}
	}
	return Request;
}

bool UGPTInstanceSubsystem::CheckSendable()
{
	if (!VaRestSubsystem)
		VaRestSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>();
	if (!VaRestSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant find UVaRestSubsystem"));
		return false;
	}
	FGPTField Feild = UGPTSettings::GetGPTKey();
	if (Feild.Value == TEXT("YOUR_API_KEY_HERE"))
	{
		UE_LOG(LogTemp, Warning, TEXT("GPT API Key is Empty"));
		return false;
	}
	return true;
}

UVaRestJsonValue* UGPTInstanceSubsystem::GetJsonValue(UVaRestJsonValue* JsonValue, const FString* ArrayFields, int Count) const
{
	if (!JsonValue)
		return nullptr;
	if (Count == 0)
	{
		return JsonValue;
	}
	UVaRestJsonValue* Result = nullptr;
	if (JsonValue->GetType() == EVaJson::Object)
	{
		UVaRestJsonObject* JsonValueObj = JsonValue->AsObject();
		Result = GetJsonValue(JsonValueObj->GetField(ArrayFields[0]), ArrayFields + 1, Count - 1);
	}
	else if (JsonValue->GetType() == EVaJson::Array)
	{
		TArray<UVaRestJsonValue*> JsonValueObj = JsonValue->AsArray();
		for (int i = 0; i < JsonValueObj.Num() && Result == nullptr; i++)
		{
			Result = GetJsonValue(JsonValueObj[i] , ArrayFields, Count);
		}
	}
	return Result;
}

void UGPTInstanceSubsystem::SetRequestField(UVaRestJsonObject* RequestRootObject, const TArray<FGPTField>& Fields)
{
	if (RequestRootObject)
	{
		//https://developers.openai.com/api/reference/resources/responses/methods/create
		for (const FGPTField& Feild : Fields)
		{
			if (Feild.ValueType == EVaJson::String)
			{
				RequestRootObject->SetStringField(Feild.Header, Feild.Value);
			}
			else if (Feild.ValueType == EVaJson::Number)
			{
				RequestRootObject->SetNumberField(Feild.Header, FCString::Atof(*Feild.Value));
			}
			else if (Feild.ValueType == EVaJson::Boolean)
			{
				RequestRootObject->SetBoolField(Feild.Header, FCString::ToBool(*Feild.Value));
			}
			else if (Feild.ValueType == EVaJson::Array)
			{
				TArray<UVaRestJsonValue*> ArrayValues;
				for (const FString& Value : Feild.ValueArray)
				{
					ArrayValues.Add(VaRestSubsystem->ConstructJsonValueString(Value));
				}
				RequestRootObject->SetArrayField(Feild.Header, ArrayValues);
			}
		}
	}
}

UVaRestJsonValue* UGPTInstanceSubsystem::GetJsonValue(UVaRestRequestJSON* Request, const TArray<FString>& FieldPath, EVaJson& Type) const
{
	if (!Request || FieldPath.IsEmpty())
		return nullptr;
	UVaRestJsonValue* Root = VaRestSubsystem->DecodeJsonValue(Request->GetResponseObject()->EncodeJson());
	UVaRestJsonValue* Value = GetJsonValue(Root, FieldPath.GetData(), FieldPath.Num());
	if (Root == Value)
		Value = nullptr;
	if (Value)
	{
		Type = Value->GetType();
	}
	return Value;
}

void UGPTInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	VaRestSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>();
	//APIKey = TEXT("Bearer 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");
}

void UGPTInstanceSubsystem::SendGPTImageRequest(FGPTImageRequest RequestData, TScriptInterface< IGPTResponseInterface> Target)
{
	//https://developers.openai.com/api/reference/resources/images/methods/generate
	// Check Send Condition
	if (!Target.GetObject())
	{
		if (Target.GetObject())
		{
			UE_LOG(LogTemp, Error, TEXT("%s No Interface :SendGPTImageRequest"), *Target.GetObject()->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No Object :SendGPTImageRequest"));
		}
		return;
	}
	if (!CheckSendable())
		return;
	if (!RequestData.CheckSendable())
		return;

	FGPTImagePostData ImagePrompt = UGPTSettings::GetImagePostData();
	UVaRestRequestJSON* Request = GetRequest(EVaRestRequestVerb::POST, EVaRestRequestContentType::json, Target.GetObject(), &ImagePrompt.PostFields);
	if (Request)
	{
		UVaRestJsonObject* RequestRootObject = VaRestSubsystem->ConstructVaRestJsonObject();
		if (RequestRootObject)
		{
			SetRequestField(RequestRootObject, ImagePrompt.PromptFields);
			RequestRootObject->SetStringField(ImagePrompt.PromptHeader, RequestData.Text);
			Request->SetRequestObject(RequestRootObject);
			Request->ProcessURL(ImagePrompt.RequestURL);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Can't Make RestJsonObject :SendGPTImageRequest"));
		}
	}
}

void UGPTInstanceSubsystem::SendGetGPTResponse(FString modelID, TScriptInterface< IGPTResponseInterface> Target)
{
	if (!Target.GetObject() || !Target.GetObject()->Implements<UGPTResponseInterface>())
	{
		if (Target.GetObject())
		{
			UE_LOG(LogTemp, Error, TEXT("%s No Interface :SendGetGPTResponse"), *Target.GetObject()->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No Object :SendGetGPTResponse"));
		}
		return;
	}
	if (!CheckSendable())
		return;

	UVaRestRequestJSON* Request = GetRequest(EVaRestRequestVerb::GET, EVaRestRequestContentType::json, Target.GetObject());
	if (Request)
	{
		Request->ProcessURL(TEXT("https://api.openai.com/v1/responses/" + modelID));
	}
}

FString UGPTInstanceSubsystem::GetRequestID(UVaRestRequestJSON* Request) const
{
	TArray<FString> Fields = UGPTSettings::GetResponsePath(EResponseType::RESPONSEID);
	EVaJson type{};
	UVaRestJsonValue* Value = GetJsonValue(Request, Fields, type);
	if (Value && type == EVaJson::String)
	{
		return Value->AsString();
	}
	return FString();
}

UTexture2D* UGPTInstanceSubsystem::GetResponseTexture(UVaRestRequestJSON* Request) const
{
	EVaJson type{};
	TArray<FString> Fields = UGPTSettings::GetResponsePath(EResponseType::IMAGE);
	UVaRestJsonValue* Value = GetJsonValue(Request, Fields, type);
	if (Value && type == EVaJson::String)
	{
		FString Base64PNG = Value->AsString();
		Base64PNG = Base64PNG.TrimStartAndEnd();

		TArray<uint8> PngBytes;
		if (!FBase64::Decode(Base64PNG, PngBytes) || Base64PNG.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("GetResponseTexture: Base64 decode failed or Base64PNG is empty"));
			return nullptr;
		}
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	
		if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(PngBytes.GetData(), PngBytes.Num()))
		{
			UE_LOG(LogTemp, Error, TEXT("GetResponseTexture: Failed to create Texture2D"));
			return nullptr;
		}

		const int32 Width = ImageWrapper->GetWidth();
		const int32 Height = ImageWrapper->GetHeight();
	
		TArray<uint8> RawData;
		if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData) || Width <= 0 || Height <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("GetResponseTexture: Failed to create Texture2D"));
			return nullptr;
		}
	
		UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
		if (!NewTexture)
		{
			UE_LOG(LogTemp, Error, TEXT("CreateTexture2DFromPngBytes: CreateTransient failed."));
			return nullptr;
		}
	
	#if WITH_EDITORONLY_DATA
		NewTexture->CompressionSettings = TC_Default;
		NewTexture->SRGB = true;
	#endif
	
		FTexture2DMipMap& Mip = NewTexture->GetPlatformData()->Mips[0];
		void* TextureData = Mip.BulkData.Lock(LOCK_READ_WRITE);
	
		const int32 ExpectedDataSize = Width * Height * 4; // BGRA
		if (RawData.Num() != ExpectedDataSize)
		{
			UE_LOG(LogTemp, Warning, TEXT("CreateTexture2DFromPngBytes: RawData.Num=%d, Expected=%d"), RawData.Num(), ExpectedDataSize);
		}
	
		FMemory::Memcpy(TextureData, RawData.GetData(), FMath::Min(RawData.Num(), ExpectedDataSize));
		Mip.BulkData.Unlock();
		NewTexture->UpdateResource();
		return NewTexture;
	}
	return nullptr;
}

int32 UGPTInstanceSubsystem::GetResponseTotalTogens(UVaRestRequestJSON* Request) const
{
	TArray<FString> Fields = UGPTSettings::GetResponsePath(EResponseType::TOTALTOKEN);
	EVaJson type{};
	UVaRestJsonValue* Value = GetJsonValue(Request, Fields, type);
	if (Value && type == EVaJson::Number)
	{
		return Value->AsInt32();
	}
	return -1;
}


bool FGPTStringRequest::CheckSendable() const
{
	//|| RequestData.Frompt.Contains("Http") >= 0 || RequestData.Frompt.Contains("http") >= 0)
	// 최근 ai에서 url을 해석하게 하는 방법으로 인젝션 공격을 한 사례가 있어서 추가
	if (prompt.IsEmpty() || Text.IsEmpty() ||
		Text.Contains("Http", ESearchCase::IgnoreCase) || prompt.Contains("Http", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogTemp, Warning, TEXT("GPT Request Invalid: %s"), *Text);
		return false;
	}
	return true;
}

bool FGPTImageRequest::CheckSendable() const
{
	//|| RequestData.Frompt.Contains("Http") >= 0 || RequestData.Frompt.Contains("http") >= 0)
	// 최근 ai에서 url을 해석하게 하는 방법으로 인젝션 공격을 한 사례가 있어서 추가
	if (Text.IsEmpty() ||
		Text.Contains("Http", ESearchCase::IgnoreCase))// || Frompt.Contains("Http", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogTemp, Warning, TEXT("GPT Request Invalid: %s"), *Text);
		return false;
	}
	return true;
}
