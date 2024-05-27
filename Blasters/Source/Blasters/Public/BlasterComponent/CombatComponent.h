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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;


protected:
	virtual void BeginPlay() override;

	void SetAiming(bool isAim);
	//ʹ��RPC
	UFUNCTION(Server,Reliable)
	void Server_SetAiming(bool isAim);

private:
	class ABlasterCharacter* Character;

	UPROPERTY(Replicated)
	class AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly)
	FName SocketName = "RightHandSocket";

	UPROPERTY(Replicated)
	bool bAiming;

public:	
	void EquipWeapon(AWeapon* WeaponToEquip);
		
};
