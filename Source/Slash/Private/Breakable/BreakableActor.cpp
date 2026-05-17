#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);  // 生成重叠事件
	GeometryCollection->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GeometryCollection->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (bBroken) return;
	bBroken = true;

	// 破碎瞬间把正站在这个可破坏物上的角色从 movement base 上解绑，否则角色会继续尝试“站稳”在碎裂的动态基座上。
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<ACharacter> CharacterIt(World); CharacterIt; ++CharacterIt)
		{
			ACharacter* Character = *CharacterIt;
			if (!Character)
			{
				continue;
			}

			UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
			if (!CharacterMovement)
			{
				continue;
			}

			UPrimitiveComponent* MovementBase = CharacterMovement->GetMovementBase();
			const bool bStandingOnThisBreakable =
				MovementBase == Capsule ||
				MovementBase == GeometryCollection ||
				(MovementBase && MovementBase->GetOwner() == this);

			if (bStandingOnThisBreakable)
			{
				CharacterMovement->SetBase(nullptr);
				CharacterMovement->SetMovementMode(MOVE_Falling);
			}
		}
	}

	// 破碎后不要再让完整物体的胶囊碰撞继续顶住角色，否则角色站在碎片区域时会和碎片物理互相打架。
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 碎片保留和场景的物理碰撞，但忽略 Pawn，避免角色或敌人站在碎片上时剧烈抖动甚至被挤穿地板。
	GeometryCollection->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GeometryCollection->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	
	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() >= 6)
	{
		//随机生成Treasure
		const int32 RandomIndex = FMath::RandRange(0, 5);
		World->SpawnActor<ATreasure>(TreasureClasses[RandomIndex], GetActorLocation(), GetActorRotation());
	}
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	 
	
}
