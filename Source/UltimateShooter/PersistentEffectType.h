#pragma once

UENUM(BlueprintType)
enum class EPersistentEffectType : uint8
{
	EPET_Armor UMETA(DisplayName = "Armor"),
	EPET_Damage UMETA(DisplayName = "Damage"),
	EPET_Health UMETA(DisplayName = "Health"),
	EPET_Speed UMETA(DisplayName = "Speed"),

	EPET_MAX UMETA(DisplayName = "DefaultMAX")
};