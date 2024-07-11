// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/BlasterHUD.h"
#include "CombatComponent.generated.h"


#define TRACE_LENGTH 80000.f



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTERS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABlasterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	//��ȡ�������ڸ��Ƶ��ߣ�������Ҫ����ͬ���ı���
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;


protected:
	virtual void BeginPlay() override;

	void SetAiming(bool isAim);

	//ʹ��RPC��server���ã�
	UFUNCTION(Server,Reliable)
	void Server_SetAiming(bool isAim);
	
	UFUNCTION()
	void OnRep_EquippedWeapon();
	void FireButtonPressed(bool bPressed);
	void Fire();

	//�������Ͽ���
	UFUNCTION(Server,Reliable)
	void Server_Fire(const FVector_NetQuantize& TraceHitTarget);

	//�����鲥  Ҫ��_Implementation  �ڷ����������пͻ��˶���ִ��
	UFUNCTION(NetMulticast,Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	//����Ļ������Ϊ׼��
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	//����׼��
	void SetHUDCrosshairs(float DeltaTime);

private:
	class ABlasterCharacter* Character;
	class ABlasterPlayerController* Controller;
	class ABlasterHUD* HUD;

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

	//׼�����߼�⵽��λ��
	FVector HitTarget;

	FHUDPackage HUDPackage;

	//��ɫ���ܲ�ʱ׼����ɢ��ֵ
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootFactor;

	//����׼ʱ��Ĭ��FOV
	float DefaultFOV;
	float CurFOV;
	UPROPERTY(EditAnywhere)
	float ZoomFOV = 30.f;
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	//�Զ�fire
	FTimerHandle FireTimerHandle;
	bool bCanFire = true;
	void StartFireTimer();
	void FireTimerFinishied();

	bool CanFire();

public:	
	void EquipWeapon(AWeapon* WeaponToEquip);
		
};
