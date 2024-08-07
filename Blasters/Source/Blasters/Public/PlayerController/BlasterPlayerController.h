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
	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
protected:

	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	virtual void BeginPlay() override;
	void SetHUDTime();

	UFUNCTION(Server,Reliable)
	void Server_RequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client,Reliable)
	void Client_ReportServerTime(float TimeOfClientRequest,float TimeServerReceivedClientRequest);

	//客户端到服务器的延迟时间
	float ClientServerDelta = 0.f;
	//
	UPROPERTY(EditAnywhere,Category = Time)
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTime = 0.f;




private:
	bool IsHUDVaild();

	//比赛时间
	float MatchTime = 120.f;
	uint32 CountdownInt = 0;


	void CheckTimeSync(float DeltaTime);
};
