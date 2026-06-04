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
class IGPTResponseInterface;
enum class EResponseType : uint8;
struct FGPTField;
class UVaRestJsonObject;
struct FGPTPostPrompt;

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
	FString prompt{};
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


USTRUCT(BlueprintType)
struct FGPTRequestResult : public FGPTRequest
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GPTRequest")
	FString RequestID{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GPTRequest")
	FString Text{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GPTRequest")
	TObjectPtr<UTexture2D>  Image{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GPTRequest")
	int32 ToggenCount{};
};

UCLASS()
class PROJECTOPENWORLD_API UGPTInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
protected:
	TObjectPtr<UVaRestSubsystem> VaRestSubsystem{};
private:
	UVaRestRequestJSON* GetRequest(EVaRestRequestVerb verb, EVaRestRequestContentType contentType, UObject* ResponseTarget,		const TArray< FGPTField>* PostFields = nullptr);
	UVaRestJsonValue* GetJsonValue(UVaRestRequestJSON* Request, const TArray<FString>& FieldPath, EVaJson& Type) const;
	bool CheckSendable() ;
	UVaRestJsonValue* GetJsonValue(UVaRestJsonValue* JsonValue, const FString* ArrayFields, int Count) const;
	void SetRequestField(UVaRestJsonObject* RequestRootObject , const TArray< FGPTField>& Fields);


public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	void SendGPTImageRequest(FGPTImageRequest RequestData, TScriptInterface< IGPTResponseInterface> Target);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	void SendGetGPTResponse(FString modelID, TScriptInterface< IGPTResponseInterface> Target);

	UFUNCTION(BlueprintPure, Category = "VaRest|Utility")
	FString  GetRequestID(UVaRestRequestJSON* Request) const;
	UFUNCTION(BlueprintPure, Category = "VaRest|Utility")
	UTexture2D* GetResponseTexture(UVaRestRequestJSON* Request) const;
	UFUNCTION(BlueprintPure, Category = "VaRest|Utility")
	int32 GetResponseTotalTogens(UVaRestRequestJSON* Request) const;

};