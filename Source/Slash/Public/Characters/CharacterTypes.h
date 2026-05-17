#pragma once

/*
 * 角色相关枚举定义
 * - 枚举命名以 E 开头，使用 enum class 限定作用域，防止常量名冲突
 * - 底层类型为 uint8，节省内存
 * - UENUM(BlueprintType) 使枚举可在蓝图中作为类型使用
 * - UMETA(DisplayName = "xxx") 定义枚举值在蓝图中的显示名称
 * - 枚举值默认从 0 递增，也可手动指定（如 Unequipped = 3）
 * - 枚举值前缀缩写便于区分所属枚举
 */

UENUM(BlueprintType)
enum class ECharacterState :uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Hand Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Hand Weapon")
};

UENUM(BlueprintType)
enum class EActionState :uint8
{
	EAS_Unoccupied UMETA(DisplayName="Unoccupied"),
	EAS_HitReaction UMETA(DisplayName="HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	EAS_Dodge UMETA(DisplayName = "Dodge"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),

	EDP_Max UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EEnemyState :uint8
{
	EES_NoState UMETA(DisplayName = "NoState"),
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),   //巡逻
	EES_Chasing UMETA(DisplayName = "Chasing"),       //追逐
	EES_Attacking UMETA(DisplayName = "Attacking"),       //攻击状态
	EES_Engaged UMETA(DisplayName = "Engaged")  //战斗中（攻击）


};


