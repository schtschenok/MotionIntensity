// Copyright (c) 2024 Tyoma Makeev

#pragma once

#include "Modules/ModuleManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MotionIntensity.generated.h"

class FMotionIntensityModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};

DECLARE_LOG_CATEGORY_EXTERN(MotionIntensityLog, Log, All);

USTRUCT(BlueprintType)
struct FMotionIntensityConfig
{
	GENERATED_BODY()

	// If true, linear motion will be calculated
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	bool bCalculateLinearMotion = true;

	// Maximum linear velocity in cm/s, must be > 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config",
		meta = (EditCondition = "bCalculateLinearMotion", ClampMin = "0.01"))
	float MaxLinearVelocity = 1000.0f; // Sensible default for cm/s

	// If true, linear velocity will be clamped
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config",
		meta = (EditCondition = "bCalculateLinearMotion"))
	bool bClampLinearVelocity = false;

	// Location interpolation speed, used in velocity calculation, must be > 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config",
		meta = (EditCondition = "bCalculateLinearMotion", ClampMin = "0.01"))
	float LocationInterpolationSpeed = 10.0f; // 10.0f is a sensible default for interpolation speeds as it's just gives a smooth but not too smooth result

	// Linear velocity interpolation speed, used in acceleration calculation, must be > 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config",
		meta = (EditCondition = "bCalculateLinearMotion", ClampMin = "0.01"))
	float LinearVelocityInterpolationSpeed = 10.0f;

	// Linear acceleration interpolation speed, used in jerk calculation, must be > 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config",
		meta = (EditCondition = "bCalculateLinearMotion", ClampMin = "0.01"))
	float LinearAccelerationInterpolationSpeed = 10.0f;

	// If true, angular motion will be calculated
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	bool bCalculateAngularMotion = true;

	// Maximum angular velocity in rev/s, must be > 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config",
		meta = (EditCondition = "bCalculateAngularMotion", ClampMin = "0.01"))
	float MaxAngularVelocity = 4.0f; // Sensible default for rev/s

	// If true, angular velocity will be clamped
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config",
		meta = (EditCondition = "bCalculateAngularMotion"))
	bool bClampAngularVelocity = false;

	// Rotation interpolation speed, used in velocity calculation, must be > 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config",
		meta = (EditCondition = "bCalculateAngularMotion", ClampMin = "0.01"))
	float RotationInterpolationSpeed = 10.0f;

	// Angular velocity interpolation speed, used in acceleration calculation, must be > 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config",
		meta = (EditCondition = "bCalculateAngularMotion", ClampMin = "0.01"))
	float AngularVelocityInterpolationSpeed = 10.0f;

	// Angular acceleration interpolation speed, used in jerk calculation, must be > 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config",
		meta = (EditCondition = "bCalculateAngularMotion", ClampMin = "0.01"))
	float AngularAccelerationInterpolationSpeed = 10.0f;

	bool IsValid() const
	{
		return MaxLinearVelocity > 0.0f
			&& LocationInterpolationSpeed > 0.0f
			&& LinearVelocityInterpolationSpeed > 0.0f
			&& LinearAccelerationInterpolationSpeed > 0.0f
			&& MaxAngularVelocity > 0.0f
			&& RotationInterpolationSpeed > 0.0f
			&& AngularVelocityInterpolationSpeed > 0.0f
			&& AngularAccelerationInterpolationSpeed > 0.0f;
	}
};

USTRUCT(BlueprintType)
struct FMotionIntensityServiceData
{
	GENERATED_BODY()

	// If true, will set the previous transform to the current one
	UPROPERTY(BlueprintReadWrite)
	bool bSetPreviousTransformToCurrent = true;

	// Previous location vector
	UPROPERTY(BlueprintReadWrite)
	FVector PreviousLocation = FVector::ZeroVector;

	// Previous rotation quaternion
	UPROPERTY(BlueprintReadWrite)
	FQuat PreviousRotation = FQuat::Identity;

	// Previous linear velocity value
	UPROPERTY(BlueprintReadWrite)
	float PreviousLinearVelocity = 0.0f;

	// Previous linear acceleration value
	UPROPERTY(BlueprintReadWrite)
	float PreviousLinearAcceleration = 0.0f;

	// Previous linear jerk value
	UPROPERTY(BlueprintReadWrite)
	float PreviousLinearJerk = 0.0f;

	// Previous angular velocity value
	UPROPERTY(BlueprintReadWrite)
	float PreviousAngularVelocity = 0.0f;

	// Previous angular acceleration value
	UPROPERTY(BlueprintReadWrite)
	float PreviousAngularAcceleration = 0.0f;

	// Previous angular jerk value
	UPROPERTY(BlueprintReadWrite)
	float PreviousAngularJerk = 0.0f;


	void Reset()
	{
		bSetPreviousTransformToCurrent = true;
		PreviousLocation = FVector::ZeroVector;
		PreviousRotation = FQuat::Identity;
		PreviousLinearVelocity = 0.0f;
		PreviousLinearAcceleration = 0.0f;
		PreviousLinearJerk = 0.0f;
		PreviousAngularVelocity = 0.0f;
		PreviousAngularAcceleration = 0.0f;
		PreviousAngularJerk = 0.0f;
	}
};

USTRUCT(BlueprintType)
struct FMotionIntensityMotionData
{
	GENERATED_BODY()

	// Normalized linear velocity
	UPROPERTY(BlueprintReadOnly)
	float LinearVelocityNormalized = 0.0f;

	// Normalized positive linear acceleration
	UPROPERTY(BlueprintReadOnly)
	float PositiveLinearAccelerationNormalized = 0.0f;

	// Normalized negative linear acceleration
	UPROPERTY(BlueprintReadOnly)
	float NegativeLinearAccelerationNormalized = 0.0f;

	// Normalized positive linear jerk
	UPROPERTY(BlueprintReadOnly)
	float PositiveLinearJerkNormalized = 0.0f;

	// Normalized negative linear jerk
	UPROPERTY(BlueprintReadOnly)
	float NegativeLinearJerkNormalized = 0.0f;

	// Normalized angular velocity
	UPROPERTY(BlueprintReadOnly)
	float AngularVelocityNormalized = 0.0f;

	// Normalized positive angular acceleration
	UPROPERTY(BlueprintReadOnly)
	float PositiveAngularAccelerationNormalized = 0.0f;

	// Normalized negative angular acceleration
	UPROPERTY(BlueprintReadOnly)
	float NegativeAngularAccelerationNormalized = 0.0f;

	// Normalized positive angular jerk
	UPROPERTY(BlueprintReadOnly)
	float PositiveAngularJerkNormalized = 0.0f;

	// Normalized negative angular jerk
	UPROPERTY(BlueprintReadOnly)
	float NegativeAngularJerkNormalized = 0.0f;
};

USTRUCT(BlueprintType)
struct FMotionIntensityCoefficients
{
	GENERATED_BODY()

	// Multiplier to be applied to the resulting value
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta = (ClampMin = "0.0"))
	float MotionIntensityMultiplier = 1.0f;

	// Coefficient for linear velocity, must be >= 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta = (ClampMin = "0.0"))
	float LinearVelocityCoefficient = 0.0f; // By default, we don't care about velocities as they don't represent the motion intensity well

	// Coefficient for positive linear acceleration, must be >= 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta = (ClampMin = "0.0"))
	float PositiveLinearAccelerationCoefficient = 1.0f;

	// Coefficient for negative linear acceleration, must be >= 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta = (ClampMin = "0.0"))
	float NegativeLinearAccelerationCoefficient = 1.0f;

	// Coefficient for positive linear jerk, must be >= 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta = (ClampMin = "0.0"))
	float PositiveLinearJerkCoefficient = 1.0f;

	// Coefficient for negative linear jerk, must be >= 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta = (ClampMin = "0.0"))
	float NegativeLinearJerkCoefficient = 1.0f;

	// Coefficient for angular velocity, must be >= 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta = (ClampMin = "0.0"))
	float AngularVelocityCoefficient = 0.0f;

	// Coefficient for positive angular acceleration, must be >= 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta = (ClampMin = "0.0"))
	float PositiveAngularAccelerationCoefficient = 1.0f;

	// Coefficient for negative angular acceleration, must be >= 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta = (ClampMin = "0.0"))
	float NegativeAngularAccelerationCoefficient = 1.0f;

	// Coefficient for positive angular jerk, must be >= 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta = (ClampMin = "0.0"))
	float PositiveAngularJerkCoefficient = 1.0f;

	// Coefficient for negative angular jerk, must be >= 0.0f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta = (ClampMin = "0.0"))
	float NegativeAngularJerkCoefficient = 1.0f;

	bool IsValid() const
	{
		return LinearVelocityCoefficient >= 0.0f
			&& PositiveLinearAccelerationCoefficient >= 0.0f
			&& NegativeLinearAccelerationCoefficient >= 0.0f
			&& PositiveLinearJerkCoefficient >= 0.0f
			&& NegativeLinearJerkCoefficient >= 0.0f
			&& AngularVelocityCoefficient >= 0.0f
			&& PositiveAngularAccelerationCoefficient >= 0.0f
			&& NegativeAngularAccelerationCoefficient >= 0.0f
			&& PositiveAngularJerkCoefficient >= 0.0f
			&& NegativeAngularJerkCoefficient >= 0.0f;
	}
};

UCLASS(meta=(BlueprintThreadSafe))
class UMotionIntensityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Calculates motion data based on the given transform and config
	UFUNCTION(BlueprintCallable)
	static UPARAM(DisplayName = "Motion Data") FMotionIntensityMotionData CalculateMotionData(
		UPARAM(DisplayName = "Current Location") const FVector Location, // Not by-ref since it needs to have a default value
		UPARAM(DisplayName = "Current Rotation") const FRotator Rotation,
		UPARAM(DisplayName = "Delta Time") float DeltaTime,
		UPARAM(DisplayName = "Config") const FMotionIntensityConfig& Config,
		UPARAM(ref, DisplayName = "Service Data") FMotionIntensityServiceData& ServiceData);

	// Calculates linear motion intensity from motion data and coefficients
	UFUNCTION(BlueprintCallable)
	static UPARAM(DisplayName = "Linear Motion Intensity") float GetLinearMotionIntensityFromMotionData(
		UPARAM(DisplayName = "Motion Data") const FMotionIntensityMotionData& MotionData,
		UPARAM(DisplayName = "Coefficients") const FMotionIntensityCoefficients& Coefficients);

	// Calculates angular motion intensity from motion data and coefficients
	UFUNCTION(BlueprintCallable)
	static UPARAM(DisplayName = "Angular Motion Intensity") float GetAngularMotionIntensityFromMotionData(
		UPARAM(DisplayName = "Motion Data") const FMotionIntensityMotionData& MotionData,
		UPARAM(DisplayName = "Coefficients") const FMotionIntensityCoefficients& Coefficients);

	// Calculates overall motion intensity from motion data and coefficients
	UFUNCTION(BlueprintCallable)
	static UPARAM(DisplayName = "Motion Intensity") float GetMotionIntensityFromMotionData(
		UPARAM(DisplayName = "Motion Data") const FMotionIntensityMotionData& MotionData,
		UPARAM(DisplayName = "Coefficients") const FMotionIntensityCoefficients& Coefficients);

	// Calculates overall motion intensity based on the transform, config, and coefficients
	UFUNCTION(BlueprintCallable)
	static UPARAM(DisplayName = "Motion Intensity") float GetMotionIntensity(
		UPARAM(DisplayName = "Current Location") const FVector Location,
		UPARAM(DisplayName = "Current Rotation") const FRotator Rotation,
		UPARAM(DisplayName = "Delta Time") float DeltaTime,
		UPARAM(DisplayName = "Config") const FMotionIntensityConfig& Config,
		UPARAM(ref, DisplayName = "Service Data") FMotionIntensityServiceData& ServiceData,
		UPARAM(DisplayName = "Coefficients") const FMotionIntensityCoefficients& Coefficients);

	// Resets Service Data to its initial state
	UFUNCTION(BlueprintCallable)
	static void ResetServiceData(
		UPARAM(ref, DisplayName = "Service Data") FMotionIntensityServiceData& ServiceData)
	{
		ServiceData.Reset();
	}

private:
	static float GetSmoothedDerivative(const float Current,
	                                   float& OutPrevious,
	                                   const float DeltaTime,
	                                   const float InterpolationSpeed);

	static float GetLinearVelocitySmoothed(const FVector& Current,
	                                       FVector& OutPrevious,
	                                       float DeltaTime,
	                                       float InterpolationSpeed);

	static float GetAngularVelocitySmoothed(const FQuat& Current,
	                                        FQuat& OutPrevious,
	                                        float DeltaTime,
	                                        float InterpolationSpeed);

	static void CalculateLinearMotionData(const FVector& CurrentLocation,
	                                      const float DeltaTime,
	                                      const FMotionIntensityConfig& Config,
	                                      FMotionIntensityServiceData& ServiceData,
	                                      FMotionIntensityMotionData& OutMotionData);

	static void CalculateAngularMotionData(const FQuat& CurrentRotation,
	                                       const float DeltaTime,
	                                       const FMotionIntensityConfig& Config,
	                                       FMotionIntensityServiceData& ServiceData,
	                                       FMotionIntensityMotionData& OutMotionData);
};

UCLASS(BlueprintType)
class UMotionIntensityPreset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config", meta=(ShowOnlyInnerProperties))
	FMotionIntensityConfig MotionIntensityConfig;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Coefficients", meta=(ShowOnlyInnerProperties))
	FMotionIntensityCoefficients Coefficients;
};
