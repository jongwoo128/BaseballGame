// BGGameStateBase.cpp


#include "Game/BGGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/BGPlayerController.h"

void ABGGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	// 서버가 아닐 경우 아래 내용을 실행
	if (HasAuthority() == false)
	{
		// Client의 GetPlayerController()를 통해 World에 있는 0번에 해당하는 Controller를 가져올 경우 자기꺼 하나 밖에 없으니 LocalController를 자연스럽게 가져옴.
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		// 그 Controller가 유효할 시, 아래 내용을 실행
		if (IsValid(PC) == true)
		{
			// 형변화를 시켜줌.
			ABGPlayerController* BGPC = Cast<ABGPlayerController>(PC);
			// 유효화 검사
			if (IsValid(BGPC) == true)
			{
				// InNameString에 해당하는 사람이 접속했다는 String이 PrintChatMessageString(NotificationString)를 통해 출력.
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				BGPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
