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
	//�����Ƿ��ܽ���ģ�������¼�
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
//1 ���Σ�
//
//10~35mm:��ǣ��ʺ���ͽǶ����ģ����ȣ�һ���������ȫ�����߰�������������÷ŵ����ģ���Ȼ���л��䡣��ǲ�Ҫ��������������д�����Ե�ͷ����ü�һЩǰ����
//{
//	24mm : �ʺ������޵����ڡ�����Ƚϴ�Ҳ���׻��䣬�����鹹ͼ
//	35mm�� ����˫�۹ۿ�Ч��
//}
//
//40~60mm����׼   ��õ���50�Ľ��Σ����ɹ��˿��ء���ӽ����ۣ������ӽǻ�Ƚ�ƽ�������ĳ������ӽǡ���������ȫ����ý��ڱ�Ե��ͷ�����հ�
//{
//	50mm���ӽ�����Ч�� ����С
//}
//
//85~135  �г���  85��ã�����֮��������д �н��� �����黯�������㱳��һ�������ߵ������壬Ҳ����������󳡾�
//{
// 85mm�� ����������������ľ��� ����������ϸ�� ���������Ե�����
//}
//
//����Խ�󣬱���Խ�黯
//
//͸�ӣ�����ԶС���뽹���޹أ�ֻ������й�
//
//��ǰȷ������
//����д������������  50mm����
//
//��ȫ���󾰣�ȫ��  50mm����
//
//������λ��ͼ�����ñ佹����ͼ
//
//2 ������ñ����黯  ���Խ������ С���Խ�黯��
//{
//	��Ȧ�� ��ȦԽ�� ����ԽС������Խ�黯    ����Ϊ���۾���  �����۾����൱����С��Ȧ������������Խ������Ҳ���ǹ�ȦԽС������Խ����������Խ��
//	���ࣺ ����Խ�� ����ԽС������Խ�黯    ����Ϊ�۾��� �۾�����Խ�����൱�ڽ���Խ�̣�����������Խ������Ҳ���ǽ���Խ�̣�����Խ����������Խ��
//	��ࣺ ���Ҳ�̣�����ԽС������Խ�黯   ����Ϊ����ͷ�����ʱ������Ľ��㶼�������ϣ������ͻ��黯����Ҳ�������ԽԶ������Խ����������Խ��
//}
//{
//	Ҫ�����黯Ч����С��������Ȧ ������ ����� ���� �� ��
//}
//{
//���ã�
//	����������Χ
//	�۽�
//	���������С���� �ʺϷ�Χ��
//}
//
//3 ����
//{
//	Զ����   ���ڷ�Χ�У������ɾ� ���ַ���������ھŹ��񽻵���
//	ȫ����   ����պ�չʾȫ�� ͷ���ͽ���Ҫ���¿ռ�
//	�о���	 ϥ������  ��Ҫ���ڹؽ�λ��   �����ϥ�����ϣ��������£�ϥ�����ϣ�
//	�н����� �������� 
//	������   �ز����ϣ�
//	��д��   �粿����
//	����д�� ȫ����
//}


