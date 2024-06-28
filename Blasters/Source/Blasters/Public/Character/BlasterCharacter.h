// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairInterface.h"
#include "Components/TimelineComponent.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTERS_API ABlasterCharacter : public ACharacter,public IInteractWithCrosshairInterface
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

	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Hit();

	//底层是用于同步角色的移动，这里用来同步角色的转向，因为SimProxiesTurn是tick执行的，但是在同步过程中不能没帧都同步过去，会导致问题
	virtual void OnRep_ReplicatedMovement() override;


	//淘汰
	void Elim();
	UFUNCTION(NetMulticast,Reliable)
	void Multicast_Elim();

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


	
	//applydamage回调函数
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();

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
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	float CalculateSpeed();
	void TurnInPlace(float DeltaTime);
	//模拟代理端的玩家转向逻辑
	void SimProxiesTurn();

	//瞄准偏移
	float AO_Yaw;
	float AO_Pitch;
	float InterpAO_Yaw;

	FRotator StartAimRotator;

	ETurningInPlace TurningInPlace;

	//开火蒙太奇
	UPROPERTY(EditAnywhere,Category = "Combat")
	class UAnimMontage* FireWeaponMontage;
	//受击蒙太奇
	UPROPERTY(EditAnywhere,Category = Combat)
	UAnimMontage* HitReactMontage;
	//死亡蒙太奇
	UPROPERTY(EditAnywhere,Category = Combat)
	UAnimMontage* ElimMontage;

	//当相机与角色之间的位置低于这个值时，隐藏角色模型，用于角色卡墙体
	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraThreshold = 200.f;
	void HideCameraIfCharacterClose();

	void PlayHitMontage();
	//是否旋转根骨骼
	bool bRotateRootBone;
	//模拟端用于转向的值
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	//用于同步转向的周期
	float TimeSinceLastMovementReplication;


	//生命
	UPROPERTY(EditAnywhere, Category = "Player Status", meta = (ClampMin = "0"));
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere,Category = "Player Status")
	float Health = MaxHealth;

	UFUNCTION()
	void OnRep_Health();

	class ABlasterPlayerController* BlasterPlayerController;

	bool bIsElimed = false;
	FTimerHandle ElimTimerHandle;
	UPROPERTY(EditAnywhere)
	float ElimDelay = 3.f;
	void ElimTimerFinishied();

	//溶解
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	//timeline委托句柄
	FOnTimelineFloat DissolveTrack;
	//timeline回调函数
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissove();
	//溶解的时间曲线
	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;
	//溶解的动态材质实例
	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;
	//角色死亡时将材质修改为溶解材质
	UPROPERTY(EditAnywhere)
	UMaterialInstance* DissolveMaterialInstance;


public:	
	//FORCEINLINE 关键字的作用是告诉编译器，它需要强制将函数内联,一般用于逻辑简单的函数
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

	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bIsElimed; }
};
