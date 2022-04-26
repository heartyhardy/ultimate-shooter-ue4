#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(DisplayName = "9mmm"),
	EAT_AR UMETA(DisplayName = "AssaultRifle"),

	EAT_MAX UMETA(DisplayName = "DefaultMAX")
};