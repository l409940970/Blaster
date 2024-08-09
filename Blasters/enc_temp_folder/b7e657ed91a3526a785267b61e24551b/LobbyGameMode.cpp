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
			//在切换地图时，如果要与服务器保持连接状态，这个值要设为true
			bUseSeamlessTravel = true;
			GetWorld()->ServerTravel(FString("/Game/Map/BlasterMap?listen"));
		}
	}
}
