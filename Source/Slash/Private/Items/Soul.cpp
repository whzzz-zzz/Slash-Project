// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 灵魂缓缓下落：当前高度高于目标高度时，以 DriftRate 速率向下漂移
	const double LocationZ = GetActorLocation().Z;
	if (LocationZ > DesiredZ)
	{
		AddActorWorldOffset(FVector(0, 0, DriftRate*DeltaTime));
	}
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	// 从灵魂生成位置向下做射线检测，找到地面高度作为下落目标
	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0, 0, 2000);  // 射线终点：向下延伸2000单位

	// 仅检测世界静态物体（ObjectTypeQuery1 = WorldStatic）
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	// 忽略拥有者（即生成该灵魂的敌人），避免射线命中敌人自身
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingleForObjects(this,Start,End,ObjectTypes,false,ActorsToIgnore,EDrawDebugTrace::None,HitResult,true);
	// 将射线命中点的高度设为灵魂下落的目标高度+50
	DesiredZ = HitResult.ImpactPoint.Z + 50.f;
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddSouls(this);
		SpawnPickupSystem();
		SpawnPickupSound();
		Destroy();
	}

}


