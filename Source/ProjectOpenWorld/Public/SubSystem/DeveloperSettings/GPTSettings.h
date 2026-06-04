#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GPTSettings.generated.h"

enum class EVaJson : uint8;

UENUM(BlueprintType)
enum class EResponseType : uint8
{
	RESPONSEID,
	IMAGE,
	TOTALTOKEN
};

USTRUCT()
struct FGPTResponsePath
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Config, Category = "JsonHeader")
	TArray<FString> Keys;
};

UCLASS(Blueprintable, editinlinenew, Abstract, Category = "Field")
class UGPTValue : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Field")
	FString Header{};
	UPROPERTY(EditAnywhere, Category = "Field")
	EVaJson ValueType{};
};

UCLASS()
class UGPTValue_String : public UGPTValue
{
	GENERATED_BODY()
public:
	UGPTValue_String();
	UPROPERTY(EditAnywhere, Category = "Field")
	FString Value{};
};

UCLASS()
class UGPTValue_Array : public UGPTValue
{
	GENERATED_BODY()
public:
	UGPTValue_Array();
	UPROPERTY(EditAnywhere, Category = "Field")
	TArray<FString> ValueArray{};
};

UCLASS()
class UGPTValue_Number: public UGPTValue
{
	GENERATED_BODY()
public:
	UGPTValue_Number();
	UPROPERTY(EditAnywhere, Category = "Field")
	int32 Value{};
};

UCLASS()
class UGPTValue_Object: public UGPTValue
{
	GENERATED_BODY()
public:
	UGPTValue_Object();
	UPROPERTY(EditAnywhere, Instanced, Category = "Field")
	TObjectPtr<UGPTValue> Value{};
};


USTRUCT()
struct FGPTField
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString Header{};
	UPROPERTY(EditAnywhere)
	EVaJson ValueType{};
	UPROPERTY(EditAnywhere)
	FString Value{};
	UPROPERTY(EditAnywhere)
	TArray< FString> ValueArray{};
};

USTRUCT()
struct FGPTImagePostData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString RequestURL{};
	UPROPERTY(EditAnywhere)
	TArray< FGPTField> PostFields{};
	UPROPERTY(EditAnywhere)
	FString PromptHeader{};
	UPROPERTY(EditAnywhere)
	TArray< FGPTField> PromptFields{};
};

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "GPT Settings"))
class PROJECTOPENWORLD_API UGPTSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UGPTSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(EditAnywhere, Config, Category = "Response")
	TMap<EResponseType, FGPTResponsePath> ResponsePaths{};

	// 서버에서 관리가 필요하지만 생략
	UPROPERTY(EditAnywhere, Config, Category = "POST")
	FGPTField GPTKey{};

	UPROPERTY(EditAnywhere, Config, Category = "POST|Image")
	FGPTImagePostData ImagePostData{};
public:
	static TArray<FString> GetResponsePath(EResponseType Type);
	static FGPTImagePostData GetImagePostData();
	static FGPTField GetGPTKey();
};
