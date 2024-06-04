// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTERS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABlasterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	//获取生命周期复制道具，设置需要属性同步的变量
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;


protected:
	virtual void BeginPlay() override;

	void SetAiming(bool isAim);

	//使用RPC（server调用）
	UFUNCTION(Server,Reliable)
	void Server_SetAiming(bool isAim);
	
	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	class ABlasterCharacter* Character;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly,Category = "MoveMent")
	FName SocketName = "RightHandSocket";

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere, Category = "MoveMent")
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere, Category = "MoveMent")
	float AimWaklSpeed;

	bool bFireButtonPressed;

public:	
	void EquipWeapon(AWeapon* WeaponToEquip);
	void FireButtonPressed(bool bPressed);
		
};
