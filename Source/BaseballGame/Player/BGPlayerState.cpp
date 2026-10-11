// ABGPlayerState.cpp

#include "Player/BGPlayerState.h"
#include "BGPlayerState.h"
#include "Net/UnrealNetwork.h"

ABGPlayerState::ABGPlayerState()
	: PlayerNameString(TEXT("None"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)
{
	// 복제가 가능하도록 bReplicates를 true로 함.
	bReplicates = true;
}

void ABGPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}

FString ABGPlayerState::GetPlayerInfoString()
{
	// player의 넘버링과 현대 시도횟수, 최대 가능 횟수에 대한 PlayerInfoString를 반환
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}
