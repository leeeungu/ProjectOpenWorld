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

UVaRestRequestJSON* UGPTInstanceSubsystem::GetRequest(EVaRestRequestVerb verb, EVaRestRequestContentType contentType, UObject* ResponseTarget)
{
	UVaRestRequestJSON* Request = VaRestSubsystem->ConstructVaRestRequestExt(verb, contentType); // );
	if (Request)
	{
		{
			FScriptDelegate Delegate{};
			Delegate.BindUFunction(ResponseTarget, TEXT("OnRequestComplete"));
			Request->OnRequestComplete.Add(Delegate);
		}
		{
			FScriptDelegate Delegate{};
			Delegate.BindUFunction(ResponseTarget, TEXT("OnRequestFail"));
			Request->OnRequestFail.Add(Delegate);
		}

		Request->SetHeader("Authorization", APIKey);
		Request->SetHeader("Content-Type", "application/json");
		// 서버에서 관리가 필요하지만 생략
		Request->SetHeader("OpenAI-Organization", "org-BZKAwjvbzNgg3Uszu2Pb91Hz");
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
	if (APIKey == TEXT("YOUR_API_KEY_HERE"))
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

TArray<FString> UGPTInstanceSubsystem::GetResponsePath(EResponseType type) const
{
	switch (type)
	{
	case UGPTInstanceSubsystem::EResponseType::RESPONSEID:
		return { TEXT("id") };
	case UGPTInstanceSubsystem::EResponseType::IMAGE:
		return  { TEXT("output"), TEXT("result") };
	case UGPTInstanceSubsystem::EResponseType::TEXT:
		return { TEXT("output"), TEXT("content"), TEXT("text") };
	case UGPTInstanceSubsystem::EResponseType::TOTALTOKEN:
		return { TEXT("usage"), TEXT("total_tokens") };
	}
	return TArray<FString>();
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
	APIKey = TEXT("Bearer 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");
}

void UGPTInstanceSubsystem::SendGPTStringRequest(FGPTStringRequest RequestData, UObject* Target)
{
	if (!Target || !Target->Implements<UGPTResponseInterface>())
	{
		UE_LOG(LogTemp, Error, TEXT("%s No Interface : UGPTResponseInterface"), *Target->StaticClass()->GetName());
		return;
	}	if (!CheckSendable())
		return;
	if (!RequestData.CheckSendable())
		return;

	UVaRestRequestJSON* Request = GetRequest(EVaRestRequestVerb::POST, EVaRestRequestContentType::json, Target);
	if (Request)
	{
		UVaRestJsonObject* RequestObject = VaRestSubsystem->ConstructVaRestJsonObject();
		{
			RequestObject->SetStringField(TEXT("model"), TEXT("gpt-4.1"));
		}
		{
			TArray< UVaRestJsonValue*> array{};
			UVaRestJsonObject* RequestValueObj = VaRestSubsystem->ConstructVaRestJsonObject();
			RequestValueObj->SetStringField(TEXT("role"), TEXT("developer"));
			RequestValueObj->SetStringField(TEXT("content"), RequestData.Frompt);
			array.Add(VaRestSubsystem->ConstructJsonValueObject(RequestValueObj));
			RequestValueObj = VaRestSubsystem->ConstructVaRestJsonObject();
			RequestValueObj->SetStringField(TEXT("role"), TEXT("user"));
			RequestValueObj->SetStringField(TEXT("content"), RequestData.Text);
			array.Add(VaRestSubsystem->ConstructJsonValueObject(RequestValueObj));
			RequestObject->SetArrayField(TEXT("input"), array);
			// 100 되면 끊김
			//RequestObject->SetIntegerField(TEXT("max_output_tokens"), 100);
		}

		Request->SetRequestObject(RequestObject);
		Request->ProcessURL("https://api.openai.com/v1/responses");
	}
}

void UGPTInstanceSubsystem::SendGPTImageRequest(FGPTImageRequest RequestData, UObject* Target)
{
	if (!Target || !Target->Implements<UGPTResponseInterface>())
	{
		UE_LOG(LogTemp, Error, TEXT("%s No Interface :UGPTResponseInterface"), *Target->StaticClass()->GetName());
		return;
	}
	if (!CheckSendable())
		return;
	if (!RequestData.CheckSendable())
		return;

	UVaRestRequestJSON* Request = GetRequest(EVaRestRequestVerb::POST, EVaRestRequestContentType::json, Target);
	if (Request)
	{
		UVaRestJsonObject* RequestRootObject = VaRestSubsystem->ConstructVaRestJsonObject();
		{
			// https://platform.openai.com/docs/guides/tools-image-generation#page-top
			RequestRootObject->SetStringField(TEXT("model"), TEXT("gpt-4.1-mini"));
			RequestRootObject->SetStringField(TEXT("input"), RequestData.Text);
			{
				TArray< UVaRestJsonValue*> array{};
				UVaRestJsonObject* RequestValueObj = VaRestSubsystem->ConstructVaRestJsonObject();
				RequestValueObj->SetStringField(TEXT("type"), TEXT("image_generation"));
				RequestValueObj->SetStringField(TEXT("quality"), TEXT("low"));
				RequestValueObj->SetStringField(TEXT("size"), TEXT("1024x1024"));
				array.Add(VaRestSubsystem->ConstructJsonValueObject(RequestValueObj));
				RequestRootObject->SetArrayField(TEXT("tools"), array);
			}
			/*{
				UVaRestJsonObject* RequestValueObj = VaRestSubsystem->ConstructVaRestJsonObject();
				RequestValueObj->SetStringField(TEXT("size"), TEXT("1024x1024"));
				RequestValueObj->SetStringField(TEXT("quality"), TEXT("Low"));
				RequestRootObject->SetObjectField(TEXT("image"), RequestValueObj);
			}*/
		}

		Request->SetRequestObject(RequestRootObject);
		Request->ProcessURL(TEXT("https://api.openai.com/v1/responses"));
	}
}

void UGPTInstanceSubsystem::SendGetGPTResponse(FString modelID, UObject* Target)
{
	if (!Target || !Target->Implements<UGPTResponseInterface>())
	{
		UE_LOG(LogTemp, Error, TEXT("%s No Interface :UGPTResponseInterface"), *Target->StaticClass()->GetName());
		return;
	}
	if (!CheckSendable())
		return;

	UVaRestRequestJSON* Request = GetRequest(EVaRestRequestVerb::GET, EVaRestRequestContentType::json, Target);
	if (Request)
	{
		Request->ProcessURL(TEXT("https://api.openai.com/v1/responses/" + modelID));
	}
}

FString UGPTInstanceSubsystem::GetResponseString(UVaRestRequestJSON* Request) const
{
	TArray<FString> Fields = GetResponsePath(EResponseType::TEXT);// { TEXT("output"), TEXT("content"), TEXT("text") };
	EVaJson type{};
	UVaRestJsonValue* Value = GetJsonValue(Request, Fields, type);
	if (Value && type==EVaJson::String)
	{
		return Value->AsString();
	}
	return FString();
}

FString UGPTInstanceSubsystem::GetRequestID(UVaRestRequestJSON* Request) const
{
	TArray<FString> Fields = GetResponsePath(EResponseType::RESPONSEID);
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
	TArray<FString> Fields = GetResponsePath(EResponseType::IMAGE);
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
	TArray<FString> Fields = GetResponsePath(EResponseType::TOTALTOKEN);
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
	if (Frompt.IsEmpty() || Text.IsEmpty() ||
		Text.Contains("Http", ESearchCase::IgnoreCase) || Frompt.Contains("Http", ESearchCase::IgnoreCase))
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
