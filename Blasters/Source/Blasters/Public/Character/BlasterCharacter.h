// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTERS_API ABlasterCharacter : public ACharacter
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

	//��׼ƫ��
	void AimOffset(float DeltaTime);
	

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
	float AO_Yaw;
	float AO_Pitch;
	FRotator StartAimRotator;

public:	
	//FORCEINLINE �ؼ��ֵ������Ǹ��߱�����������Ҫǿ�ƽ���������,һ�������߼��򵥵ĺ���
	//FORCEINLINE void SetOverlappingWeapon(AWeapon* Weapon) { OverlappingWeapon = Weapon; }
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAO_Yaw() const{ return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	AWeapon* GetEquippedWeapon();
};
