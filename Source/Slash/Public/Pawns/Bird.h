// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bird.generated.h"          // 这个必须放在最后

class UCapsuleComponent;   // 前向声明：UCapsule不在这个文件中，但可以在别处找到
class USkeletatalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class SLASH_API ABird : public APawn
{
	GENERATED_BODY()

public:

	// Sets default values for this pawn's properties
	ABird();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//映射
	void MoveForward(float Value);
	void Turn(float Value);
	void LookUp(float Value);

private:
	UPROPERTY(VisibleAnywhere)
    UCapsuleComponent* Capsule;   

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;


	//弹簧臂与摄像机
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;      // 有时候用SpringArm作为变量名会出现在蓝图中无详细信息的bug，改个名字即可

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;


}; 
