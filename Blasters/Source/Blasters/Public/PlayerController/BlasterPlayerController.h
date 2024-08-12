// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERS_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void ReceivedPlayer() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	virtual void BeginPlay() override;

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	//倒计时
	void SetHUDAnnouncementCountdown(float CountdownTime);
	virtual float GetServerTime();
	void OnMatchStateSet(FName State);
	void PollInit();


protected:

	UFUNCTION(Server,Reliable)
	void Server_RequestServerTime(float TimeOfClientRequest);
	UFUNCTION(Client,Reliable)
	void Client_ReportServerTime(float TimeOfClientRequest,float TimeServerReceivedClientRequest);
	UFUNCTION(Server,Reliable)
	void Server_CheckMatchState();
	//玩家中途加入game
	UFUNCTION(Client,Reliable)
	void Client_JoinMidgame(FName StateOfMatch,float Warmup,float Match,float StartingTime,float CoolTime);

	//设置游戏时间
	void SetHUDTime();
	//游戏状态开始
	void HandleMatchHasStarted();
	//游戏冷却期
	void HandleCooldown();

private:
	UFUNCTION()
	void OnRep_MatchState();

	bool IsHUDVaild();
	void CheckTimeSync(float DeltaTime);



protected:
	UPROPERTY()
	class ABlasterHUD* BlasterHUD;
	//client和server多久同步一次时间
	UPROPERTY(EditAnywhere,Category = Time)
	float TimeSyncFrequency = 5.f;

	//客户端到服务器的延迟时间
	float ClientServerDelta = 0.f;
	float TimeSyncRunningTime = 0.f;

private:

	//游戏状态
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;

	//比赛时间
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float LevelStartingTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;
	bool bInitializeCharacterOverlay = false;
	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;


};
