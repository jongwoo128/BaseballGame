// BassballGame.h

#pragma once

#include "CoreMinimal.h"

class BassballGameFunctionLibrary
{
public:
	// 정적 함수의 개념으로 MyPrintString함수를 만들주기.
	static void MyPrintString(const AActor* InWorldContextActor, const FString& InString, float InTimeToDisplay = 1.f, FColor InColor = FColor::Cyan)
	{
		if (IsValid(GEngine) == true && IsValid(InWorldContextActor) == true)
		// GEngine이라는 개체가 유효한지? 그리고 월드 단위마다 NetMode가 존재하며, 그 NetMode를 알아내기 위해서는 그 월드를 알아야한다.
		// 그래서 World를 알기위해 World를 내가 특정할 수 있을 만한 월드 객체를 넘겨 받기 위한 InWorldContextActor의 유효성을 체크.
		// InWorldContextActor에는 world객체를 넘겨줘도 되고, 그 World에 속해 있는 액터를 넘겨줘도 됨.
		{
			if (InWorldContextActor->GetNetMode() == NM_Client || InWorldContextActor->GetNetMode() == NM_ListenServer)
			// 위에서 넘겨 받은 InWorldContextActor의 GetNetMode()를 통해 NetMode를 알아낼 수 있으며,
			// 알아낸 NetMode가 Client나 ListenSever 중 하나 일 경우에 속할 경우. 
			{
				GEngine->AddOnScreenDebugMessage(-1, InTimeToDisplay, InColor, InString);
				// UKismetSystemLibrary::PrintString()의 안에 존재하는 화면에 메세지를 띄워주는 AddOnScreenDebugMessage()만을 활용해서
				// 나만의 PrintString을 만들기 위해 AddOnScreenDebugMessage()만 가져와 사용.
				// 원래의 PrintString()에는 내가 쓴 메세지 말고도 이것저것 사족이 붙어서 그것을 제외하고 쓰고 싶어서 개별로 만드는 과정임.
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("%s"), *InString);
				// 만일 NetMode가 Client와 ListenSever가 둘 다 아닐 경우 DedicatedServer일테니 UE_LOG를 활용함.
				// 왜냐하면 DedicatedServer에는 Player이 없어 CMD창 만을 사용하기에 Viewport가 필요로 하지 않기 때문.
			}
		}
	}

	static FString GetNetModeString(const AActor* InWorldContextActor)
	// 똑같이 정적 함수를 사용하는데 이유는 Class의 객체가 없어도 이 함수를 호출할 수 있게끔 하기 위함임.
	{
		FString NetModeString = TEXT("None");

		if (IsValid(InWorldContextActor) == true)
		{
			// InWorldContextActor를 통해 얻은 NetMode에 따라서 어떤 모드인지 TEXT로 알려주기 위한 코드
			ENetMode NetMode = InWorldContextActor->GetNetMode();
			if (NetMode == NM_Client)
			{
				NetModeString = TEXT("Client");
			}
			else
			{
				if (NetMode == NM_Standalone)
				{
					NetModeString = TEXT("StandAlone");
				}
				else
				{
					NetModeString = TEXT("Server");
				}
			}
		}

		return NetModeString;
	}

	static FString GetRoleString(const AActor* InActor)
	{
		// 해당 함수에 액터를 넣어줬을 때 그 액터가 유효하다면 LocalRoleString을 알 수 있도록 하나 코드(디버깅용).
		FString RoleString = TEXT("None");

		if (IsValid(InActor) == true)
		{
			//Enum값을 스트링으로 바꿔주는 함수에 InActor의 GetLocalRole값을 인자로 받았을 때 스트링으로 바꿔서 알려줌 
			FString LocalRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InActor->GetLocalRole());
			//동일하게 InActor의 GetRemoteRole값을 인자로 받았을 때 스트링으로 바꿔서 알려줌 
			FString RemoteRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InActor->GetRemoteRole());

			RoleString = FString::Printf(TEXT("%s / %s"), *LocalRoleString, *RemoteRoleString);
		}

		return RoleString;
	}

};