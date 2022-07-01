#pragma once

UENUM(BlueprintType)
enum class EKillStreakAnnoucementType : uint8
{
	EKSAT_FirstKill UMETA(DisplayName = "FirstKill"),
	EKSAT_DoubleKill UMETA(DisplayName = "DoubleKill"),
	EKSAT_TripleKill UMETA(DisplayName = "TripleKill"),
	EKSAT_QuadraKill UMETA(DisplayName = "QuadraKill"),
	EKSAT_PentaKill UMETA(DisplayName = "PentaKill"),
	EKAST_EpicKill UMETA(DisplayName = "EpicKill")
};