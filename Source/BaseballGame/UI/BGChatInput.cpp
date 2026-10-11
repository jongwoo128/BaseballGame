// BGChatInput.cpp


#include "UI/BGChatInput.h"

#include "Components/EditableTextBox.h"
#include "Player/BGPlayerController.h"

void UBGChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
		// BGChatInput.h에서 EditableTextBox_ChatInput라는 이름을 가진 UEditableTextBox는 언리얼 속 WBP_ChatInput의 EditableTextBox_ChatInput과 바인딩되어 있음.
		// 그래서 EditableTextBox가 가지고 있는 이벤트 중 하나인 "OnTextCommitted(채팅을 치고 Enter를 쳤을 때 확정 이벤트)"와 델리게이트를 통해서 OnChatInputTextCommitted와 바인드되어 있는지 확인하는 if문.
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
		// 연결이 안되어 있다면 EditableTextBox가 가지고 있는 이벤트 중 하나인 "OnTextCommitted(채팅을 치고 Enter를 쳤을 때 확정 이벤트)"와 델리게이트를 통해서 OnChatInputTextCommitted와 바인드 하기 위한 코드
	}
}

void UBGChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
		// NativeConstruct와 반대로 NativeDestruct는 바인드가 안되어 있는지 확인하는 if문.
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
		// 연결이 되어 있다면 EditableTextBox가 가지고 있는 이벤트 중 하나인 "OnTextCommitted"와 델리게이트를 통한 "OnChatInputTextCommitted"와의 바인드 해제하기 위한 코드
	}
}

void UBGChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	// Enter를 누르는 등의 방법으로 Text를 제출을 할 경우의 함수
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer(); // GetOwningPlayer로부터 Enter을 사용한 PlayerController를 가져올 수 있게 됨.
		if (IsValid(OwningPlayerController) == true) // 가져온 OnwingPlayerController가 유효한지 확인.
		{
			ABGPlayerController* OwningBGPlayerController = Cast<ABGPlayerController>(OwningPlayerController);
			// 유효하다면 ABGPlayerController를 Cast하여 OwningPlayerController로 객체 생성.
			if (IsValid(OwningBGPlayerController) == true) // 생성한 OwningPlayerController가 유효한지 확인.
			{
				OwningBGPlayerController->SetChatMessageString(Text.ToString());
				// 유효하다면 OwningBGPlayerController로부터 SetChatMessageString() 함수를 호출 하여서 EditableTextBox_ChatInput에 있는 Text를 SetText()로 FText()와 같이 빈공간으로 만들어 달라.
				// SetChatMessageString()은 이전 BGPlayerController.cpp에 만들어 놓은 Text 대입 함수.
				EditableTextBox_ChatInput->SetText(FText());
			}
		}
	}
}
