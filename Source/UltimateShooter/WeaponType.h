#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_AssultRifle UMETA(DisplayName = "AssualtRifle"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};