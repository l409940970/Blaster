// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);

	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	//物体是否能进行模拟命中事件
	CasingMesh->SetNotifyRigidBodyCollision(true);
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
	CasingMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
}

void ACasing::Destroyed()
{
	Super::Destroyed();
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulsem, const FHitResult& Hit)
{
	Destroy();

}
//
//
//1 焦段：
//
//10~35mm:广角：适合与低角度仰拍，大长腿，一般可用于拍全身，或者半身，但是人物最好放到中心，不然会有畸变。广角不要用于拍摄人物特写。会显得头大（最好加一些前景）
//{
//	24mm : 适合在有限的室内。景别比较大，也容易畸变，，考验构图
//	35mm： 符合双眼观看效果
//}
//
//40~60mm：标准   最好的是50的焦段，进可攻退可守。最接近人眼，但是视角会比较平淡，能拍出男友视角。，，，拍全身最好脚在边缘，头顶留空白
//{
//	50mm：接近单眼效果 景别小
//}
//
//85~135  中长焦  85最好（人像之王），特写 中近景 背景虚化，在拍摄背景一棵树或者单个物体，也可用于拍摄大场景
//{
// 85mm： 能拉进背景和人物的距离 ，合适拍摄细节 但是容易显得脸大
//}
//
//焦段越大，背景越虚化
//
//透视：近大远小，与焦段无关，只和物距有关
//
//提前确定焦段
//拍特写，近景，半身  50mm以上
//
//拍全身，大景，全景  50mm以下
//
//多用走位构图，少用变焦来构图
//
//2 景深：能让背景虚化  大景深（越清晰） 小景深（越虚化）
//{
//	光圈： 光圈越大 景深越小，背景越虚化    想象为眯眼睛，  眯着眼睛就相当于缩小光圈。看到的物体越清晰，也就是光圈越小，背景越清晰（景深越大）
//	焦距： 焦距越长 景深越小，背景越虚化    想象为眼镜， 眼镜戴的越近就相当于焦距越短，看到的物体越清晰，也就是焦距越短，背景越清晰（景深越大）
//	摄距： 摄距也短，景深越小，背景越虚化   想象为当镜头怼脸的时候，相机的焦点都在人脸上，背景就会虚化掉，也就是相机越远，背景越清晰（景深越大）
//}
//{
//	要背景虚化效果（小景深）：大光圈 长焦距 相机近 （大 长 今）
//}
//{
//作用：
//	控制清晰范围
//	聚焦
//	表达情绪：小景深 适合氛围感
//}
//
//3 景别
//{
//	远景：   用于氛围感，背景干净 三分法，人物放在九宫格交点上
//	全景：   人物刚好展示全身， 头顶和脚下要留下空间
//	中景：	 膝盖以上  不要卡在关节位置   最好在膝盖以上（大腿往下，膝盖往上）
//	中近景： 腰部以上 
//	近景：   胸部以上，
//	特写：   肩部以上
//	大特写： 全是脸
//}


