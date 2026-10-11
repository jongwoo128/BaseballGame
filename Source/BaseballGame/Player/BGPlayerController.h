// BGPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BGPlayerController.generated.h"

class UBGChatInput;
class UUserWidget;

/**
 * 
 */
UCLASS()
class BASEBALLGAME_API ABGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABGPlayerController();

	virtual void BeginPlay() override;
	// Ctrl + . 을 눌러서 Create Declaration으로 cpp 파일에 정의부를 만들 수 있도록 생성해준다. 자주 사용하자.
	// ChatMessageString을 셋팅을 하기 위한 함수. 캐싱된 채팅값을 인자로 받음.
	void SetChatMessageString(const FString& InChatMessageString);
	
	// 우리가 친 채팅을 Printing 해주는 함수
	void PrintChatMessageString(const FString& InChatMessageString);
	// Client RPC로 채팅 메세지가 날아가지 않고 신뢰성이 있게끔 하기위해 Reliable로 키워드를 넣어준다.
	// Client RPC를 통해서 서버에 다시 출력이 되게끔 해줌.
	// Client RPC라는 것을 알기 용이하도록 함수명 맨 앞에 ClientRPC인지 ServerRPC인지 붙여줌.
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);
	// Server RPC 채팅 메세지가 날아가지 않고 신뢰성이 있게끔 하기위해 Reliable로 키워드를 넣어준다.
	// Server에 보내기 위해서, 그리고 서버쪽에 먼저 보내기 위해서 Server RPC를 먼저 써야함.
	// Server RPC의 경우 With Validation도 추가할 수 있음으로 나중 cpp의 내용에 Validate()함수도 적어줘야 함.
	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBGChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UBGChatInput> ChatInputWidgetInstance;

	FString ChatMessageString; // 우리가 쓴 채팅을 ChatMessageString에 캐싱.

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;

};
