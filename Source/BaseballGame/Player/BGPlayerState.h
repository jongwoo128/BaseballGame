// ABGPlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BGPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BASEBALLGAME_API ABGPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABGPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// player의 시도 횟수 출력을 위한 함수
	FString GetPlayerInfoString();

public:
	//네트워크로 복제할 액터의 속성에 Replicated 키워드 추가하여 지정.
	// Player 넘버링
	UPROPERTY(Replicated)
	FString PlayerNameString;
	// 현재 시도 횟수
	UPROPERTY(Replicated)
	int32 CurrentGuessCount;
	//최대 시도 가능한 횟수
	UPROPERTY(Replicated)
	int32 MaxGuessCount;
	
};
