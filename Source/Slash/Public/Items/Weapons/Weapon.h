// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include <UObject/ObjectMacros.h>
#include "Weapon.generated.h"



class USoundBase;
class UBoxComponent;     // 剑刃的碰撞箱

/**
 *
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();  // 构造函数

	void Equip(USceneComponent*InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator, bool bPlaySound = true);

	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }

	UPROPERTY()
	TArray<AActor*> IgnoreActors;
protected:
	virtual void BeginPlay()override;
	void PlayEquipSound();
	void DisableSphereCollision();
	void DeactivateEmbers();

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:

	void BoxTrace(FHitResult& BoxHit);
	void ExecuteGetHit(FHitResult& BoxHit);
	bool ActorIsSameType(AActor* OtherActor);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);   // 碰撞箱的范围

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;  // 是否显示碰撞箱

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;


	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

};
