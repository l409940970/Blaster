// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairInterface.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTERS_API ABlasterCharacter : public ACharacter,public IInteractWithCrosshairInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//��ȡ�������ڸ��Ƶ���
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//ActorComponent��ʼ��ɺ���õĺ���
	virtual void PostInitializeComponents() override;

	void PlayFireMontage(bool bAiming);

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();


	

private:
	UPROPERTY(VisibleAnywhere,Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere,Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverHeadWidget;

	//һ���ɱ����縴�Ƶ���������
	//UPROPERTY(Replicated)
	//ReplicatedUsing ��ʾ���ñ����ڷ������ϱ仯ʱ,��������Ȱ󶨺õĻص�����(������һ����OnRep��ͷ),�ص�����Ҫ��UFUNCTION���
	//�������������������ã���Ϊ�����Ǵӷ��������ͻ��ˣ����ܴӿͻ��˵�������
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	//����洢��lastWeapon����һ�ε�ֵ
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);


	//ս�����
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	//ʹ��RPC,�ɿ�����
	//�����Ķ������Ҫ���� _Implementation
	UFUNCTION(Server,Reliable)
	void Server_EquipButtonPressed();

	//��׼ƫ��
	void AimOffset(float DeltaTime);
	void TurnInPlace(float DeltaTime);

	//��׼ƫ��
	float AO_Yaw;
	float AO_Pitch;
	float InterpAO_Yaw;

	FRotator StartAimRotator;

	ETurningInPlace TurningInPlace;

	//������̫��
	UPROPERTY(EditAnywhere,Category = "Combat")
	class UAnimMontage* FireWeaponMontage;


public:	
	//FORCEINLINE �ؼ��ֵ������Ǹ��߱�����������Ҫǿ�ƽ���������,һ�������߼��򵥵ĺ���
	//FORCEINLINE void SetOverlappingWeapon(AWeapon* Weapon) { OverlappingWeapon = Weapon; }
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAO_Yaw() const{ return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	AWeapon* GetEquippedWeapon();

	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

	FVector GetHitTarget() const;

	float GetFollowCameraFOV() const;
	void SetFollowCameraFOV(float FOV) const;
};
