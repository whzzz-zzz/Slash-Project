// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Item.generated.h"

class USphereComponent;
class UNiagaraComponent;


enum class EItemState:uint8
{
	EIS_Hovering,      // 悬浮
	EIS_Equipped
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)     // 不改变类的任何属性的纯蓝图函数  Callable则可以
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	template<typename T>
	T Avg(T First, T Second);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//重叠球体的绑定委托调用函数(开始时)

	virtual void SpawnPickupSystem();
	virtual void SpawnPickupSound();

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void ApplyHoveringEffect(float DeltaTime);  // 悬浮效果实现函数

	//暴露给蓝图反应系统  EditDefaultsOnly只能在蓝图细节中修改  EditInstanceOnly只能在实例中修改   EditAnywhere则都允许
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")   // 使用蓝图读写或者只读（ReadOnly）功能时，不能是私有属性，除非加上特殊的限定词
	float Amplitude = 0.25f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sine Parameters")    // 添加到一个标签（类别）中
	float TimeConstant = 5.f;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;       // 重叠用的球体

	UPROPERTY(EditAnywhere)       //粒子系统
	class UNiagaraComponent* ItemEffect;
	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;

private:
                                                 //通过元指定符的语法来使私有元素可以被获取
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess="true"))      //visible跟edit用法一样，前者只读，后者可编辑
	float RunningTime;
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickupEffect;

};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;     // 注意有的类型不行，比如Rotator（旋转角）不支持加减乘除运算
}
