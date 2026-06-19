#pragma once
#include "CoreMinimal.h"
#include "Building/Actor/BuildingActor.h"
#include "SubSystem/GPT/GPTResponseInterface.h"
#include "PalGroupBanner.generated.h"

class UVaRestRequestJSON;
class UTexture2D;

UCLASS()
class PROJECTOPENWORLD_API APalGroupBanner : public ABuildingActor, public IGPTResponseInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticker")
	TSubclassOf<UUserWidget> WidgetClass{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticker")
	TObjectPtr< UUserWidget> Widget{};

public:
	// 프롬프트를 GPT 이미지 엔드포인트로 전송(리스너=this).
	// 깃발 입력 위젯이 입력 텍스트로 이 함수만 호출하면 됨.
	UFUNCTION(BlueprintCallable, Category = "TeamFlag")
	void SendFlagRequest(const FString& Prompt);

	// IGPTResponseInterface
	virtual void OnRequestComplete_Implementation(UVaRestRequestJSON* Request) override;
	virtual void OnRequestFail_Implementation(UVaRestRequestJSON* Request) override;


	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override;
protected:
	// 응답 텍스처를 깃발 메시 머티리얼에 주입.
	// ※ 메시/파라미터는 프로젝트 셋업에 맞게 조정 — 은구님 담당 영역.
	void SetFlagImage(UTexture2D* pImage);
};