// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	//Look for attached physics handle
	physicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!physicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *GetOwner()->GetName());
	}
	//Look for attached physics component
	inputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (inputComponent)
	{
		inputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing physics input component"), *GetOwner()->GetName());

	}
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab pressed"));
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;
	
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		playerViewPointLocation,
		playerViewPointRotation
	);

	FVector LineTraceEnd = playerViewPointLocation + playerViewPointRotation.Vector() * reach;

	//Draw a red debug line
	DrawDebugLine(
		GetWorld(),
		playerViewPointLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0,
		2.0f
	);

	//Setup query params
	FCollisionQueryParams traceParams(FName(TEXT("")), false, GetOwner());

	//Line-trace(raycast) out to reach distance
	FHitResult hit;
	GetWorld()->LineTraceSingleByObjectType(
		hit,
		playerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		traceParams
	);

	// See what we hit
	AActor* actorHit = hit.GetActor();
	if (actorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *(actorHit->GetName()))
	}
}

