// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTERS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;


protected:
	virtual void BeginPlay() override;

	//�ӵ�����
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulsem, const FHitResult& Hit);


	UPROPERTY(EditAnywhere)
	float Damage = 10.f;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	//������Ч
	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	class UParticleSystemComponent* TracerComponent;

	//������Ч
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;
	//������Ч
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	bool isHitDestroy ;

	UPROPERTY(EditAnywhere)
	float SpawnTime = 3.f;

public:	


};
