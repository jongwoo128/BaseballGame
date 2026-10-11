// BGGameModeBase.cpp


#include "Game/BGGameModeBase.h"

#include "BGGameStateBase.h"
#include "Player/BGPlayerController.h"
#include "EngineUtils.h"
#include "Player/BGPlayerState.h"

void ABGGameModeBase::OnPostLogin(AController* NewPlayer)
{
	
	Super::OnPostLogin(NewPlayer);

	// 들어온 PlayerController를 형변화.
	ABGPlayerController* BGPlayerController = Cast<ABGPlayerController>(NewPlayer);
	// 유효할 경우 아래 실행
	if (IsValid(BGPlayerController) == true)
	{
		//Server에 있는 PlayerController에 있는 NotificationText를 수정함에 있어서 Replication이 되어 Client쪽에 복제되어 보여질 것임.
		BGPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		// 유효할 시 참가한 Player들을 저장
		AllPlayerControllers.Add(BGPlayerController);

		// 가져와진 PlayerController를 통해 PlayerState를 가져와 줌.
		ABGPlayerState* BGPS = BGPlayerController->GetPlayerState<ABGPlayerState>();
		//유효할시 아래 실해
		if (IsValid(BGPS) == true)
		{
			// 가져와진 PlayerState에 있는 NameString을 Player라는 TEXT + FromInt()를 통한 저장된 참가자 수에 대한 넘버링 부여.
			// 이 과정에서 Server에 있는 PlayerState의 특정 속성을 변경하는 것이기 때문에 Replication이 될 것임.
			BGPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		//NewPlayer가 접속할 때마다 GameStateBase를 가져옴
		ABGGameStateBase* BGGameStateBase = GetGameState<ABGGameStateBase>();
		// 유효할 경우 아래 내용 실행
		if (IsValid(BGGameStateBase) == true)
		{
			// 위에서 저장된 Player의 넘버링이 붙여져 Login 안내가 Client들에게 보여짐.
			BGGameStateBase->MulticastRPCBroadcastLoginMessage(BGPS->PlayerNameString);
		}
	}
}

// 정답이 될 번호 생성 함수.
FString ABGGameModeBase::GenerateSecretNumber()
{
	// 중복되지 않는 숫자 3개를 랜덤하게 뽑아 FString으로 Result가 될 값을 String으로 반환해줌.
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

// 정답을 유추하기 위한 시도 번호 함수
bool ABGGameModeBase::IsGuessNumberString(const FString& InNumberString)
{

	bool bCanPlay = false;

	do {
		//자릿수가 3개 인가? 아니면 break;
		if (InNumberString.Len() != 3)
		{
			break;
		}

		// 숫자가 아닌 문자가 포함되어 있는가?
		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		// 중복된 숫자가 있는가?
		if (bIsUnique == false || UniqueDigits.Num() != 3)
		{
			break;
		}

		bCanPlay = true;

	} while (false);
	
	// 위 상황이 맞을 경우 진행이 가능함.
	return bCanPlay;
}

// 정답 번호와 유추 시도 번호와의 판단 함수
FString ABGGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		// 정답과 유추 번호가 숫자와 자리가 동시에 일치한가?
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			// 위치는 다르지만 포함이 되어있는가?
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	// 위의 사항 둘 다 적용되지 않았을 경우 OUT.
	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	// 몇개의 Strike와 Ball이 있는지 반환.
	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ABGGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	// 만들어진 정답 번호 저장
	SecretNumberString = GenerateSecretNumber();
}

// Client에서 Server RPC인 PrintChatMessageString()를 호출하여 사용하도록 할 것임.
void ABGGameModeBase::PrintChatMessageString(ABGPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	if (!IsValid(InChattingPlayerController))
	{
		return;
	}

	ABGPlayerState* BGPS = InChattingPlayerController->GetPlayerState<ABGPlayerState>();
	
	if (!IsValid(BGPS))
	{
		return;
	}

	FString GuessNumberString = InChatMessageString;

	// 유추하기 위한 번호 3자리가 맞는지에 대한 유효성 검사하여 true일 경우 판단 로직 실행.
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		// PlayerState의 CurrentGuessCount가 MaxGuessCount 보다 크거나 같으면 특정 Text로 시도 불가 안내.
		if (BGPS->CurrentGuessCount >= BGPS->MaxGuessCount)
		{
			InChattingPlayerController->ClientRPCPrintChatMessageString(
				TEXT("시도 기회를 모두 사용했습니다. 다른 플레이어의 결과를 기다려주세요.")
			);

			return;
		}

		// 모든 플레이어의 이전 안내 문구 제거
		for (const auto& BGPlayerController : AllPlayerControllers)
		{
			if (IsValid(BGPlayerController))
			{
				BGPlayerController->NotificationText = FText::GetEmpty();
			}
		}

		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		// 유추 시도에 유효할 경우 시도 횟수 카운팅 함수 실행
		IncreaseGuessCount(InChattingPlayerController);

		FString CombinedMessageString = BGPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString + TEXT(" -> ") + JudgeResultString;


		for (TActorIterator<ABGPlayerController> It(GetWorld()); It; ++It)
		{
			// 유추한 채팅에 대해서 모든 Client들을 순회하여 결과 보고
			ABGPlayerController* BGPlayerController = *It;

			if (IsValid(BGPlayerController) == true)
			{
				BGPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}

		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));

		JudgeGame(InChattingPlayerController, StrikeCount);

	}
	else
	{
		InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("1~9의 중복 없는 숫자 3자리를 입력해주세요. 시도 기회는 차감되지 않습니다."));

		FString CombinedMessageString = BGPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

		// 유추 번호가 아닐 경우 일반 채팅처럼 처리
		// TActorIterator(It)를 사용하기 위해서 EngineUtils.h를 Include해줘야 함.
		for (TActorIterator<ABGPlayerController> It(GetWorld()); It; ++It)
		{
			// Iterator를 통해 가져온 Server에 존재하는 여러 PlayerController를 통해 유효한지 확인 후 Client RPC를 호출하여 해당 PC에서
			// ClientRPCPrintChatMessageString(InChatMessageString)를 호출. 
			ABGPlayerController* BGPlayerController = *It;

			if (IsValid(BGPlayerController) == true)
			{
				BGPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}
	}
}

// 시도 횟수에 대한 카운팅 등가 함수.
void ABGGameModeBase::IncreaseGuessCount(ABGPlayerController* InChattingPlayerController)
{
	ABGPlayerState* BGPS = InChattingPlayerController->GetPlayerState<ABGPlayerState>();
	if (IsValid(BGPS) == true)
	{
		BGPS->CurrentGuessCount++;
	}
}

void ABGGameModeBase::ResetGame()
{
	// Reset시 정답 새로 만들어서 저장
	SecretNumberString = GenerateSecretNumber();

	// 모든 Player들을 순회하여 현재 시도 횟수 초기화.
	for (const auto& BGPlayerController : AllPlayerControllers)
	{
		ABGPlayerState* BGPS = BGPlayerController->GetPlayerState<ABGPlayerState>();
		if (IsValid(BGPS) == true)
		{
			BGPS->CurrentGuessCount = 0;
		}
	}
}

void ABGGameModeBase::JudgeGame(ABGPlayerController* InChattingPlayerController, int InStrikeCount)
{
	// InStrikeCount가 3인 PlayerController가 존재할 경우 해당 플레이어가 승리했음을 NotificationText를 통해 알림.
	if (3 == InStrikeCount)
	{
		ABGPlayerState* BGPS = InChattingPlayerController->GetPlayerState<ABGPlayerState>();

		for (const auto& BGPlayerController : AllPlayerControllers)
		{
			if (IsValid(BGPS) == true)
			{
				FString CombinedMessageString = BGPS->PlayerNameString + TEXT(" has won the game.");

				BGPlayerController->NotificationText = FText::FromString(CombinedMessageString);
			}
		}

		ResetGame();

	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BGPlayerController : AllPlayerControllers)
		{
			ABGPlayerState* BGPS = BGPlayerController->GetPlayerState<ABGPlayerState>();
			if (IsValid(BGPS) == true)
			{
				if (BGPS->CurrentGuessCount < BGPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& BGPlayerController : AllPlayerControllers)
			{
				BGPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));
			}

			ResetGame();

		}
	}
}
