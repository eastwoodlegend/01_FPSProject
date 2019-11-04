// Will Pechell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;
class UParticleSystemComponent;
class AFPSHUD;

UCLASS()
class FPSPROJECT_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void StartJump();

	UFUNCTION()
	void StopJump();

	UFUNCTION()
	void Fire();

	UFUNCTION()
	void EndAutoFire();

	UFUNCTION()
	void Reload();

	void TimerExpired();

	void ReloadTimerExpired();

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FPSCameraComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* FPSMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* FPSWeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FVector MuzzleOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AFPSProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	UParticleSystemComponent* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	TSubclassOf<class AFPSBulletHole> BulletHole;

	//UPROPERTY(EditDefaultsOnly, Category = "HUD")
	//AFPSHUD* HUD = nullptr;

	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	FVector LineTraceEnd;
	float Reach = 100000.f;
	FHitResult Hit;
	AActor* ActorHit = nullptr;

	float Magnitude = 100.f;

	bool bAutoFire = true;
	float RateOfFire = .95f;
	bool bEndAutoFire = false;
	bool bTimerExpired;

	int32 StartingAmmo = 32;
	int32 Ammo = 0;
	bool bOutOfAmmo = false;
	bool bIsReloading = false;
	bool bReloadTimerExpired;
};
