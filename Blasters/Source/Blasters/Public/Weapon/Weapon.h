// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlasterTypes/WeaponTypes.h"
#include "Weapon.generated.h"


//武器状态
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};



UCLASS()
class BLASTERS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	virtual void Tick(float DeltaTime) override;
	//显示拾取ui
	void ShowPickupWidget(bool bShowWidget);

	//获取生命周期复制道具
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;

	virtual void Fire(const FVector& HitTarget);

	void Dropped();
	//添加子弹
	void AddAmmo(int32 AmmoToAdd);

	//十字准星ui
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;

	//自动开火
	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = 0.15f;
	UPROPERTY(EditAnywhere, Category = Combat)
	bool bAutomatic = true;

	//拾取音效
	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;

	void SetHUDAmmo();


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere,Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere,Category = "Weapon Properties")
	class USphereComponent* AreaSpere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	//Weaponstate属性改变后的调用函数（客户端执行）
	UFUNCTION()
	void OnRep_WeaponState();

	//拾取ui
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickUpWidget;
	//开火动画
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TSubclassOf<class ACasing> CasingClass;

	//瞄准时的fov
	UPROPERTY(EditAnywhere)
	float ZoomFOV= 30.f;
	UPROPERTY(EditAnywhere)
	//fov切换速度
	float ZoomInterSpeed = 20.f;

	//武器当前的子弹数
	UPROPERTY(EditAnywhere,ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;
	//弹夹最大容量
	UPROPERTY(EditAnywhere, meta = (ToolTip = "弹夹最大容量"))
	int32 MagCapacity;

	UFUNCTION()
	void OnRep_Ammo();
	//子弹减少
	void SpendRound();

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerPlayerController;

	//武器
	UPROPERTY(EditAnywhere)
	EWeaponTyps WeaponType;



public:		

	void SetWeaponState(EWeaponState State);
	USphereComponent* GetAreaSphere()const;
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const{ return ZoomFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterSpeed; }
	bool IsEmpty();
	FORCEINLINE EWeaponTyps GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
};
