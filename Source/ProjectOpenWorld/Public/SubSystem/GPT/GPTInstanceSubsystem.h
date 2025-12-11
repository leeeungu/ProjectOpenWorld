#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GPTInstanceSubsystem.generated.h"

class UVaRestSubsystem;
class UVaRestRequestJSON;
enum class EVaRestRequestVerb : uint8;
enum class EVaRestRequestContentType : uint8;
class UTexture2D;
class UVaRestJsonValue;

USTRUCT(BlueprintType)
struct FGPTRequest
{
	GENERATED_BODY()
	FGPTRequest() = default;
	~FGPTRequest() = default;
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

private:
	UVaRestRequestJSON* GetRequest(EVaRestRequestVerb verb, EVaRestRequestContentType contentType, UObject* ResponseTarget);
	bool CheckSendable() ;
	TArray<uint8> GetPNGData(UVaRestRequestJSON* Request);
	int GetTotalTogens(UVaRestRequestJSON* Request) const ;
	TArray<UVaRestJsonValue*> GetResponseArrayField(UVaRestRequestJSON* Request, const FString& FieldName);
	FString GetResponseStringField(UVaRestRequestJSON* Request, const FString& FieldName);
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	void SendGPTStringRequest(FGPTStringRequest RequestData, UObject* Target);
	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	void SendGPTImageRequest(FGPTImageRequest RequestData,UObject* Target);
	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	FString GetStringFromRequestJSON(UVaRestRequestJSON* Request);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	bool SaveImageData(UVaRestRequestJSON* Request, FString Path);
	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	bool ApplyImageData(UVaRestRequestJSON* Request, UTexture2D*& Texture);
};