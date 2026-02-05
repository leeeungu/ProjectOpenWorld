#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PatternWidget_Anubis.generated.h"

class UHorizontalBox;
class UTexture2D;
class USizeBox;

UCLASS()
class PROJECTOPENWORLD_API UPatternWidget_Anubis : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "Pattern", meta = (BindWidget))
	TObjectPtr<	UHorizontalBox> PatternBox{};
	UPROPERTY(VisibleAnywhere, Category = "Pattern", meta = (BindWidget))
	TObjectPtr<	USizeBox> PatternSizeBox{};
	
	UPROPERTY(EditAnywhere, Category = "Pattern")
	TObjectPtr<UTexture2D> PatternImage{};
	UPROPERTY(EditAnywhere, Category = "Pattern")
	TObjectPtr<UTexture2D> SucceedImage{};
	int nCount{};
public:
	void SetPatternImage(int32 Count);

	void UpdatePatternImage();
	
};
