// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine.h"

#include "VRBPDatatypes.generated.h"

USTRUCT()
struct VREXPANSIONPLUGIN_API FBPVRComponentPosRep
{
	GENERATED_BODY()
public:
	UPROPERTY()
		FVector_NetQuantize100 Position;
	UPROPERTY()
		uint32 YawPitchINT;
	UPROPERTY()
		uint8 RollBYTE;
//		FRotator Orientation;


	FORCEINLINE void SetRotation(FRotator NewRot)
	{
		YawPitchINT = (FRotator::CompressAxisToShort(NewRot.Yaw) << 16) | FRotator::CompressAxisToShort(NewRot.Pitch);
		RollBYTE = FRotator::CompressAxisToByte(NewRot.Roll);
	}

	FORCEINLINE FRotator GetRotation()
	{
		const uint16 nPitch = (YawPitchINT & 65535);
		const uint16 nYaw = (YawPitchINT >> 16);

		return FRotator(FRotator::DecompressAxisFromShort(nPitch), FRotator::DecompressAxisFromShort(nYaw), FRotator::DecompressAxisFromByte(RollBYTE));
	}
};

/*
Interactive Collision With Physics = Held items can be offset by geometry, uses physics for the offset, pushes physics simulating objects with weight taken into account
Interactive Collision With Sweep = Held items can be offset by geometry, uses sweep for the offset, pushes physics simulating objects, no weight
Sweep With Physics = Only sweeps movement, will not be offset by geomtry, still pushes physics simulating objects, no weight
Physics Only = Does not sweep at all (does not trigger OnHitEvents), still pushes physics simulating objects, no weight
*/
UENUM(Blueprintable)
enum EGripCollisionType
{
	InteractiveCollisionWithPhysics,
	InteractiveCollisionWithSweep,
	InteractiveHybridCollisionWithSweep,
	SweepWithPhysics,
	PhysicsOnly
};

// This needs to be updated as the original gets changed, that or hope they make the original blueprint accessible.
UENUM(Blueprintable)
enum EBPHMDDeviceType
{
	DT_OculusRift,
	DT_Morpheus,
	DT_ES2GenericStereoMesh,
	DT_SteamVR,
	DT_GearVR,
	DT_Unknown
};

// This needs to be updated as the original gets changed, that or hope they make the original blueprint accessible.
UENUM(Blueprintable)
enum EGripLerpState
{
	StartLerp,
	EndLerp,
	ConstantLerp,
	NotLerping
};


USTRUCT(BlueprintType, Category = "VRExpansionLibrary")
struct VREXPANSIONPLUGIN_API FBPInteractionSettings
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bCanUse;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bLimitsInLocalSpace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LinearSettings")
		bool bLimitX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LinearSettings")
		bool bLimitY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LinearSettings")
		bool bLimitZ;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AngularSettings")
		bool bLimitPitch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AngularSettings")
		bool bLimitYaw;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AngularSettings")
		bool bLimitRoll;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AngularSettings")
		FVector CustomPivot;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LinearSettings")
		FVector InitialLinearTranslation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LinearSettings")
		FVector MinLinearTranslation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LinearSettings")
		FVector MaxLinearTranslation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AngularSettings")
		FRotator InitialAngularTranslation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AngularSettings")
		FRotator MinAngularTranslation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AngularSettings")
		FRotator MaxAngularTranslation;

	FBPInteractionSettings()
	{
		bCanUse = false;
		bLimitsInLocalSpace = true;

		bLimitX = false;
		bLimitY = false;
		bLimitZ = false;

		bLimitPitch = false;
		bLimitYaw = false;
		bLimitRoll = false;

		CustomPivot = FVector::ZeroVector;

		InitialLinearTranslation = FVector::ZeroVector;
		MinLinearTranslation = FVector::ZeroVector;
		MaxLinearTranslation = FVector::ZeroVector;

		InitialAngularTranslation = FRotator::ZeroRotator;
		MinAngularTranslation = FRotator::ZeroRotator;
		MaxAngularTranslation = FRotator::ZeroRotator;
	}

};

USTRUCT(BlueprintType, Category = "VRExpansionLibrary")
struct VREXPANSIONPLUGIN_API FBPActorGripInformation
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		AActor * Actor;
	UPROPERTY(BlueprintReadOnly)
		UPrimitiveComponent * Component;
	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte<EGripCollisionType> GripCollisionType;
	UPROPERTY(BlueprintReadOnly)
		bool bColliding;
	UPROPERTY(BlueprintReadOnly)
		FTransform RelativeTransform;
	UPROPERTY(BlueprintReadOnly)
		bool bOriginalReplicatesMovement;
	UPROPERTY(BlueprintReadOnly)
		bool bTurnOffLateUpdateWhenColliding;
	UPROPERTY(BlueprintReadOnly)
		float Damping;
	UPROPERTY(BlueprintReadOnly)
		float Stiffness;

	// For multi grip situations
	UPROPERTY(BlueprintReadOnly)
		bool bHasSecondaryAttachment;
	UPROPERTY(BlueprintReadOnly)
		USceneComponent * SecondaryAttachment;
	UPROPERTY(BlueprintReadOnly)
		float SecondarySmoothingScaler;
	UPROPERTY(BlueprintReadOnly)
		bool bTurnOffLateUpdateWhenDoubleGrip;
	UPROPERTY()
		FVector SecondaryRelativeLocation;

	// Lerp transitions
	UPROPERTY(BlueprintReadOnly)
		float LerpToRate;
	
	// These are not replicated, they don't need to be
	TEnumAsByte<EGripLerpState> GripLerpState;
	FVector LastRelativeLocation;
	float curLerp;

	// Locked transitions
	bool bIsLocked;
	FQuat LastLockedRotation;

	/** Network serialization */
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << Actor;
		Ar << Component;
		Ar << GripCollisionType;

		// Is being set locally
		//Ar << bColliding;

		Ar << RelativeTransform;

		// This doesn't matter to clients
		//Ar << bOriginalReplicatesMovement;

		Ar << bTurnOffLateUpdateWhenColliding;
		
		bool bHadAttachment = bHasSecondaryAttachment;
	
		Ar << bHasSecondaryAttachment;
		Ar << LerpToRate;

		// If this grip has a secondary attachment
		if (bHasSecondaryAttachment)
		{
			Ar << SecondaryAttachment;
			Ar << SecondaryRelativeLocation;
			Ar << SecondarySmoothingScaler;
			Ar << bTurnOffLateUpdateWhenDoubleGrip;
		}

		// Manage lerp states
		if (Ar.IsLoading())
		{
			if (bHadAttachment != bHasSecondaryAttachment)
			{
				if (LerpToRate < 0.01f)
					GripLerpState = EGripLerpState::NotLerping;
				else
				{
					// New lerp
					if (bHasSecondaryAttachment)
					{
						curLerp = LerpToRate;
						GripLerpState = EGripLerpState::StartLerp;
					}
					else // Post Lerp
					{
						curLerp = LerpToRate;
						GripLerpState = EGripLerpState::EndLerp;
					}
				}
			}
		}

		// Don't bother replicated physics grip types if the grip type doesn't support it.
		if (GripCollisionType == EGripCollisionType::InteractiveCollisionWithPhysics || GripCollisionType == EGripCollisionType::InteractiveHybridCollisionWithSweep)
		{
			Ar << Damping;
			Ar << Stiffness;
		}

		bOutSuccess = true;
		return true;
	}

	//Check if a grip is the same as another, the only things I check for are the actor / component
	//This is here for the Find() function from TArray
	FORCEINLINE bool operator==(const FBPActorGripInformation &Other) const
	{
		if (Actor && Actor == Other.Actor)
			return true;

		if (Component && Component == Other.Component)
			return true;

		return false;
	}

	FBPActorGripInformation()
	{
		bTurnOffLateUpdateWhenColliding = true;
		Damping = 200.0f;
		Stiffness = 1500.0f;
		Component = nullptr;
		Actor = nullptr;
		bColliding = false;
		GripCollisionType = EGripCollisionType::InteractiveCollisionWithSweep;
		bIsLocked = false;
		curLerp = 0.0f;
		LerpToRate = 0.0f;
		GripLerpState = EGripLerpState::NotLerping;
		SecondarySmoothingScaler = 1.0f;
		bTurnOffLateUpdateWhenDoubleGrip = false;

		SecondaryAttachment = nullptr;
		bHasSecondaryAttachment = false;
	}
};

template<>
struct TStructOpsTypeTraits< FBPActorGripInformation > : public TStructOpsTypeTraitsBase
{
	enum
	{
		WithNetSerializer = true
	};
};

USTRUCT(BlueprintType, Category = "VRExpansionLibrary")
struct VREXPANSIONPLUGIN_API FBPActorPhysicsHandleInformation
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		AActor * Actor;
	UPROPERTY(BlueprintReadOnly)
		UPrimitiveComponent * Component;

	/** Physics scene index of the body we are grabbing. */
	int32 SceneIndex;
	/** Pointer to PhysX joint used by the handle*/
	physx::PxD6Joint* HandleData;
	/** Pointer to kinematic actor jointed to grabbed object */
	physx::PxRigidDynamic* KinActorData;

	FBPActorPhysicsHandleInformation()
	{
		HandleData = NULL;
		KinActorData = NULL;
		Actor = nullptr;
		Component = nullptr;
	}
};