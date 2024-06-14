// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class BLASTERS_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	ACasing();

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;


	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulsem, const FHitResult& Hit);


private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CasingMesh;

	//���ǳ���
	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse = 15.f;

	//�����Ч
	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;


public:	

};
