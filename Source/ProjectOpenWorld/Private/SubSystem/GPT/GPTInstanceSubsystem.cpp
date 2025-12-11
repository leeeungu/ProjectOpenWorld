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
	FString FullResponse = Request->GetResponseContentAsString();
	int32 ResultKeyIndex = FullResponse.Find(TEXT("\"result\""), ESearchCase::IgnoreCase, ESearchDir::FromStart);
	if (ResultKeyIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("ExtractResultBase64: 'result' not found."));
		return TArray<uint8>();
	}

	// 2) ':' 이후 문자열 위치 찾기
	int32 ColonIndex = FullResponse.Find(TEXT(":"), ESearchCase::IgnoreCase, ESearchDir::FromStart, ResultKeyIndex);
	if (ColonIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("ExtractResultBase64: ':' after 'result' not found."));
		return TArray<uint8>();
	}

	// 3) 첫 번째 따옴표(") 위치 찾기
	int32 FirstQuoteIndex = FullResponse.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, ColonIndex);
	if (FirstQuoteIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("ExtractResultBase64: opening quote not found."));
		return TArray<uint8>();
	}

	// 4) 두 번째 따옴표(") 위치 찾기
	int32 SecondQuoteIndex = FullResponse.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, FirstQuoteIndex + 1);
	if (SecondQuoteIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("ExtractResultBase64: closing quote not found."));
		return TArray<uint8>();
	}

	// 5) base64 substring 추출
	FString Base64PNG = FullResponse.Mid(FirstQuoteIndex + 1, SecondQuoteIndex - (FirstQuoteIndex + 1));
	Base64PNG = Base64PNG.TrimStartAndEnd();
	if (Base64PNG.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("ProcessOpenAIImageResponse: Base64PNG is empty."));
		return TArray<uint8>();
	}

	// 2) Base64 → Byte Array(PNG)
	TArray<uint8> PngBytes;
	if (!FBase64::Decode(Base64PNG, PngBytes))
	{
		UE_LOG(LogTemp, Error, TEXT("ProcessOpenAIImageResponse: Base64 decode failed."));
		return TArray<uint8>();
	}
	return PngBytes;
}

int UGPTInstanceSubsystem::GetTotalTogens(UVaRestRequestJSON* Request) const
{
	UVaRestJsonValue* Value = VaRestSubsystem->DecodeJsonValue(Request->GetResponseContentAsString());
	if (Value && Value->GetType() == EVaJson::Object)
	{
		UVaRestJsonObject* Response = Value->AsObject();
		if (Response)
		{
			UVaRestJsonValue* val = Response->GetField("usage");
			if (val && val->GetType() == EVaJson::Object)
			{
				UVaRestJsonObject* togenObj = val->AsObject();
				UE_LOG(LogTemp, Warning, TEXT("total_tokens : %d"), togenObj->GetIntegerField(TEXT("total_tokens")));
			}
		}
	}
	return 0;
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


			RequestObject->SetIntegerField(TEXT("max_output_tokens"), 100);
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
	FString result = Request->GetResponseContentAsString();
	result.Split("\"text\":", nullptr, &result);
	result.Split("\n", &result, nullptr);
	result = result.Mid(2, result.Len() - 4);
	//UE_LOG(LogTemp, Warning, TEXT("GPT Response: %d"), result.Len());

	TArray<UVaRestJsonValue*> array = GetResponseArrayField(Request, TEXT("output"));
	GetTotalTogens(Request);
	if (array.IsValidIndex(0))
	{
		UVaRestJsonObject* content = array[0]->AsObject();
		if (content)
		{
			UE_LOG(LogTemp, Log, TEXT("Fields: %d"), content->GetFieldNames().Num());
			return content->GetStringField("text");
		}
	}
	return "";
}

bool UGPTInstanceSubsystem::SaveImageData(UVaRestRequestJSON* Request, FString Path)
{
	TArray<uint8> PngBytes = GetPNGData(Request);
	UVaRestJsonValue* Value = VaRestSubsystem->ConstructJsonValueString(Request->GetResponseContentAsString());
	// 3) PNG 파일로 저장 (선택)
	if (!PngBytes.IsEmpty())
	{
		FString SavePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("OpenAI_Image.png"));
		FFileHelper::SaveArrayToFile(PngBytes, *SavePath);
		UE_LOG(LogTemp, Log, TEXT("Saved PNG at: %s"), *SavePath);
		UE_LOG(LogTemp, Log, TEXT("Texture updated from OpenAI PNG Base64."));
		return true;
	}
	return false;

}

bool UGPTInstanceSubsystem::ApplyImageData(UVaRestRequestJSON* Request, UTexture2D*& NewTexture)
{
	TArray<uint8> PngData = GetPNGData(Request);
	if (PngData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateTexture2DFromPngBytes: PngData is empty."));
		return false;
	}

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
