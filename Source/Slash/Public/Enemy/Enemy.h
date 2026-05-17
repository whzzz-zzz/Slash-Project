// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;
class UAnimMontage;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual ~AEnemy() override;
	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** <AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	/** <IHitInterface> */


protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Die_Implementation() override;
	void SpawnSoul();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;
	void ControlHealthBarVisibilityBasedOnDistance();

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
	/*Behaviour*/
	void InitializeEnemy();
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsEngaged();
	bool IsDead();
	bool IsPlayingMontage();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	void StartAttackRecoveryTimer();
	void ClearAttackRecoveryTimer();
	bool IsCombatTargetDead() const;
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();


	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);	        //PawnSeen的回调
	UFUNCTION()
	void AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);


	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;
	UPROPERTY(visibleAnywhere)
	UPawnSensingComponent* PawnSensing;	// 感知组件
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;


	/* Patrol 导航 */
	// 当前巡逻点
	UPROPERTY()
	class AAIController* EnemyController;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;
	FTimerHandle PatrolTimer;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;


	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;
	FTimerHandle AttackRecoveryTimer;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRecoveryTime = 5.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 125.f;
	UPROPERTY(EditAnywhere, category = "Combat")
	float DeathLifeSpan = 8.f;
	UPROPERTY(EditAnywhere, category = "Combat")
	TSubclassOf<class ASoul> SoulClass;
	UPROPERTY(EditAnywhere , Category = "Combat")
	double CombatRadius = 1000.f;
	UPROPERTY(EditAnywhere , Category = "Combat")
	double AttackRadius = 150.f;
	UPROPERTY(EditAnywhere , Category = "Combat")
	double AcceptanceRadius = 60.f;

};
