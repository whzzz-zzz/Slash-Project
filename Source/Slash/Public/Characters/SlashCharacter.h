// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class USlashOverlay;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()



public:
	ASlashCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(class ASoul* Soul) override;
	virtual void AddGold(class ATreasure* Treasure) override;
	//强制内联宏去内联函数（即调用函数时不跳转到定义而是将定义复制过来，适用与简单的set与get，效率更高）
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;

	//输入的回调函数
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();

	void Dodge();

	/** Combat */
	void EquipWeapon(AWeapon* Weapon);
	virtual void Attack()override;
	virtual void AttackEnd()override;
	virtual void DodgeEnd()override;
	virtual bool CanAttack()override;
	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arms();
	void PlayEquipMontage(FName SectionName);
	virtual void Die_Implementation() override;
	bool IsOccupied();
	bool HasEnoughStamina();
	// 在角色真正收尸前统一执行的死亡收尾逻辑
	void FinishDeath();


	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

private:

	void InitializeSlashOverlay();
	void SetHUDHealth();
	/** Character Components */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;      //存儲重叠物品

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	// 空中致死时先继续下落，等落地后再播放死亡蒙太奇
	bool bPendingDeathAfterLanding = false;

	UPROPERTY()
	USlashOverlay* SlashOverlay;
};
