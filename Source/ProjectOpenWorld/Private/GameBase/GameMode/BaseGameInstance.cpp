#include "GameBase/GameMode/BaseGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UBaseGameInstance::LoadLevelStatic(UObject* pWorldContext)
{
	if (!pWorldContext || !pWorldContext->GetWorld())
		return;
	UWorld* World = pWorldContext->GetWorld();
	UBaseGameInstance* Instance = Cast<UBaseGameInstance>(World->GetGameInstance());
	if (!Instance || Instance->m_pTargetLevel.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetLevel is Nullptr"));
		UKismetSystemLibrary::QuitGame(World, nullptr, EQuitPreference::Type::Quit, false);
		return;
	}

	Instance->m_nCount = 0;
	FLoadPackageAsyncDelegate LoadedDelegate{};
	LoadedDelegate.BindUFunction(Instance, FName(TEXT("OpenLoadedLevel")));
	LoadPackageAsync(FPackageName::ObjectPathToPackageName(Instance->m_pTargetLevel.ToString()), MoveTemp(LoadedDelegate));
}

void UBaseGameInstance::OpenLevelStatic(UObject* pWorldContext, TSoftObjectPtr<UWorld> newLevel)
{
	if (newLevel.IsNull() || !pWorldContext || !pWorldContext->GetWorld())
		return;
	UWorld* World = pWorldContext->GetWorld();
	UBaseGameInstance* Instance = Cast<UBaseGameInstance>(World->GetGameInstance());
	if (!Instance)
		return;
	Instance->m_pTargetLevel = newLevel;
	UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(World, World->GetCurrentLevel(), FLatentActionInfo{}, false);
	UGameplayStatics::OpenLevel(World, TEXT("Loading"));
}

void UBaseGameInstance::OpenLoadedLevel()
{
	UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(GetWorld(), GetWorld()->GetCurrentLevel(), FLatentActionInfo{}, false);
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), m_pTargetLevel);
	m_pTargetLevel = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("OpenLoadedLevel %d"), m_nCount);
	m_nCount++;
}

void UBaseGameInstance::ProgressFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("Load %d"), m_nCount);
	m_nCount++;
}
