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

TArray<uint8> UGPTInstanceSubsystem::GetPNGData(UVaRestRequestJSON* Request)
{
	if (!Request)
		return TArray<uint8>();
	UVaRestJsonValue* Value = VaRestSubsystem->DecodeJsonValue(Request->GetResponseContentAsString());
	TArray<FString> Fields = { TEXT("output"), TEXT("result") };
	Value = GetJsonValue(Value, Fields.GetData(), Fields.Num());
	if (!Value || Value->GetType() != EVaJson::String)
	{
		UE_LOG(LogTemp, Error, TEXT("ProcessOpenAIImageResponse: Base64PNG is empty."));
		return TArray<uint8>();
	}
	FString Base64PNG = Value->AsString();
	Base64PNG = Base64PNG.TrimStartAndEnd();

	TArray<uint8> PngBytes;
	if (!FBase64::Decode(Base64PNG, PngBytes) || Base64PNG.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("ProcessOpenAIImageResponse: Base64 decode failed or Base64PNG is empty"));
		return TArray<uint8>();
	}
	return PngBytes;
}

int UGPTInstanceSubsystem::GetTotalTogens(UVaRestRequestJSON* Request) const
{
	UVaRestJsonValue* Value = VaRestSubsystem->DecodeJsonValue(Request->GetResponseContentAsString());
	TArray<FString> Fields = { TEXT("usage"), TEXT("total_tokens") };
	Value = GetJsonValue(Value, Fields.GetData(), Fields.Num());
	if (Value)
	{
		return Value->AsNumber();
	}
	return -1;
}

TArray<UVaRestJsonValue*> UGPTInstanceSubsystem::GetResponseArrayField(UVaRestRequestJSON* Request, const FString& FieldName)
{
	UVaRestJsonValue* Value = VaRestSubsystem->DecodeJsonValue(Request->GetResponseContentAsString());
	if (Value && Value->GetType() == EVaJson::Object)
	{
		UVaRestJsonObject* Response = Value->AsObject();
		if (Response)
		{
			return Response->GetArrayField(FieldName);
		}
	}
	return TArray<UVaRestJsonValue*>();
}

UVaRestJsonValue* UGPTInstanceSubsystem::GetJsonValue_Recul(UVaRestJsonValue* JsonValue, const FString& FieldName, const EVaJson& Type) const
{
	UVaRestJsonValue* Result = nullptr;
	if (JsonValue)
	{
		if (JsonValue->GetType() == EVaJson::Array)
		{
			TArray< UVaRestJsonValue*> Array = JsonValue->AsArray();
			for (UVaRestJsonValue*& key : Array)
			{
				Result = GetJsonValue_Recul(key, FieldName, Type);
				if (Result && Result->GetType() == Type)
				{
					break;
				}
			}
		}
		else if (JsonValue->GetType() == EVaJson::Object)
		{
			UVaRestJsonObject* JsonValueObj = JsonValue->AsObject();
			Result = JsonValueObj->GetField(FieldName);
			if (!Result || Result->GetType() != Type)
			{
				for (const FString& key : JsonValueObj->GetFieldNames())
				{
					UE_LOG(LogTemp, Warning, TEXT("Key : %s"), *key);
					Result = GetJsonValue_Recul(JsonValueObj->GetField(key), FieldName, Type);
					if (Result && Result->GetType() == Type)
					{
						break;
					}
				}
			}
		}
	}
	return Result;
}

UVaRestJsonValue* UGPTInstanceSubsystem::GetJsonValue(UVaRestJsonValue* JsonValue, const FString* ArrayFields, int Count) const
{
	if (!JsonValue)
		return nullptr;
	if (Count == 0)
	{
		return JsonValue;
	}

	if (JsonValue->GetType() == EVaJson::Object)
	{
		UVaRestJsonObject* JsonValueObj = JsonValue->AsObject();
		return GetJsonValue(JsonValueObj->GetField(ArrayFields[0]), ArrayFields + 1, Count - 1);
	}
	else if (JsonValue->GetType() == EVaJson::Array)
	{
		TArray<UVaRestJsonValue*> JsonValueObj = JsonValue->AsArray();
		UVaRestJsonValue* Result = nullptr;
		for (int i = 0; i < JsonValueObj.Num() && Result == nullptr; i++)
		{
			Result = GetJsonValue(JsonValueObj[i] , ArrayFields, Count);
		}
		return Result;
	}
	return nullptr;
}

FString UGPTInstanceSubsystem::GetResponseStringField(UVaRestRequestJSON* Request, const FString& FieldName)
{
	UVaRestJsonValue* Value = VaRestSubsystem->DecodeJsonValue(Request->GetResponseContentAsString());
	if (Value && Value->GetType() == EVaJson::Object)
	{
		UVaRestJsonObject* Response = Value->AsObject();
		if (Response)
		{
			return Response->GetStringField(FieldName);
		}
	}
	return "";
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
		UE_LOG(LogTemp, Error, TEXT("%s No Interface :UGPTResponseInterface"), *Target->StaticClass()->GetName());
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
			//"": 64
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

FString UGPTInstanceSubsystem::GetStringFromRequestJSON(UVaRestRequestJSON* Request)
{
	if (!Request)
		return {};
	UVaRestJsonValue * Value = VaRestSubsystem->DecodeJsonValue(Request->GetResponseContentAsString());
	TArray<FString> Fields =  { TEXT("output"), TEXT("content"), TEXT("text") };
	Value = GetJsonValue(Value, Fields.GetData(), Fields.Num());
	if (Value)
	{
		return Value->AsString();
	}
	return "";
}

UVaRestJsonValue* UGPTInstanceSubsystem::GetJsonValue(UVaRestRequestJSON* Request, const TArray<FString>& FieldPath, EVaJson& Type) const
{
	if (!Request || FieldPath.IsEmpty())
		return nullptr;
	UVaRestJsonValue* Root = VaRestSubsystem->DecodeJsonValue(Request->GetResponseContentAsString());
	UVaRestJsonValue* Value = GetJsonValue(Root, FieldPath.GetData(), FieldPath.Num());
	if (Root == Value)
		Value = nullptr;
	if (Value)
	{
		Type = Value->GetType();
	}
	return Value;
}

void UGPTInstanceSubsystem::GetGPTResponse(FString modelID, UObject* Target)
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
bool UGPTInstanceSubsystem::SaveImageData(UVaRestRequestJSON* Request, FString Path)
{
	TArray<uint8> PngBytes = GetPNGData(Request);
	// PNG 파일로 저장
	if (!PngBytes.IsEmpty())
	{
		FString SavePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("OpenAI_Image.png"));
		UE_LOG(LogTemp, Log, TEXT("Texture updated from OpenAI PNG Base64."));
		return 	FFileHelper::SaveArrayToFile(PngBytes, *SavePath);
	}
	return false;
}

bool UGPTInstanceSubsystem::ApplyImageData(UVaRestRequestJSON* Request, UTexture2D*& NewTexture)
{
	TArray<uint8> PngData = GetPNGData(Request);
	if (PngData.Num() == 0)
		return false;

	// 1) ImageWrapper 모듈 로드
	IImageWrapperModule& ImageWrapperModule =
		FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	// 2) PNG 래퍼 생성
	TSharedPtr<IImageWrapper> ImageWrapper =
		ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (!ImageWrapper.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("CreateTexture2DFromPngBytes: Failed to create PNG ImageWrapper."));
		return false;
	}

	// 3) 압축된 PNG 데이터 설정
	if (!ImageWrapper->SetCompressed(PngData.GetData(), PngData.Num()))
	{
		UE_LOG(LogTemp, Error, TEXT("CreateTexture2DFromPngBytes: SetCompressed failed."));
		return false;
	}

	// 4) 이미지 크기
	const int32 Width = ImageWrapper->GetWidth();
	const int32 Height = ImageWrapper->GetHeight();

	if (Width <= 0 || Height <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateTexture2DFromPngBytes: Invalid image size (%d x %d)."), Width, Height);
		return false;
	}

	// 5) PNG → BGRA 8bit raw 데이터
	TArray<uint8> RawData;
	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
	{
		UE_LOG(LogTemp, Error, TEXT("CreateTexture2DFromPngBytes: GetRaw failed."));
		return false;
	}

	// 6) Transient Texture 생성
	NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	if (!NewTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateTexture2DFromPngBytes: CreateTransient failed."));
		return false;
	}

#if WITH_EDITORONLY_DATA
	NewTexture->CompressionSettings = TC_Default;
	NewTexture->SRGB = true;
#endif

	// 7) Mip0에 RawData 복사
	FTexture2DMipMap& Mip = NewTexture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_WRITE);

	const int32 ExpectedDataSize = Width * Height * 4; // BGRA
	if (RawData.Num() != ExpectedDataSize)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("CreateTexture2DFromPngBytes: RawData.Num=%d, Expected=%d"),
			RawData.Num(), ExpectedDataSize);
	}

	FMemory::Memcpy(TextureData, RawData.GetData(), FMath::Min(RawData.Num(), ExpectedDataSize));
	Mip.BulkData.Unlock();

	// 8) GPU 업데이트
	NewTexture->UpdateResource();

	return true;
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
