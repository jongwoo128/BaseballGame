// BGGameModeBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BGGameModeBase.generated.h"


class ABGPlayerController;
/**
 * 
 */
UCLASS()
class BASEBALLGAME_API ABGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// LogIn 후의 함수
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);
	// 시도에 대한 채점 함수
	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(ABGPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ABGPlayerController* InChattingPlayerController);

	void ResetGame();
	// 게임이 진행되도 되는지 판단 함수
	void JudgeGame(ABGPlayerController* InChattingPlayerController, int InStrikeCount);


protected:
	// 만들어진 정답을 캐싱(저장)
	FString SecretNumberString;

	// 참가자들을 저장
	TArray<TObjectPtr<ABGPlayerController>> AllPlayerControllers;

};
