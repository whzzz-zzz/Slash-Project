// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Animation/AnimInstance.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapons/Weapon.h"
#include "AIController.h"
#include "Items/Soul.h"
#include "Kismet/GameplayStatics.h"

class UAnimMontage;
class AWeapon;

AEnemy::AEnemy()
{

	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);


	// 血条组件
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	// 朝移动方向转向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	// 角色感知
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;             // 距离（半径）
	PawnSensing->SetPeripheralVisionAngle(45.f);   // 视野角度
}

AEnemy::~AEnemy()
{

}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 死亡后不再执行任何 AI 逻辑
	if (IsDead()) return;

	// 只要还持有战斗目标，就持续推进战斗状态机；NoState 仍然属于战斗过渡态
	if (CombatTarget)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
		ControlHealthBarVisibilityBasedOnDistance();
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	HandleDamage(DamageAmount);  // 更新血量显示
	// 只有存活时才更新战斗目标
	if (EventInstigator && Attributes && Attributes->IsAlive())
	{
		ShowHealthBar();
		CombatTarget = EventInstigator->GetPawn();
		if (IsInsideAttackRadius())
		{
			EnemyState = EEnemyState::EES_Attacking;
		}
		else if (IsOutsideAttackRadius())
		{
			ChaseTarget();
		}
	}
	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (IsDead()) HideHealthBar();
	else
	{
		// 受击会打断攻击蒙太奇，确保敌人不会卡在 Engaged 状态
		ClearAttackTimer();
		ClearAttackRecoveryTimer();
		EnemyState = EEnemyState::EES_NoState;
	}
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnable(ECollisionEnabled::NoCollision);
	StopAttackMontage();
	if (IsInsideAttackRadius())
	{
		if (!IsDead())StartAttackTimer();
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitializeEnemy();
	if (PawnSensing)PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);	// 绑定回调函数
	Tags.Add(FName("Enemy"));
}

void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 在 EndPlay 中清理资源，此时 World 仍然有效
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	// 解绑感知事件
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.RemoveDynamic(this, &AEnemy::PawnSeen);
	}
	Super::EndPlay(EndPlayReason);
}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();
	// 死亡后清除战斗目标和攻击定时器，并标记为死亡状态
	CombatTarget = nullptr;
	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	ClearAttackRecoveryTimer();
	DisableCapsule();
	HideHealthBar();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	// 保留 Mesh 碰撞，让尸体还能被角色踩住，也能被腿部 IK 的地面检测命中。
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	SpawnSoul();

}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 125);
		ASoul* SpawnSoul = World->SpawnActor<ASoul>(SoulClass,SpawnLocation,GetActorRotation());
		if (SpawnSoul)
		{
			SpawnSoul->SetSouls(Attributes->GetSouls());
			SpawnSoul->SetOwner(this);  // 设置灵魂的拥有者为敌人
		}
	}
}

void AEnemy::Attack()
{
	if (CombatTarget == nullptr) return;
	if (IsCombatTargetDead() || !IsInsideAttackRadius() || IsDead() || IsEngaged() || IsPlayingMontage())
	{
		EnemyState = EEnemyState::EES_NoState;
		CheckCombatTarget();
		return;
	}

	EnemyState = EEnemyState::EES_Engaged;
	Super::Attack();
	PlayAttackMontage();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (UAnimMontage* ActiveMontage = AnimInstance->GetCurrentActiveMontage())
		{
			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &AEnemy::AttackMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, ActiveMontage);
		}
	}
	StartAttackRecoveryTimer();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead() &&
		!IsPlayingMontage();
	return bCanAttack;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::AttackEnd()
{
	Super::AttackEnd();
	if (IsCombatTargetDead())
	{
		ClearAttackRecoveryTimer();
		EnemyState = EEnemyState::EES_NoState;
		LoseInterest();
		StartPatrolling();
		return;
	}

	if (IsPlayingMontage())
	{
		StartAttackRecoveryTimer();
		return;
	}

	ClearAttackRecoveryTimer();
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::ControlHealthBarVisibilityBasedOnDistance()
{
	// 根据玩家距离控制血条显示（未进入战斗状态时）
	if (HealthBarWidget && Attributes && Attributes->IsAlive() && !CombatTarget)
	{
		const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			const double DistanceToPlayer = (PlayerPawn->GetActorLocation() - GetActorLocation()).Size();
			HealthBarWidget->SetVisibility(DistanceToPlayer <= CombatRadius);
		}
	}
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	HideHealthBar();
	StartPatrolling();
	SpawnDefaultWeapon();
}

void AEnemy::CheckCombatTarget()
{
	if (IsCombatTargetDead())
	{
		ClearAttackTimer();
		ClearAttackRecoveryTimer();
		LoseInterest();
		if (!IsEngaged()) StartPatrolling();
		return;
	}

	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged()) StartPatrolling();
	}
	else if (IsOutsideAttackRadius() && !IsChasing() && !IsAttacking())
	{
		ClearAttackTimer();
		if (!IsEngaged()) ChaseTarget();
	}
	else if (IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsPlayingMontage())
	{
		StartAttackTimer();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		// 到达巡逻目标后，设置定时器，在停留时间结束后选择新的巡逻目标并移动
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::PatrolTimerFinished()
{
	StartPatrolling();
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	ShowHealthBar();
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsPlayingMontage()
{
	if (const UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		return AnimInstance->IsAnyMontagePlaying();
	}
	return false;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::StartAttackRecoveryTimer()
{
	GetWorldTimerManager().SetTimer(AttackRecoveryTimer, this, &AEnemy::AttackEnd, AttackRecoveryTime);
}

void AEnemy::ClearAttackRecoveryTimer()
{
	GetWorldTimerManager().ClearTimer(AttackRecoveryTimer);
}

bool AEnemy::IsCombatTargetDead() const
{
	return CombatTarget && CombatTarget->ActorHasTag(FName("Dead"));
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr)
	{
		return;
	}
	// 创建 AI 移动请求，设置巡逻目标和到达判定半径
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);           // 指定移动目标
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);      // 距离目标 60 个单位即视为到达
	EnemyController->MoveTo(MoveRequest);       // AI 开始沿导航路径移动
}

AActor* AEnemy::ChoosePatrolTarget()
{
	// 当前巡逻目标到达后，筛选其他有效巡逻目标
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		// 排除当前目标，将其他巡逻目标加入候选列表
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}
	// 随机选择一个巡逻目标作为新的当前目标
	const int32 NumValidTargets = ValidTargets.Num();
	if (NumValidTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumValidTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

void AEnemy::SpawnDefaultWeapon()
{
	// 生成敌人的武器
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		// 敌人初始化自带武器时不播放装备音效
		DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this, false);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState != EEnemyState::EES_Attacking &&
		EnemyState < EEnemyState::EES_Engaged &&
		SeenPawn->ActorHasTag(FName("EngageableTarget")) &&
		!SeenPawn->ActorHasTag(FName("Dead"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

void AEnemy::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsEngaged())
	{
		ClearAttackRecoveryTimer();
		EnemyState = EEnemyState::EES_NoState;
		CheckCombatTarget();
	}
}
