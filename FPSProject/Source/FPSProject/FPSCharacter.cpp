// Will Pechell

#include "FPSProject.h"
#include "FPSCharacter.h"
#include "Math/RotationMatrix.h"
#include "Components/InputComponent.h"
#include "Math/Matrix.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FPSProjectile.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "FPSBulletHole.h"
#include "FPSHUD.h"

//TODO remove after debug line testing.
//#include "DrawDebugHelpers.h" 

// Sets default values
AFPSCharacter::AFPSCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FPSCameraComponent->SetupAttachment(GetCapsuleComponent());

	FPSCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f + BaseEyeHeight));
	FPSCameraComponent->bUsePawnControlRotation = true;

	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FPSMesh->SetOnlyOwnerSee(true);
	FPSMesh->SetupAttachment(FPSCameraComponent);
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	FPSWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	FPSWeaponMesh->SetOnlyOwnerSee(true);
	FPSWeaponMesh->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GripPoint"));
	FPSWeaponMesh->bCastDynamicShadow = false;
	FPSWeaponMesh->CastShadow = false;

	MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MuzzleFlash"));
	MuzzleFlash->AttachToComponent(FPSWeaponMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Muzzle"));
	
	GetMesh()->SetOwnerNoSee(true);
	Ammo = StartingAmmo;

	//HUD->UpdateAmmo(Ammo);
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);

	LineTraceEnd = PlayerViewLocation + (PlayerViewRotation.Vector() * Reach);

	//TODO Need to remove this eventually
	//DrawDebugLine(GetWorld(), PlayerViewLocation, LineTraceEnd, FColor(255, 0, 0), false, 0, 0, 1.f);

	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(OUT Hit, PlayerViewLocation, LineTraceEnd, FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParams);

	ActorHit = Hit.GetActor();

	if (!bEndAutoFire && bTimerExpired)
	{
		Fire();
	}

	if (bReloadTimerExpired)
	{
		bIsReloading = false;
	}
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &AFPSCharacter::StopJump);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AFPSCharacter::Fire);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AFPSCharacter::EndAutoFire);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &AFPSCharacter::Reload);
}

void AFPSCharacter::MoveForward(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::MoveRight(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::StartJump()
{
	bPressedJump = true;
}

void AFPSCharacter::StopJump()
{
	bPressedJump = false;
}

void AFPSCharacter::Fire()
{
	if (!bOutOfAmmo && !bIsReloading)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rounds Left: %i"), Ammo);
		MuzzleFlash->ActivateSystem(false);
		if (bAutoFire)
		{
			bEndAutoFire = false;
			FTimerHandle Timer;
			GetWorld()->GetTimerManager().SetTimer(Timer, this, &AFPSCharacter::TimerExpired, 1.f - RateOfFire, false);
			bTimerExpired = false;
		}
		UE_LOG(LogTemp, Warning, TEXT("Fired at time: %f"), GetWorld()->GetTimeSeconds());
		UPrimitiveComponent* PrimComp = nullptr;
		if (ActorHit)
		{
			PrimComp = ActorHit->FindComponentByClass<UPrimitiveComponent>();
		}
		if (PrimComp)
		{
			PrimComp->AddImpulseAtLocation(Hit.Location * Magnitude, Hit.ImpactPoint);
			UE_LOG(LogTemp, Warning, TEXT("Actor Hit: %s"), *ActorHit->GetName());
			auto BulletHoleMark = GetWorld()->SpawnActor<AFPSBulletHole>(BulletHole, Hit.Location, FRotator::ZeroRotator);
			BulletHoleMark->AttachToActor(ActorHit, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
		}

		if (--Ammo == 0)
		{
			bOutOfAmmo = true;
		}
	}
	if(bOutOfAmmo)
	{
		UE_LOG(LogTemp, Warning, TEXT("Out of Ammo!!"));
	}
	if (bIsReloading)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reloading!"));
	}

	
}

void AFPSCharacter::EndAutoFire()
{
	bEndAutoFire = true;
}

void AFPSCharacter::TimerExpired()
{
	bTimerExpired = true;
}

void AFPSCharacter::Reload()
{
	Ammo = StartingAmmo;
	bOutOfAmmo = false;
	bIsReloading = true;
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AFPSCharacter::ReloadTimerExpired, 2.f, false);
	bReloadTimerExpired = false;
}

void AFPSCharacter::ReloadTimerExpired()
{
	bReloadTimerExpired = true;
}