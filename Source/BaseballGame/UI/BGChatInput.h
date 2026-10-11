// BGChatInput.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BGChatInput.generated.h"


class UEditableTextBox;
/**
 * 
 */
UCLASS()
class BASEBALLGAME_API UBGChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	
public:
	UPROPERTY(meta = (BindWidget))
	//BindWidget이란 meta 키워드가 붙은 UPROPERTY는 EditableTextBox_ChatInput와 이름이 똑같은 UEditableTextBox가 존재하게끔 자식 블루프린트에게 강제함.
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;
	//언리얼에서  BGChatInput을 상속받는 WBP_ChatInput을 만들고 Hierarchy(계층구조)에 EditableTextBox_ChatInput라는 이름의 EditableTextBox가 없으면 언리얼 내에서 컴파일 안됨.
};
