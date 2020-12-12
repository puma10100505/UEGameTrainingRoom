// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "UEGameTrainingRoom/Weapons/WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "Math/UnrealMathUtility.h"

DEFINE_LOG_CATEGORY(LogCharacter);

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // (Without a weapon)
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;
}

void ACharacterBase::InitializeWeapon()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;	

	if (GetWorld() == nullptr)
	{
		UE_LOG(LogCharacter, Log, TEXT("World is not exist"));
		return;
	}

	PrimaryWeapon = GetWorld()->SpawnActor<AWeaponBase>(InitialWeaponClass, Params);

	if (IsValid(PrimaryWeapon) == false)
	{
		UE_LOG(LogCharacter, Log, TEXT("Spawn primary weapon failed"));
		return;
	}

	PrimaryWeapon->AttachTo(this);

	UE_LOG(LogCharacter, Log, TEXT("After spawn primary weapon, role: %d, netmode: %d"), GetLocalRole(), GetNetMode());
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	
	OnHealthChangedDelegator.Broadcast(Health, MaxHealth);
	OnArmorChangedDelegator.Broadcast(Armor, MaxArmor);
}

void ACharacterBase::InitializeAttributes()
{
	Health = MaxHealth;
	Armor = MaxArmor;
	bIsAiming = false;
	bIsFiring = false;
	bIsPreparingFire = false;
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;


	UE_LOG(LogCharacter, Log, TEXT("After init, health: %f, armor: %f"), Health, Armor);
}

void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetLocalRole() == ROLE_Authority) 
	{
		InitializeAttributes();

		GetWorldTimerManager().SetTimerForNextTick(this, &ACharacterBase::InitializeWeapon);
	}
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacterBase::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACharacterBase::OnStopFire);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ACharacterBase::SwitchToAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ACharacterBase::RecoveryFromAiming);

}

void ACharacterBase::OnStartFire()
{
	UE_LOG(LogWeapon, Log, TEXT("Entry of fire in character, Role: %d, NetMode: %d"), GetLocalRole(), GetNetMode());

	if (CanFire() == false)
	{
		return;
	}

	if (IsValid(PrimaryWeapon) == false)
	{
		UE_LOG(LogWeapon, Error, TEXT("PrimaryWeapon is not exist"));
		return;
	}

	GetWorldTimerManager().SetTimer(TimerHandle_Fire, this, &ACharacterBase::Fire, 
		PrimaryWeapon->GetWeaponModelData().FireInterval, true, 0.f);
}

void ACharacterBase::OnStopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Fire);
}

void ACharacterBase::Fire()
{
	if (IsValid(PrimaryWeapon))
	{
		PrimaryWeapon->Fire();
	}
	else 
	{
		UE_LOG(LogWeapon, Log, TEXT("Weapon is not valid"));
	}
}

void ACharacterBase::MoveForward(float Value)
{
	if (IsValid(Controller) && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);

		InterplateMoveSpeed();
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if (IsValid(Controller) && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		InterplateMoveSpeed();
	}
}

void ACharacterBase::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ACharacterBase::CanFire()
{
	if (bIsFiring)
	{
		UE_LOG(LogWeapon, Log, TEXT("Current character is firing"));
		return false;
	}

	if (bIsPreparingFire == false)
	{
		UE_LOG(LogWeapon, Log, TEXT("Current character is not prepare for shooting"));
		return false;
	}

	return true;
}

bool ACharacterBase::CanReload()
{
	return true;
}

void ACharacterBase::SwitchToAiming()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerSwitchToAiming();
	}

	bIsPreparingFire = true;
	bUseControllerRotationYaw = true;
	if (GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	WalkSpeed = 150;
}

void ACharacterBase::RecoveryFromAiming()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerRecoveryFromAiming();
	}

	bIsPreparingFire = false;
	bUseControllerRotationYaw = false;
	if (GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	WalkSpeed = 600;
}

// 同步回调，在属性同步之后执行，参数是旧值，PrimaryWeapon是新值
void ACharacterBase::OnRep_PrimaryWeapon(AWeaponBase* LastWeapon)
{
	UE_LOG(LogCharacter, Log, TEXT("Entry of OnRep PrimaryWeapon, Role: %d, NetMode: %d, PrimaryWeapon is null: %d, LastWeapon is null: %d"), 
		GetLocalRole(), GetNetMode(), PrimaryWeapon == nullptr, LastWeapon == nullptr);
}

void ACharacterBase::InterplateMoveSpeed()
{
	float InterpVal = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, 
		WalkSpeed, GetWorld()->GetDeltaSeconds(), 0.9f);
	GetCharacterMovement()->MaxWalkSpeed = InterpVal;
	
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerInterplateMoveSpeed();
	}
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, Health);
	DOREPLIFETIME(ACharacterBase, Armor);
	DOREPLIFETIME(ACharacterBase, bIsFiring);
	DOREPLIFETIME(ACharacterBase, bIsAiming);
	DOREPLIFETIME(ACharacterBase, PrimaryWeapon);
	DOREPLIFETIME(ACharacterBase, WalkSpeed);
	DOREPLIFETIME(ACharacterBase, bIsDead);
}

bool ACharacterBase::ServerModifyMoveSpeed_Validate(float NewSpeed)
{
	return true;
}

void ACharacterBase::ServerModifyMoveSpeed_Implementation(float NewSpeed)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	}
}

bool ACharacterBase::ServerInterplateMoveSpeed_Validate()
{
	return true;
}

void ACharacterBase::ServerInterplateMoveSpeed_Implementation()
{
	InterplateMoveSpeed();
}

bool ACharacterBase::ServerSwitchToAiming_Validate()
{
	return true;
}

void ACharacterBase::ServerSwitchToAiming_Implementation()
{
	SwitchToAiming();
}

bool ACharacterBase::ServerRecoveryFromAiming_Validate()
{
	return true;
}

void ACharacterBase::ServerRecoveryFromAiming_Implementation()
{
	RecoveryFromAiming();
}

void ACharacterBase::TakeHurt(const AWeaponBase* SourceWeapon, float Distance)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogCharacter, Log, TEXT("Character was hit by weapon, role: %d"), GetLocalRole());
		if (SourceWeapon)
		{
			const FWeaponModelData& WeaponData = SourceWeapon->GetWeaponModelData();
			float RealDamage = (1.0f - Distance / WeaponData.ShootRange + 0.1f) * WeaponData.Damage;

			RealDamage = FMath::Clamp<float>(RealDamage, 0.f, WeaponData.Damage);

			UE_LOG(LogCharacter, Log, TEXT("Distance: %f, ShootRange: %f, WeaponDamage: %f, RealDamage: %f"), 
				Distance, WeaponData.ShootRange, WeaponData.Damage, RealDamage);

			ChangeLife(-RealDamage);

			if (Health <= 0.f)
			{
				KillToDeath();
			}
		}
	}
}

void ACharacterBase::ChangeLife(float Value)
{
	float OldArmor = Armor;
	float OldHealth = Health;

	if (Armor > 0)
	{
		Armor += Value;
		if (Armor < 0)
		{
			Health += Armor;
		}
	}
	else 
	{
		Health += Value;
	}

	if (Health < 0) 
	{
		Health = 0;
	}
	
	UE_LOG(LogCharacter, Log, TEXT("OldArmor: %f, CurrArmor: %f, OldHealth: %f, CurrHealth: %f"), 
		OldArmor, Armor, OldHealth, Health);
}

void ACharacterBase::KillToDeath()
{
	bIsDead = true;
	UE_LOG(LogCharacter, Log, TEXT("Character is killed to death"));

}

void ACharacterBase::OnRep_HealthChanged(float OldHealth)
{
	if (OldHealth != Health)
	{
		OnHealthChangedDelegator.Broadcast(Health, MaxHealth);
	}
}

void ACharacterBase::OnRep_ArmorChanged(float OldArmor)
{
	if (OldArmor != Armor)
	{
		OnArmorChangedDelegator.Broadcast(Armor, MaxArmor);
	}
}