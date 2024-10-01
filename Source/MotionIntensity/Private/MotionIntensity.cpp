// Copyright (c) 2024 Tyoma Makeev

#include "MotionIntensity.h"

IMPLEMENT_MODULE(FMotionIntensityModule, MotionIntensity)

DEFINE_LOG_CATEGORY(MotionIntensityLog);

/* Public methods */

FMotionIntensityMotionData UMotionIntensityFunctionLibrary::CalculateMotionData(const FVector Location,
                                                                                const FRotator Rotation,
                                                                                const float DeltaTime,
                                                                                const FMotionIntensityConfig& Config,
                                                                                FMotionIntensityServiceData& ServiceData)
{
	if (DeltaTime <= 0.0f)
	{
		UE_LOG(MotionIntensityLog, Error, TEXT("Delta Time should be larger than zero"));
		return FMotionIntensityMotionData();
	}

	const FQuat RotationQuat = Rotation.Quaternion();

	if (!Config.IsValid())
	{
		UE_LOG(MotionIntensityLog, Error, TEXT("MotionIntensityConfig is invalid"));
		return FMotionIntensityMotionData();
	}

	if (ServiceData.bSetPreviousTransformToCurrent)
	{
		ServiceData.PreviousLocation = Location;
		ServiceData.PreviousRotation = RotationQuat;
		ServiceData.bSetPreviousTransformToCurrent = false;
	}

	FMotionIntensityMotionData MotionData;

	if (Config.bCalculateLinearMotion)
	{
		CalculateLinearMotionData(Location, DeltaTime, Config, ServiceData, MotionData);
	}
	if (Config.bCalculateAngularMotion)
	{
		CalculateAngularMotionData(RotationQuat, DeltaTime, Config, ServiceData, MotionData);
	}

	return MotionData;
}

float UMotionIntensityFunctionLibrary::GetLinearMotionIntensityFromMotionData(const FMotionIntensityMotionData& MotionData,
                                                                              const FMotionIntensityCoefficients& Coefficients)
{
	if (!Coefficients.IsValid())
	{
		UE_LOG(MotionIntensityLog, Error, TEXT("MotionIntensityCoefficients is invalid"));
		return 0.0f;
	}

	const float SumOfSquares = FMath::Square(MotionData.LinearVelocityNormalized * Coefficients.LinearVelocityCoefficient)
		+ FMath::Square(MotionData.PositiveLinearAccelerationNormalized * Coefficients.PositiveLinearAccelerationCoefficient)
		+ FMath::Square(MotionData.NegativeLinearAccelerationNormalized * Coefficients.NegativeLinearAccelerationCoefficient)
		+ FMath::Square(MotionData.PositiveLinearJerkNormalized * Coefficients.PositiveLinearJerkCoefficient)
		+ FMath::Square(MotionData.NegativeLinearJerkNormalized * Coefficients.NegativeLinearJerkCoefficient);

	if (SumOfSquares == 0.0f)
	{
		return 0.0f;
	}

	const float LinearMotionIntensity = FMath::Sqrt(SumOfSquares);

	const float MaxPossibleLinearMotionIntensity = FMath::Sqrt(
		FMath::Square(Coefficients.LinearVelocityCoefficient)
		+ FMath::Square(Coefficients.PositiveLinearAccelerationCoefficient)
		+ FMath::Square(Coefficients.NegativeLinearAccelerationCoefficient)
		+ FMath::Square(Coefficients.PositiveLinearJerkCoefficient)
		+ FMath::Square(Coefficients.NegativeLinearJerkCoefficient)
	);

	if (MaxPossibleLinearMotionIntensity == 0.0f)
	{
		return 0.0f;
	}

	return (LinearMotionIntensity / MaxPossibleLinearMotionIntensity) * Coefficients.MotionIntensityMultiplier;
}

float UMotionIntensityFunctionLibrary::GetAngularMotionIntensityFromMotionData(const FMotionIntensityMotionData& MotionData,
                                                                               const FMotionIntensityCoefficients& Coefficients)
{
	if (!Coefficients.IsValid())
	{
		UE_LOG(MotionIntensityLog, Error, TEXT("MotionIntensityCoefficients is invalid"));
		return 0.0f;
	}

	const float SumOfSquares = FMath::Square(MotionData.AngularVelocityNormalized * Coefficients.AngularVelocityCoefficient)
		+ FMath::Square(MotionData.PositiveAngularAccelerationNormalized * Coefficients.PositiveAngularAccelerationCoefficient)
		+ FMath::Square(MotionData.NegativeAngularAccelerationNormalized * Coefficients.NegativeAngularAccelerationCoefficient)
		+ FMath::Square(MotionData.PositiveAngularJerkNormalized * Coefficients.PositiveAngularJerkCoefficient)
		+ FMath::Square(MotionData.NegativeAngularJerkNormalized * Coefficients.NegativeAngularJerkCoefficient);

	if (SumOfSquares == 0.0f)
	{
		return 0.0f;
	}

	const float AngularMotionIntensity = FMath::Sqrt(SumOfSquares);

	const float MaxPossibleAngularMotionIntensity = FMath::Sqrt(
		FMath::Square(Coefficients.AngularVelocityCoefficient)
		+ FMath::Square(Coefficients.PositiveAngularAccelerationCoefficient)
		+ FMath::Square(Coefficients.NegativeAngularAccelerationCoefficient)
		+ FMath::Square(Coefficients.PositiveAngularJerkCoefficient)
		+ FMath::Square(Coefficients.NegativeAngularJerkCoefficient)
	);

	if (MaxPossibleAngularMotionIntensity == 0.0f)
	{
		return 0.0f;
	}

	return (AngularMotionIntensity / MaxPossibleAngularMotionIntensity) * Coefficients.MotionIntensityMultiplier;
}

float UMotionIntensityFunctionLibrary::GetMotionIntensityFromMotionData(const FMotionIntensityMotionData& MotionData,
                                                                        const FMotionIntensityCoefficients& Coefficients)
{
	if (!Coefficients.IsValid())
	{
		UE_LOG(MotionIntensityLog, Error, TEXT("MotionIntensityCoefficients is invalid"));
		return 0.0f;
	}

	const float LinearMotionIntensity = GetLinearMotionIntensityFromMotionData(MotionData, Coefficients);
	const float AngularMotionIntensity = GetAngularMotionIntensityFromMotionData(MotionData, Coefficients);

	const float MotionIntensity = FMath::Sqrt(
		FMath::Square(LinearMotionIntensity) +
		FMath::Square(AngularMotionIntensity)
	) / UE_SQRT_2;

	return MotionIntensity;
}

float UMotionIntensityFunctionLibrary::GetMotionIntensity(const FVector Location,
                                                          const FRotator Rotation,
                                                          const float DeltaTime,
                                                          const FMotionIntensityConfig& Config,
                                                          FMotionIntensityServiceData& ServiceData,
                                                          const FMotionIntensityCoefficients& Coefficients)
{
	const FMotionIntensityMotionData MotionData = CalculateMotionData(Location, Rotation, DeltaTime, Config, ServiceData);
	return GetMotionIntensityFromMotionData(MotionData, Coefficients);
}

/* Private methods */

float UMotionIntensityFunctionLibrary::GetSmoothedDerivative(const float Current,
                                                             float& OutPrevious,
                                                             const float DeltaTime,
                                                             const float InterpolationSpeed)
{
	if (DeltaTime <= 0.0f)
	{
		UE_LOG(MotionIntensityLog, Error, TEXT("Delta Time should be larger than zero"));
		return 0.0f;
	}
	const float SmoothedValue = FMath::FInterpTo(OutPrevious, Current, DeltaTime, InterpolationSpeed);
	const float Derivative = (SmoothedValue - OutPrevious) / DeltaTime;
	OutPrevious = SmoothedValue;
	return Derivative;
}

float UMotionIntensityFunctionLibrary::GetLinearVelocitySmoothed(const FVector& Current,
                                                                 FVector& OutPrevious,
                                                                 const float DeltaTime,
                                                                 const float InterpolationSpeed)
{
	if (DeltaTime <= 0.0f)
	{
		UE_LOG(MotionIntensityLog, Error, TEXT("Delta Time should be larger than zero"));
		return 0.0f;
	}
	const FVector SmoothedValue = FMath::VInterpTo(OutPrevious, Current, DeltaTime, InterpolationSpeed);
	const float Derivative = (SmoothedValue - OutPrevious).Length() / DeltaTime;
	OutPrevious = SmoothedValue;
	return Derivative;
}

float UMotionIntensityFunctionLibrary::GetAngularVelocitySmoothed(const FQuat& Current,
                                                                  FQuat& OutPrevious,
                                                                  const float DeltaTime,
                                                                  const float InterpolationSpeed)
{
	if (DeltaTime <= 0.0f)
	{
		UE_LOG(MotionIntensityLog, Error, TEXT("Delta Time should be larger than zero"));
		return 0.0f;
	}
	const FQuat SmoothedValue = FMath::QInterpTo(OutPrevious, Current, DeltaTime, InterpolationSpeed);
	const float Revolutions = SmoothedValue.AngularDistance(OutPrevious) / (2.0f * PI); // Convert radians to revolutions
	OutPrevious = SmoothedValue;
	return Revolutions / DeltaTime;
}

void UMotionIntensityFunctionLibrary::CalculateLinearMotionData(const FVector& CurrentLocation,
                                                                const float DeltaTime,
                                                                const FMotionIntensityConfig& Config,
                                                                FMotionIntensityServiceData& ServiceData,
                                                                FMotionIntensityMotionData& OutMotionData)
{
	if (DeltaTime <= 0.0f)
	{
		UE_LOG(MotionIntensityLog, Error, TEXT("Delta Time should be larger than zero"));
		return;
	}

	OutMotionData.LinearVelocityNormalized = GetLinearVelocitySmoothed(CurrentLocation,
	                                                                   ServiceData.PreviousLocation,
	                                                                   DeltaTime,
	                                                                   Config.LocationInterpolationSpeed) / Config.MaxLinearVelocity;

	if (Config.bClampLinearVelocity)
	{
		OutMotionData.LinearVelocityNormalized = FMath::Min(1.0f, OutMotionData.LinearVelocityNormalized);
	}

	const float LinearAccelerationNormalized = GetSmoothedDerivative(OutMotionData.LinearVelocityNormalized,
	                                                                 ServiceData.PreviousLinearVelocity,
	                                                                 DeltaTime,
	                                                                 Config.LinearVelocityInterpolationSpeed) / Config.LinearVelocityInterpolationSpeed;
	const float LinearJerkNormalized = GetSmoothedDerivative(LinearAccelerationNormalized,
	                                                         ServiceData.PreviousLinearAcceleration,
	                                                         DeltaTime,
	                                                         Config.LinearAccelerationInterpolationSpeed) / Config.LinearAccelerationInterpolationSpeed;

	OutMotionData.PositiveLinearAccelerationNormalized = FMath::Max(0.0f, LinearAccelerationNormalized);
	OutMotionData.NegativeLinearAccelerationNormalized = FMath::Abs(FMath::Min(0.0f, LinearAccelerationNormalized));
	OutMotionData.PositiveLinearJerkNormalized = FMath::Max(0.0f, LinearJerkNormalized);
	OutMotionData.NegativeLinearJerkNormalized = FMath::Abs(FMath::Min(0.0f, LinearJerkNormalized));
}

void UMotionIntensityFunctionLibrary::CalculateAngularMotionData(const FQuat& CurrentRotation,
                                                                 const float DeltaTime,
                                                                 const FMotionIntensityConfig& Config,
                                                                 FMotionIntensityServiceData& ServiceData,
                                                                 FMotionIntensityMotionData& OutMotionData)
{
	if (DeltaTime <= 0.0f)
	{
		UE_LOG(MotionIntensityLog, Error, TEXT("Delta Time should be larger than zero"));
		return;
	}

	OutMotionData.AngularVelocityNormalized = GetAngularVelocitySmoothed(CurrentRotation,
	                                                                     ServiceData.PreviousRotation,
	                                                                     DeltaTime,
	                                                                     Config.RotationInterpolationSpeed) / Config.MaxAngularVelocity;

	if (Config.bClampAngularVelocity)
	{
		OutMotionData.AngularVelocityNormalized = FMath::Min(1.0f, OutMotionData.AngularVelocityNormalized);
	}

	const float AngularAccelerationNormalized = GetSmoothedDerivative(OutMotionData.AngularVelocityNormalized,
	                                                                  ServiceData.PreviousAngularVelocity,
	                                                                  DeltaTime,
	                                                                  Config.AngularVelocityInterpolationSpeed) / Config.AngularVelocityInterpolationSpeed;
	const float AngularJerkNormalized = GetSmoothedDerivative(AngularAccelerationNormalized,
	                                                          ServiceData.PreviousAngularAcceleration,
	                                                          DeltaTime,
	                                                          Config.AngularAccelerationInterpolationSpeed) / Config.AngularAccelerationInterpolationSpeed;

	OutMotionData.PositiveAngularAccelerationNormalized = FMath::Max(0.0f, AngularAccelerationNormalized);
	OutMotionData.NegativeAngularAccelerationNormalized = FMath::Abs(FMath::Min(0.0f, AngularAccelerationNormalized));
	OutMotionData.PositiveAngularJerkNormalized = FMath::Max(0.0f, AngularJerkNormalized);
	OutMotionData.NegativeAngularJerkNormalized = FMath::Abs(FMath::Min(0.0f, AngularJerkNormalized));
}
