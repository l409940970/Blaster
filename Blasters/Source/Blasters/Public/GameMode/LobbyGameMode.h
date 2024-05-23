// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERS_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	//µÇÂ½·þÎñÆ÷
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
