// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Grippables/GrippableSphereComponent.h"
#include "Net/UnrealNetwork.h"

  //=============================================================================
UGrippableSphereComponent::UGrippableSphereComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	VRGripInterfaceSettings.bDenyGripping = false;
	VRGripInterfaceSettings.OnTeleportBehavior = EGripInterfaceTeleportBehavior::DropOnTeleport;
	VRGripInterfaceSettings.bSimulateOnDrop = true;
	VRGripInterfaceSettings.SlotDefaultGripType = EGripCollisionType::ManipulationGrip;
	VRGripInterfaceSettings.FreeDefaultGripType = EGripCollisionType::ManipulationGrip;
	//VRGripInterfaceSettings.bCanHaveDoubleGrip = false;
	VRGripInterfaceSettings.SecondaryGripType = ESecondaryGripType::SG_None;
	//VRGripInterfaceSettings.GripTarget = EGripTargetType::ComponentGrip;
	VRGripInterfaceSettings.MovementReplicationType = EGripMovementReplicationSettings::ForceClientSideMovement;
	VRGripInterfaceSettings.LateUpdateSetting = EGripLateUpdateSettings::LateUpdatesAlwaysOff;
	VRGripInterfaceSettings.ConstraintStiffness = 1500.0f;
	VRGripInterfaceSettings.ConstraintDamping = 200.0f;
	VRGripInterfaceSettings.ConstraintBreakDistance = 100.0f;
	VRGripInterfaceSettings.SecondarySlotRange = 20.0f;
	VRGripInterfaceSettings.PrimarySlotRange = 20.0f;
	//VRGripInterfaceSettings.bIsInteractible = false;

	bReplicateMovement = false;
	//this->bReplicates = true;

	VRGripInterfaceSettings.bIsHeld = false;
	VRGripInterfaceSettings.HoldingController = nullptr;
	bRepGripSettingsAndGameplayTags = true;
}

void UGrippableSphereComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME/*_CONDITION*/(UGrippableSphereComponent, GripLogicScripts);// , COND_Custom);
	DOREPLIFETIME(UGrippableSphereComponent, bRepGripSettingsAndGameplayTags);
	DOREPLIFETIME(UGrippableSphereComponent, bReplicateMovement);
	DOREPLIFETIME_CONDITION(UGrippableSphereComponent, VRGripInterfaceSettings, COND_Custom);
	DOREPLIFETIME_CONDITION(UGrippableSphereComponent, GameplayTags, COND_Custom);
}

void UGrippableSphereComponent::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	// Don't replicate if set to not do it
	DOREPLIFETIME_ACTIVE_OVERRIDE(UGrippableSphereComponent, VRGripInterfaceSettings, bRepGripSettingsAndGameplayTags);
	DOREPLIFETIME_ACTIVE_OVERRIDE(UGrippableSphereComponent, GameplayTags, bRepGripSettingsAndGameplayTags);

	DOREPLIFETIME_ACTIVE_OVERRIDE(USceneComponent, RelativeLocation, bReplicateMovement);
	DOREPLIFETIME_ACTIVE_OVERRIDE(USceneComponent, RelativeRotation, bReplicateMovement);
	DOREPLIFETIME_ACTIVE_OVERRIDE(USceneComponent, RelativeScale3D, bReplicateMovement);
}

bool UGrippableSphereComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (UVRGripScriptBase* Script : GripLogicScripts)
	{
		if (Script && !Script->IsPendingKill())
		{
			WroteSomething |= Channel->ReplicateSubobject(Script, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

//=============================================================================
UGrippableSphereComponent::~UGrippableSphereComponent()
{
}

void UGrippableSphereComponent::BeginPlay()
{
	// Call the base class 
	Super::BeginPlay();

	// Call all grip scripts begin play events so they can perform any needed logic
	for (UVRGripScriptBase* Script : GripLogicScripts)
	{
		if (Script)
		{
			Script->OnBeginPlay(this);
		}
	}
}

void UGrippableSphereComponent::SetDenyGripping(bool bDenyGripping)
{
	VRGripInterfaceSettings.bDenyGripping = bDenyGripping;
}

void UGrippableSphereComponent::TickGrip_Implementation(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation, float DeltaTime) {}
void UGrippableSphereComponent::OnGrip_Implementation(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation) {}
void UGrippableSphereComponent::OnGripRelease_Implementation(UGripMotionControllerComponent * ReleasingController, const FBPActorGripInformation & GripInformation, bool bWasSocketed) {}
void UGrippableSphereComponent::OnChildGrip_Implementation(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation) {}
void UGrippableSphereComponent::OnChildGripRelease_Implementation(UGripMotionControllerComponent * ReleasingController, const FBPActorGripInformation & GripInformation, bool bWasSocketed) {}
void UGrippableSphereComponent::OnSecondaryGrip_Implementation(USceneComponent * SecondaryGripComponent, const FBPActorGripInformation & GripInformation) {}
void UGrippableSphereComponent::OnSecondaryGripRelease_Implementation(USceneComponent * ReleasingSecondaryGripComponent, const FBPActorGripInformation & GripInformation) {}
void UGrippableSphereComponent::OnUsed_Implementation() {}
void UGrippableSphereComponent::OnEndUsed_Implementation() {}
void UGrippableSphereComponent::OnSecondaryUsed_Implementation() {}
void UGrippableSphereComponent::OnEndSecondaryUsed_Implementation() {}
void UGrippableSphereComponent::OnInput_Implementation(FKey Key, EInputEvent KeyEvent) {}
bool UGrippableSphereComponent::RequestsSocketing_Implementation(USceneComponent *& ParentToSocketTo, FName & OptionalSocketName, FTransform_NetQuantize & RelativeTransform) { return false; }

bool UGrippableSphereComponent::DenyGripping_Implementation()
{
	return VRGripInterfaceSettings.bDenyGripping;
}

EGripInterfaceTeleportBehavior UGrippableSphereComponent::TeleportBehavior_Implementation()
{
	return VRGripInterfaceSettings.OnTeleportBehavior;
}

bool UGrippableSphereComponent::SimulateOnDrop_Implementation()
{
	return VRGripInterfaceSettings.bSimulateOnDrop;
}

EGripCollisionType UGrippableSphereComponent::GetPrimaryGripType_Implementation(bool bIsSlot)
{
	return bIsSlot ? VRGripInterfaceSettings.SlotDefaultGripType : VRGripInterfaceSettings.FreeDefaultGripType;
}

ESecondaryGripType UGrippableSphereComponent::SecondaryGripType_Implementation()
{
	return VRGripInterfaceSettings.SecondaryGripType;
}

EGripMovementReplicationSettings UGrippableSphereComponent::GripMovementReplicationType_Implementation()
{
	return VRGripInterfaceSettings.MovementReplicationType;
}

EGripLateUpdateSettings UGrippableSphereComponent::GripLateUpdateSetting_Implementation()
{
	return VRGripInterfaceSettings.LateUpdateSetting;
}

void UGrippableSphereComponent::GetGripStiffnessAndDamping_Implementation(float &GripStiffnessOut, float &GripDampingOut)
{
	GripStiffnessOut = VRGripInterfaceSettings.ConstraintStiffness;
	GripDampingOut = VRGripInterfaceSettings.ConstraintDamping;
}

FBPAdvGripSettings UGrippableSphereComponent::AdvancedGripSettings_Implementation()
{
	return VRGripInterfaceSettings.AdvancedGripSettings;
}

float UGrippableSphereComponent::GripBreakDistance_Implementation()
{
	return VRGripInterfaceSettings.ConstraintBreakDistance;
}

void UGrippableSphereComponent::ClosestGripSlotInRange_Implementation(FVector WorldLocation, bool bSecondarySlot, bool & bHadSlotInRange, FTransform & SlotWorldTransform, UGripMotionControllerComponent * CallingController, FName OverridePrefix)
{
	if (OverridePrefix.IsNone())
		bSecondarySlot ? OverridePrefix = "VRGripS" : OverridePrefix = "VRGripP";

	UVRExpansionFunctionLibrary::GetGripSlotInRangeByTypeName_Component(OverridePrefix, this, WorldLocation, bSecondarySlot ? VRGripInterfaceSettings.SecondarySlotRange : VRGripInterfaceSettings.PrimarySlotRange, bHadSlotInRange, SlotWorldTransform);
}

/*bool UGrippableSphereComponent::IsInteractible_Implementation()
{
	return VRGripInterfaceSettings.bIsInteractible;
}*/

void UGrippableSphereComponent::IsHeld_Implementation(UGripMotionControllerComponent *& HoldingController, bool & bIsHeld)
{
	HoldingController = VRGripInterfaceSettings.HoldingController;
	bIsHeld = VRGripInterfaceSettings.bIsHeld;
}

void UGrippableSphereComponent::SetHeld_Implementation(UGripMotionControllerComponent * HoldingController, bool bIsHeld)
{
	if (bIsHeld)
		VRGripInterfaceSettings.HoldingController = HoldingController;
	else
		VRGripInterfaceSettings.HoldingController = nullptr;

	VRGripInterfaceSettings.bIsHeld = bIsHeld;
}

/*FBPInteractionSettings UGrippableSphereComponent::GetInteractionSettings_Implementation()
{
	return VRGripInterfaceSettings.InteractionSettings;
}*/


bool UGrippableSphereComponent::GetGripScripts_Implementation(TArray<UVRGripScriptBase*> & ArrayReference)
{
	ArrayReference = GripLogicScripts;
	return GripLogicScripts.Num() > 0;
}
