#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TransportInterface.generated.h"

UINTERFACE(MinimalAPI)
class UTransportInterface : public UInterface
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class ETransportState : uint8
{
	NotTransport,
	Transport
};

class PROJECTOPENWORLD_API ITransportInterface // ex item etc
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Transport")
	void OnTransportRegister(AActor* Other);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Transport")
	void OnTransportUnRegister(AActor* Other);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Transport")
	void OnTransportEnd(AActor* Other);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Transport")
	void OnTransportCancel(AActor* Other);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Transport")
	ETransportState GetTransportState();
};


/*
	virtual void OnTransportRegister_Implementation(AActor* Other) override;
	virtual void OnTransportUnRegister_Implementation(AActor* Other) override;
	virtual void OnTransportEnd_Implementation(AActor* Other) override;
	virtual void OnTransportCancel_Implementation(AActor* Other) override;
	virtual ETransportState GetTransportState_Implementation() override;
*/