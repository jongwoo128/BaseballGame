// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BGPawn.h"
#include "BaseballGame.h"


// Called when the game starts or when spawned
void ABGPawn::BeginPlay()
{
	Super::BeginPlay();
	// BeginPlay()가 호출될 경우 GetRoleString()을 통해 Role을 가져와 NetMode와 Combined하고 출력하는 코드
	// 이 단계에서는 아직 빙의 작업을 통한 SetOwner가 안되어 있기 때문에 SimulatedProxy로 로그가 나옴.
	FString NetRoleString = BassballGameFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::BeginPlay() %s [%s]"), *BassballGameFunctionLibrary::GetNetModeString(this), *NetRoleString);
	BassballGameFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ABGPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Super::PossessedBy()가 호출될 경우 GetRoleString()을 통해 Role을 가져와 NetMode와 Combined하고 출력하는 코드
	// 빙의를 통한 SetOwner가 되었기 때문에 이 단계에서는 AutonomousProxy로 로그가 나옴.
	FString NetRoleString = BassballGameFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::PossessedBy() %s [%s]"), *BassballGameFunctionLibrary::GetNetModeString(this), *NetRoleString);
	BassballGameFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}



