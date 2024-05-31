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

	//获取生命周期复制道具
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//ActorComponent初始完成后调用的函数
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

	//瞄准偏移
	void AimOffset(float DeltaTime);
	

private:
	UPROPERTY(VisibleAnywhere,Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere,Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverHeadWidget;

	//一个可被网络复制的武器变量
	//UPROPERTY(Replicated)
	//ReplicatedUsing 表示当该变量在服务器上变化时,会调用事先绑定好的回调函数(函数名一般以OnRep开头),回调函数要用UFUNCTION标记
	//这个函数服务器不会调用，因为复制是从服务器到客户端，不能从客户端到服务器
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	//这个存储的lastWeapon是上一次的值
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);


	//战斗组件
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	//使用RPC,可靠传输
	//函数的定义后面要加上 _Implementation
	UFUNCTION(Server,Reliable)
	void Server_EquipButtonPressed();


	//瞄准偏移
	float AO_Yaw;
	float AO_Pitch;
	FRotator StartAimRotator;

public:	
	//FORCEINLINE 关键字的作用是告诉编译器，它需要强制将函数内联,一般用于逻辑简单的函数
	//FORCEINLINE void SetOverlappingWeapon(AWeapon* Weapon) { OverlappingWeapon = Weapon; }
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAO_Yaw() const{ return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	AWeapon* GetEquippedWeapon();
};
