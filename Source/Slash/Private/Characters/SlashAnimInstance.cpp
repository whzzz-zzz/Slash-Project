// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"     //访问角色移动组件的函数跟变量
#include"Kismet/KismetMathLibrary.h"        //kismet数学库头文件  静态库

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();    //因为是重写函数，调用父类确保父版本里的操作都能执行

	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());
	if (SlashCharacter)
	{
		SlashCharacterMovement = SlashCharacter->GetCharacterMovement();
	}

}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)     //类似于Tick
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SlashCharacterMovement && SlashCharacter)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity);  //获取移动速度，使用Kismet里的静态函数去掉Z轴的分量传给GroundSpeed

		IsFalling = SlashCharacterMovement->IsFalling();

		CharacterState = SlashCharacter->GetCharacterState();
		ActionState = SlashCharacter->GetActionState();
		DeathPose = SlashCharacter->GetDeathPose();
	}

}
