#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalActionWidgetComponent.generated.h"

class UUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalActionWidgetComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "ActionWidget")
	TSubclassOf< UUserWidget> ActionWidgetClass{};
	UPROPERTY(VisibleAnywhere, Category = "ActionWidget")
	TObjectPtr< UUserWidget> ActionWidget{};

public:
	void CreateActionWidget();
	void DeleteActionWidget();
	UUserWidget* GetActionWidget() const { return ActionWidget; }
};
