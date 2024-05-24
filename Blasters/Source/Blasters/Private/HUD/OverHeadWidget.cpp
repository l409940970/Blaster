// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverHeadWidget.h"
#include "Components/TextBlock.h"

void UOverHeadWidget::SetDisplayText(FString Text)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(Text));
	}
}

void UOverHeadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	//通过获取本地和远程，在客户端和服务端上两次是不同的结果

	//显示本地
	ENetRole LocalRole = InPawn->GetLocalRole();
	// 
	//显示远程
	//ENetRole RemoteRole = InPawn->GetRemoteRole();


	FString Role;

	switch (LocalRole)
	{
	case ROLE_None:
		Role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ROLE_Authority:
		Role = FString("Authority");
		break;
	case ROLE_MAX:
		Role = FString("Max");
		break;
	}

	FString LocalRoleString = FString::Printf(TEXT("Local Role: %s"), *Role);

	//FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);

	SetDisplayText(LocalRoleString);

}

void UOverHeadWidget::NativeDestruct()
{
	RemoveFromParent();

	Super::NativeDestruct();
}
 