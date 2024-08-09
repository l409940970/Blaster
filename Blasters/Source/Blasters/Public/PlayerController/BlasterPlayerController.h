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
	//����ʱ
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
	//�����;����game
	UFUNCTION(Client,Reliable)
	void Client_JoinMidgame(FName StateOfMatch,float Warmup,float Match,float StartingTime);

	//������Ϸʱ��
	void SetHUDTime();
	//��Ϸ״̬��ʼ
	void HandleMatchHasStarted();

private:
	UFUNCTION()
	void OnRep_MatchState();

	bool IsHUDVaild();
	void CheckTimeSync(float DeltaTime);



protected:
	UPROPERTY()
	class ABlasterHUD* BlasterHUD;
	//client��server���ͬ��һ��ʱ��
	UPROPERTY(EditAnywhere,Category = Time)
	float TimeSyncFrequency = 5.f;

	//�ͻ��˵����������ӳ�ʱ��
	float ClientServerDelta = 0.f;
	float TimeSyncRunningTime = 0.f;

private:

	//��Ϸ״̬
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	//����ʱ��
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float LevelStartingTime = 0.f;
	uint32 CountdownInt = 0;
	bool bInitializeCharacterOverlay = false;
	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;


};
