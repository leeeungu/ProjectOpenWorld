#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

class UWeaponeWidget;
enum class EWeapone : uint8;
class UPlayerStatusProgress;
class UStatComponent;
class UPalHpWidget_Boss;
class UMonsterHpLayer;
class UPlayerPalSpawnerWidget;
class UPalStorageComponent;

UCLASS()
class PROJECTOPENWORLD_API UMainUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Weapone")
	TObjectPtr<UWeaponeWidget> WBP_WeaponUI{};
	UPROPERTY(meta = (BindWidget), Category = "Status", EditDefaultsOnly)
	TObjectPtr<UPlayerStatusProgress> PlayerHPBar{};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr < UMonsterHpLayer> MonsterHpLayer{};
	//UPROPERTY(meta = (BindWidgetOptional), Category = "Status", EditDefaultsOnly)
	TObjectPtr<UPlayerStatusProgress> PlayerHealthBar{};
	//UPROPERTY(meta = (BindWidgetOptional), Category = "Status", EditDefaultsOnly)
	TObjectPtr<UPlayerStatusProgress> PlayerShieldBar{};

	UPROPERTY(meta = (BindWidget), Category = "Spawner", VisibleAnywhere)
	TObjectPtr<UPlayerPalSpawnerWidget> PlayerPalSpawnerWidget{};
	

public:
	void ChangeWeapone(FName NewWeaponeID, EWeapone NewWeaponeType);
	void SetPlayerStatWidget(UStatComponent* StatCom);
	void SetPalStorageComponent(UPalStorageComponent* InStorage);
	void RotateSelection(int32 Direction);
	int32 GetSelectedPal() const;
	void RegisterMonster(AActor* Target);
	void UnregisterMonster(AActor* Target);
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
};
