// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "Runtime/Engine/Classes/Components/PrimitiveComponent.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

// Look for attached physics handle
void UGrabber::FindPhysicsHandleComponent()
{
	physicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (physicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *GetOwner()->GetName());
	}
}

// Look for attached physics input component
void UGrabber::SetupInputComponent()
{
	inputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (inputComponent)
	{
		inputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		inputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing physics input component"), *GetOwner()->GetName());
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!physicsHandle) { return; }
	// if the physics handle is attached
	if (physicsHandle->GrabbedComponent)
	{
		// move the object that we're holding
		physicsHandle->SetTargetLocation(GetReachLineEnd());
	}
}

void UGrabber::Grab()
{
	// Line Trace and see if we reach any actors with physics
	auto hitResult = GetFirstPhysicsBodyInReach();
	auto componentToGrab = hitResult.GetComponent(); // Gets the mesh component
	auto actorHit = hitResult.GetActor();

	// If we hit something then attach a physics handle
	if (actorHit)
	{
		if (!physicsHandle) { return; }
		physicsHandle->GrabComponent // GrabComponentAtLocation() is newer
		(
			componentToGrab,
			NAME_None, // no bones needed
			componentToGrab->GetOwner()->GetActorLocation(),
			true // constrain rotation
		);
	}
}

void UGrabber::Release()
{
	if (!physicsHandle) { return; }
	physicsHandle->ReleaseComponent();
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	// Line-trace(raycast) out to reach distance
	FHitResult hit;
	FCollisionQueryParams traceParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		hit,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		traceParams
	);
	return hit;
}

FVector UGrabber::GetReachLineStart()
{
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint
	(
		playerViewPointLocation,
		playerViewPointRotation
	);
	return playerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd()
{
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint
	(
		playerViewPointLocation,
		playerViewPointRotation
	);
	return playerViewPointLocation + playerViewPointRotation.Vector() * reach;
}