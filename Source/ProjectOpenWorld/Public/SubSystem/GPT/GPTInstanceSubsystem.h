#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GPTInstanceSubsystem.generated.h"

class UVaRestSubsystem;
class UVaRestRequestJSON;
enum class EVaRestRequestVerb : uint8;
enum class EVaRestRequestContentType : uint8;
enum class EVaJson : uint8;
class UTexture2D;
class UVaRestJsonValue;

USTRUCT(BlueprintType)
struct FGPTRequest
{
	GENERATED_BODY()
	FGPTRequest() = default;
	virtual~FGPTRequest() = default;
	virtual bool CheckSendable() const { return false; }
};

USTRUCT(BlueprintType)
struct FGPTStringRequest : public FGPTRequest
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GPTRequest")
	FString Frompt{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GPTRequest")
	FString Text{};

	virtual bool CheckSendable() const override;
};

USTRUCT(BlueprintType)
struct FGPTImageRequest : public FGPTRequest
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GPTRequest")
	FString Text{};

	virtual bool CheckSendable() const override;
};

UCLASS()
class PROJECTOPENWORLD_API UGPTInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
protected:
	UVaRestSubsystem* VaRestSubsystem{};
	// 서버에서 관리가 필요하지만 생략
	FString APIKey = TEXT("YOUR_API_KEY_HERE");

	enum class EResponseType
	{
		NONE,
		RESPONSEID,
		IMAGE,
		TEXT,
		TOTALTOKEN,
	};
private:
	UVaRestRequestJSON* GetRequest(EVaRestRequestVerb verb, EVaRestRequestContentType contentType, UObject* ResponseTarget);
	UVaRestJsonValue* GetJsonValue(UVaRestRequestJSON* Request, const TArray<FString>& FieldPath, EVaJson& Type) const;
	bool CheckSendable() ;
	UVaRestJsonValue* GetJsonValue(UVaRestJsonValue* JsonValue, const FString* ArrayFields, int Count) const;
	TArray<FString> GetResponsePath(EResponseType type) const;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	void SendGPTStringRequest(FGPTStringRequest RequestData, UObject* Target);
	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	void SendGPTImageRequest(FGPTImageRequest RequestData,UObject* Target);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	void SendGetGPTResponse(FString modelID, UObject* Target);


	UFUNCTION(BlueprintPure, Category = "VaRest|Utility")
	FString  GetResponseString(UVaRestRequestJSON* Request) const;
	UFUNCTION(BlueprintPure, Category = "VaRest|Utility")
	FString  GetRequestID(UVaRestRequestJSON* Request) const;
	UFUNCTION(BlueprintPure, Category = "VaRest|Utility")
	UTexture2D* GetResponseTexture(UVaRestRequestJSON* Request) const;
	UFUNCTION(BlueprintPure, Category = "VaRest|Utility")
	int32 GetResponseTotalTogens(UVaRestRequestJSON* Request) const;

};