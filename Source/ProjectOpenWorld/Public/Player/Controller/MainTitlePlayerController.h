#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainTitlePlayerController.generated.h"

UCLASS()
class PROJECTOPENWORLD_API AMainTitlePlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MainTitlePlayerController")
	TSubclassOf<UUserWidget> MainTitleWidgetClass{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MainTitlePlayerController")
	TObjectPtr<UUserWidget> MainTitleWidget{};

	virtual void BeginPlay() override;
	
};
