// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"

#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;


UCLASS()
class SLASH_API ABreakableActor : public AActor,public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

private:


	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;  //建立数组去随机生成Treasure    
	//使用包装器而不是直接使用UClass可以在UE中减少不必要的选择项
	//UClass* TreasureClass;
	
	bool bBroken;    //防止重复碰撞不断死循环，用于判断是否已经破碎
};
