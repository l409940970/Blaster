// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERS_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ABlasterGameMode();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
public:
	//玩家淘汰
	virtual void PlayerEliminated(class ABlasterCharacter* ElimmedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	//重生
	virtual void RequestRespawn(ACharacter* ElimCharacter, AController* ElimmedController);

public:
	//开始游戏倒计时
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;
	//游戏时长
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	float LevelStartingTime = 0.f;

private:
	float CountdownTime = 0.f;

};
