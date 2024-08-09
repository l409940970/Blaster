// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 PlayerNum = GameState.Get()->PlayerArray.Num();
	if (PlayerNum == 1)
	{
		if (GetWorld())
		{
			//���л���ͼʱ�����Ҫ���������������״̬�����ֵҪ��Ϊtrue
			bUseSeamlessTravel = true;
			GetWorld()->ServerTravel(FString("/Game/Map/BlasterMap?listen"));
		}
	}
}
