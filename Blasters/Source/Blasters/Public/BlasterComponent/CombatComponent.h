// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/BlasterHUD.h"
#include "BlasterTypes/WeaponTypes.h"
#include "BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"


#define TRACE_LENGTH 80000.f



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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
    UFUNCTION(Server, Reliable)
    void Server_SetAiming(bool isAim);

    UFUNCTION()
    void OnRep_EquippedWeapon();
    void FireButtonPressed(bool bPressed);
    void Fire();
    void Reload();
    //在动画蓝图中调用，通过蒙太奇消息
    UFUNCTION(BlueprintCallable)
    void FinishReloading();
    UFUNCTION(Server, Reliable)
    void Server_Reload();
    void HandReload();
    int32 AmmoutToReload();


    //服务器上开火
    UFUNCTION(Server, Reliable)
    void Server_Fire(const FVector_NetQuantize& TraceHitTarget);




    //网络组播  要加_Implementation  在服务器和所有客户端都会执行
    UFUNCTION(NetMulticast, Reliable)
    void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

    //以屏幕中心作为准星
    void TraceUnderCrosshairs(FHitResult& TraceHitResult);

    //绘制准星
    void SetHUDCrosshairs(float DeltaTime);

private:
    class ABlasterCharacter* Character;
    class ABlasterPlayerController* Controller;
    class ABlasterHUD* HUD;

    UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
    class AWeapon* EquippedWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "MoveMent")
    FName SocketName = "RightHandSocket";

    UPROPERTY(Replicated)
    bool bAiming;

    UPROPERTY(EditAnywhere, Category = "MoveMent")
    float BaseWalkSpeed;
    UPROPERTY(EditAnywhere, Category = "MoveMent")
    float AimWaklSpeed;

    bool bFireButtonPressed;

    //准星射线检测到的位置
    FVector HitTarget;

    FHUDPackage HUDPackage;

    //角色在跑步时准星扩散的值
    float CrosshairVelocityFactor;
    float CrosshairInAirFactor;
    float CrosshairAimFactor;
    float CrosshairShootFactor;

    //不喵准时的默认FOV
    float DefaultFOV;
    float CurFOV;
    UPROPERTY(EditAnywhere)
    float ZoomFOV = 30.f;
    UPROPERTY(EditAnywhere)
    float ZoomInterpSpeed = 20.f;

    void InterpFOV(float DeltaTime);

    //自动fire
    FTimerHandle FireTimerHandle;
    bool bCanFire = true;
    void StartFireTimer();
    void FireTimerFinishied();

    bool CanFire();

    //角色当前的子弹数
    UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
    int32 CarriedAmmo;
    UFUNCTION()
    void OnRep_CarriedAmmo();
    TMap<EWeaponTyps, int32> CarriedAmmoMap;
    //每种类型的武器，角色初始所携带的子弹数
    UPROPERTY(EditAnywhere)
    int32 StartingARAmmo = 30;

    void InitialzeCarriedAmmo();


    //组件状态
    UPROPERTY(ReplicatedUsing = OnRep_CombatState)
    ECombatState CombatState = ECombatState::ECS_Unoccupied;
    UFUNCTION()
    void OnRep_CombatState();

public:
    void EquipWeapon(AWeapon* WeaponToEquip);

    void UpdataAmmoValues();

};
