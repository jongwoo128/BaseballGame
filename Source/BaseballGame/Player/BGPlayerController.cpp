// BGPlayerController.cpp


#include "Player/BGPlayerController.h"
#include "UI/BGChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include <BaseballGame.h>
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BGGameModeBase.h"
#include "BGPlayerState.h"
#include "Net/UnrealNetwork.h"

ABGPlayerController::ABGPlayerController()
{
	bReplicates = true;
}

void ABGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		// 멀티 플레이 도중 다른 플레이어가 죽었을 경우의 UI 화면이 나의 화면에도 동시에 노출되거나 혹은 반대의 상황이 생기지 않게 하기 위해 위젯 객체는 Owning Client에서만 생성하면 됨.
		// 현재 실행 중인 환경에서 포컬 플레이어의 컨트롤러인지 확인하기 위해 IsLocalController() 함수로 본인의 LocalController에 해당 되지 않는 상황일 경우,
		// 이후 UI 생성 로직이 싱행하지 않도록 return으로 종료하도록 하는 코드.
		return;
	}

	FInputModeUIOnly InputModeUIOnly; // InputMode를 UI쪽으로만 흘러가게 할 것인가? -> 채팅만 띄울 것이기 때문에 FInputModeUIOnly로 InputModeUIOnly를 만들어서
	SetInputMode(InputModeUIOnly); // SetInputMode에 InputModeUIOnly로 Setting함.


	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UBGChatInput>(this, ChatInputWidgetClass);
		// PlayController.h에 TsubclassOf를 넣어 주었는데, 이 설정이 되어있다면 그것을 CreateWidget으로 만들어주고 그것이 유효 하다면 AddToViewport하라.
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport(); // 만든 채팅창이 Viewport에 보이기 위한 함수. 블루프린트의 AddToViewport와 동일. 단지 ()만 추가 되었을 뿐.
		}
	}

	// 만들은 NotificationTextWidgetClass를 내화면에 띄우는 함수
	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ABGPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString; // 들어온 채팅 메세지를 대입.

	// OwningClient인지 검사하여 true일 경우 ServerRPC를 호출하여 Server에서 ServerRPCPrintChatMessageString(InChatMessageString)가 실행되도록 함.
	if (IsLocalController() == true)
	{

		ServerRPCPrintChatMessageString(InChatMessageString);

	}
}

void ABGPlayerController::PrintChatMessageString(const FString& InChatMessageString) 
{
	// UKismetSystemLibrary::PrintString(this, ChatMessageString, true, true, FLinearColor::Red, 5.0f);
	// (앞 순서대로)실행 중인 월드의 어느객체?,우리가 친 채팅을 캐싱한 ChatMessageString(정확히는 출력할 문자열 InString), 화면에 출력할지의 여부(bool형), Output Log에 출력할지 여부,
	// 화면에 표시할 글자 색상, 화면에 표시할 시간의 순으로 인자를 넣어 문자열을 출력해주는 PrintString()함수를 사용.

	// 내가 만든 PrintString()함수로 들어온 String 문자열 Print하기.
	BassballGameFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ABGPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

// RPC를 만들때는 함수명 뒤에 _Implementation를 뒤에 붙여줘야 함. (하나의 문법으로 이해할 것.)
void ABGPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	// 아래에서 호출했으니 여기서 실행.
	PrintChatMessageString(InChatMessageString);
}

void ABGPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	// GameModeBase를 얻어와 그것이 유효한지 확인. 유효할시 아래 내용 실행
	if (IsValid(GM) == true)
	{
		// 유효할 경우 ABGGameModeBase의 형태로 형변화
		ABGGameModeBase* BGGM = Cast<ABGGameModeBase>(GM);
		// 유효한지 확인 후 유효할 경우 GameModeBase 내에 구현되어 있는 PrintChatMessageString()을 실행.
		if (IsValid(BGGM) == true)
		{
			BGGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}
