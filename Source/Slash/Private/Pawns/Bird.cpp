// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include"Components/CapsuleComponent.h"   // 胶囊体头文件
#include"Components/SkeletalMeshComponent.h"    // 骨骼网格体头文件
#include "GameFramework/SpringArmComponent.h"    // 弹簧臂头文件
#include "Camera/CameraComponent.h"     // 摄像机头文件


// Sets default values
ABird::ABird()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);  
	SetRootComponent(Capsule);  // 用Item那边的用法替换也行
	
	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(GetRootComponent());    // 将骨骼网格组件附加到根组件上
	// 后面还有一个默认参数是连接到插槽上，默认为没连

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);


	AutoPossessPlayer = EAutoReceiveInput::Player0;    // 设置控制器
}

// Called every frame
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//将前进函数绑定到了前进轴的映射上
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward);

	PlayerInputComponent->BindAxis(FName("Turn"), this, &ABird::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ABird::LookUp);




}

// Called when the game starts or when spawned
void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABird::MoveForward(float Value)
{

	if (Controller && (Value != 0.f))
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);
	} 

}

void ABird::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABird::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}
