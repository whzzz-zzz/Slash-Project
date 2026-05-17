// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include"Components/SphereComponent.h"
#include"Characters/SlashCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

//定义一个绘制球体的宏(已将该宏移到DebugMacros.h中)
//#define DRAW_SPHERE(Location) if(GetWorld())DrawDebugSphere(GetWorld(),Location,25.f,12,FColor::Red,true);


// Sets default values
AItem::AItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));    //创建默认子对象
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;      //指向根组件的默认场景被替换然后自动删除

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
	//Sphere 预设
	Sphere->SetGenerateOverlapEvents(true);       // 开启重叠事件
	Sphere->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 碰撞预设：重叠所有动态物体
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 仅查询，无物理碰撞，不挡角色

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	ItemEffect->SetupAttachment(GetRootComponent());

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), DeltaTime);
	if (GEngine)
	{
		FString Name=GetName();   //GetName用于初始化，初始化为该物品在UE5的名称
		//格式化字符串FString  用于输出
		//FString Message = FString::Printf(TEXT("DeltaTime: %f"), DeltaTime);
		//FString 类似于string，用%s，同时传入时*用于重载为c风格的字符串（即为字符数组），返回的是TChar*类型（相当于char）
		FString Message = FString::Printf(TEXT("Item Name: %s"), *Name);
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Cyan, Message);
		UE_LOG(LogTemp, Warning, TEXT("Item Name: %s"), *Name);
	}
	*/


	//前进与旋转
	//float MovementRate = 50.f;
	//float RotationRate = 45.f;
	//MovementRate * DeltaTime = (cm/s)*(s/frame) = (cm/frame)   这样就使不用帧率下的物体移动速度变成一样的了
	//AddActorWorldOffset(FVector(MovementRate*DeltaTime, 0.f, 0.f));
	//AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));


	RunningTime += DeltaTime;
	//float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);
	//  0.25修饰振幅，5修饰上下波动的速率  周期等于2*Π/k
	//AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

    //DRAW_SPHERE_SingleFrame(GetActorLocation());
	//DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);

	//绘制与原点的中点
	//FVector AvgVector = Avg<FVector>(GetActorLocation(), FVector::ZeroVector);  //零向量
	//DRAW_POINT_SingleFrame(AvgVector);

	if (ItemState == EItemState::EIS_Hovering)   //悬浮效果实现
	{
		ApplyHoveringEffect(DeltaTime);
	}

}




// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	//日志输出 检查是否调用
	//UE_LOG(LogTemp, Warning, TEXT("Begin Play called!"))

		//检查是否为空指针
		//添加屏幕调试信息
		//  key值, 显示时间, 颜色（一个类，包括了各种信息）, 显示的字符串, 后面的都是默认值可以使用故直接结束
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Cyan, FString("Item OnScreen Message!"));
	//}


	//设置角色位置  先在绘制几何体之前执行
	//SetActorLocation(FVector(0.f, 0.f, 50.f));    //还有三个默认参数
	//设置角色旋转角度
	//SetActorRotation(FRotator(0.f, 45.f, 0.f));


	//构建球体
	//UWorld* World = GetWorld();   //获取当前世界
	//if (World)
	//{
	//	FVector Location = GetActorLocation();   //获取位置
	//	DrawDebugSphere(World, Location, 25.f, 24, FColor::Red, false, 30.f);
		// 当前世界  中心位置  半径  线段数  颜色  是否永久显示   “否”的话显示的时间
		//半径是类型是int32 相当于是int ，当时在ue中需要固定的32位，所以用ue自带的int32
	//}
	//用宏绘制球体   无法调参数
	//UWorld* World = GetWorld();
	//FVector Location = GetActorLocation();
	//DRAW_SPHERE(Location)
	//可以省略分号因为宏定义时已写



	//绘制线
		//if (World)
		//{
		//	FVector Forward = GetActorForwardVector();
		//	DrawDebugLine(World, Location, Location + Forward * 100.f, FColor::Red, true,-1.0f,0,1.0f);
		//世界  起始位置  终止位置（这里用起始位置与方向向量的和来取）（放大了100倍）  颜色  是否永久显示  “是”的话默认为-1输入即可  显示的优先级（越低优先级越高）  线的厚度
		//}
	//用宏绘制线
	//FVector Forward = GetActorForwardVector();
	//DRAW_LINE(Location,Location+Forward*100.f)


	//绘制点
	//if (World)
	//{
		//DrawDebugPoint(World, Location + Forward * 100.f, 15.f, FColor::Red, true);
	//点无论从什么距离看都是一样的大小
	//}
		//用宏绘制点
	//DRAW_POINT(Location + Forward * 100.f)


	//用宏绘制向量（两个宏合为一个）
	//DRAW_VECTOR(Location, Location + Forward * 100.f)

	//int32 AvgInt =Avg <int32>(1, 3);
	//UE_LOG(LogTemp, Warning, TEXT("Avg of 1 and 3 : %d"), AvgInt);


	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	//绑定回调到委托(开始)
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
	//End

}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//打印调式信息
	/*const FString OtherActorName = OtherActor->GetName();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 30.f, FColor::Red, OtherActorName);
	}
	*/

	//设置当前物品
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->SetOverlappingItem(this);
	}

}

void AItem::SpawnPickupSystem()
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation());
	}
}

void AItem::SpawnPickupSound()
{
	if (PickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, PickupSound, GetActorLocation());
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*const FString OtherActorName = FString("Ending Overlap with: ") + OtherActor->GetName();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(2, 30.f, FColor::Blue, OtherActorName);
	}
	*/

	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->SetOverlappingItem(nullptr); //离开重叠范围时指针置空
	}

}

void AItem::ApplyHoveringEffect(float DeltaTime)
{
	const float Z = TransformedSin();
	AddActorLocalOffset(FVector(0.f, 0.f, Z));
}

