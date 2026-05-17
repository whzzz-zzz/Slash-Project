// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAnimMontage;
class UAttributeComponent;


UCLASS()
class SLASH_API ABaseCharacter : public ACharacter,public IHitInterface   //多重继承
{
	GENERATED_BODY()

public:

	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; } //获取死亡姿势

protected:

	virtual void BeginPlay() override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	//Combat
	virtual void Attack();
	virtual bool CanAttack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitReactParticles(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	bool IsAlive();

	UFUNCTION(BlueprintNativeEvent)
	void Die();

	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	void StopAttackMontage();
	//武器碰撞以及追踪开关
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnable(ECollisionEnabled::Type CollisionEnabled);
	void DisableCapsule();
	void DisableMeshCollision();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	UAttributeComponent* Attributes;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;
	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance;
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, Category = "Combat")     //视觉效果
	UParticleSystem* HitParticles;       //粒子系统
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;     //攻击蒙太奇的分段名称
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> DeathMontageSections;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;        //在蓝图中设置蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DodgeMontage;

	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage,const TArray<FName>& SectionNames);


};
