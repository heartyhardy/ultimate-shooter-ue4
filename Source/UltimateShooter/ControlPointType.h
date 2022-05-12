#pragma once

UENUM(BlueprintType)
enum class EControlPointType : uint8
{
	ECPT_Armor UMETA(DisplayName = "Armor"),
	ECPT_Damage UMETA(DisplayName = "Damage"),
	ECPT_Health UMETA(DisplayName = "Health"),
	ECPT_Speed UMETA(DisplayName = "Speed"),

	ECPT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EControlPointFaction : uint8
{
	ECPF_Friendly UMETA(DisplayName = "Friendly"),
	ECPF_Enemy UMETA(DisplayName = "Enemy"),
	ECPF_Neutral UMETA(DisplayName = "Neutral"),

	ECPF_MAX UMETA(DisplayName = "DefaultMAX")
};