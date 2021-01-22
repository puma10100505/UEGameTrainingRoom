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
#include "Components/ArrowComponent.h"
#include "BrainComponent.h"
#include "Components/WidgetComponent.h"
#include "PlayerHeadUpUI.h"
#include "Blueprint/UserWidget.h"
#include "AIControllerBase.h"
#include "PlayerStateBase.h"
#include "AbilityComponent.h"
#include "AbilitySystemComponent.h"
#include "PlayerControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Attributes/AttributeSetHealth.h"
// #include "Attributes/AttributeSetArmor.h"

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
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // (Without a weapon)
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//GetMesh()->SetCollisionObjectType(ECC_WorldDynamic)

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;
	
	if (IsValid(PrimaryWeapon))
	{	
		FollowCamera->SetFieldOfView(PrimaryWeapon->GetWeaponModelData().DefaultFOV);
	}

	// Setting of HeadUp UI
	HeadUpWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HeadUpWidgetComponent"));
	HeadUpWidgetComponent->SetupAttachment(GetMesh(), FName(TEXT("UI_Head")));

	AbilityComp = CreateDefaultSubobject<UAbilityComponent>(TEXT("Ability"));
	
}

// Server Only
void ACharacterBase::InitializeASCFromPlayerState()
{
	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
	if (PS)
	{
		// Set the ASC and AttributeSet ON SERVER!
		AbilitySystem = PS->GetAbilitySystemComponent();

		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		AttributeSetHealth = PS->GetAttributeSetHealth();
		// AttributeSetArmor = PS->GetAttributeSetArmor();

		// if (PrimaryWeapon)
		// {
		// 	UAttributeSetWeapon* AttributeSetWeapon = PrimaryWeapon->GetWeaponAttributeSet();
		// 	if (IsValid(AttributeSetWeapon))
		// 	{
		// 		AbilitySystem->AddAttributeSetSubobject<UAttributeSetWeapon>(AttributeSetWeapon);
		// 		UE_LOG(LogCharacter, Log, TEXT("After add weapon attributeset into ASC of character"));
		// 	}
		// }

		if (DefaultAttributes)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystem->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			FGameplayEffectSpecHandle NewHandle = AbilitySystem->MakeOutgoingSpec(DefaultAttributes, 1.f, EffectContext);
			if (NewHandle.IsValid())
			{
				//FActiveGameplayEffectHandle ActiveGEHandle = 
				AbilitySystem->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
			}
		}
	}
}

void ACharacterBase::InitializeHeadUpUI()
{
 	if (HeadUpUIInstance || IsValid(AbilitySystem) == false)
	{
		return;
	}

	// 自己控制的人物头上不显示这个UI
	if (IsPlayerControlled() && IsLocallyControlled())
	{
		return;
	}

	if (IsValid(GetPlayerState()) == false)
	{
		return;
	}

	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC && PC->IsLocalPlayerController())
	{
		if (HeadUpUIClass)
		{
			HeadUpUIInstance = CreateWidget<UPlayerHeadUpUI>(PC, HeadUpUIClass);
			if (HeadUpUIInstance && HeadUpWidgetComponent)
			{
				HeadUpWidgetComponent->SetWidget(HeadUpUIInstance);

				// 设置HeadUpUI的初始值
				HeadUpUIInstance->SetHealthPercentage(GetAttributeSetHealth()->GetHealth(), GetAttributeSetHealth()->GetMaxHealth());
				HeadUpUIInstance->SetArmorPercentage(GetAttributeSetHealth()->GetArmor(), GetAttributeSetHealth()->GetMaxArmor());
				HeadUpUIInstance->SetOwningCharacter(this);
			}
		}
	}
}

void ACharacterBase::BindAbilityInput()
{
	if (bAbilityInputBound == false && IsValid(AbilitySystem) && IsValid(InputComponent))
	{
		AbilitySystem->BindAbilityActivationToInputComponent(InputComponent,
			FGameplayAbilityInputBinds(FString("Jump"), FString(""), FString("EAbilityInputID")));

		bAbilityInputBound = true;
	}
}

void ACharacterBase::InitializeDefaultAbilities()
{
	if (GetLocalRole() != ROLE_Authority || IsValid(AbilitySystem) == false || bAbilityGranted)
	{
		return;		
	}

	// 初始化开火技能
	if (FireAbility)
	{
		GameplayAbility_FireHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(FireAbility, 1.f, INDEX_NONE, this));
	}

	if (ADSAbility)
	{
		GameplayAbility_ADSHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(ADSAbility, 1.f, INDEX_NONE, this));
	}

	if (ReloadAbility)
	{
		GameplayAbility_ReloadHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(ReloadAbility, 1.f, INDEX_NONE, this));
	}

	bAbilityGranted = true;
}

// Client Only
void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 初始化ASC和属性以及执行用于设置初始数据的GE（DefaultAttributes配置）
	InitializeASCFromPlayerState();

	OnClientAttributeDataPreparedEvent();

	// 创建头顶UI
	InitializeHeadUpUI();
	
	InitializeDefaultAbilities();

	// TODO: 创建主UI
}

// Server Only
void ACharacterBase::PossessedBy(class AController* InController)
{
	Super::PossessedBy(InController);

	InitializeASCFromPlayerState();

	OnServerAttributeDataPreparedEvent();

	InitializeHeadUpUI();

	InitializeDefaultAbilities();
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

	
	
	// 初始化头顶UI的委托
	// if (GetLocalRole() < ROLE_Authority)
	// {
	// 	if (HeadUpWidgetComponent && HeadUpWidgetComponent->GetUserWidgetObject())
	// 	{
	// 		if (UPlayerHeadUpUI* HeadUpUIInst = Cast<UPlayerHeadUpUI>(HeadUpWidgetComponent->GetUserWidgetObject()))
	// 		{
	// 			HeadUpUIInst->InitializeModelDelegates(this);
	// 		}
	// 	}
	// }

	if (GetLocalRole() == ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_SetPrepareForBattle, 
			this, &ACharacterBase::SetPreparedForBattle, 5.f, false);
	}

	// OnHealthChangedDelegator.Broadcast(Health, MaxHealth);
	// OnArmorChangedDelegator.Broadcast(Armor, MaxArmor);
}

void ACharacterBase::SetPreparedForBattle()
{
	bIsPreparedForBattle = true;
	GetWorldTimerManager().ClearTimer(TimerHandle_SetPrepareForBattle);
}

void ACharacterBase::InitializeAttributes()
{
	// Health = MaxHealth;
	// Armor = MaxArmor;
	bIsAiming = false;
	bIsFiring = false;
	bIsPreparingFire = false;
	bIsPreparedForBattle = false;
	bWantsToAim = false;
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	if (HeadUpUIClass)
	{
		HeadUpWidgetComponent->SetWidgetClass(HeadUpUIClass);
		HeadUpWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		HeadUpWidgetComponent->SetDrawSize(FVector2D(150.f, 20.f));
		HeadUpWidgetComponent->SetVisibility(true, true);
	}


	// UE_LOG(LogCharacter, Log, TEXT("After init, health: %f, armor: %f"), Health, Armor);
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

	if (GetLocalRole() < ROLE_Authority)
	{
		if (IsValid(PrimaryWeapon))
		{
			float TargetFOV = bWantsToAim ? PrimaryWeapon->GetWeaponModelData().AimingFOV 
				: PrimaryWeapon->GetWeaponModelData().DefaultFOV;
			float InterpFOV = FMath::FInterpTo(GetFollowCamera()->FieldOfView, TargetFOV,
				DeltaTime, PrimaryWeapon->GetWeaponModelData().AimingSpeed);

			GetFollowCamera()->SetFieldOfView(InterpFOV);
		}
	}
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

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::ActivateFireAbility);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACharacterBase::CancelFireAbility);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ACharacterBase::ActivateADSAbility);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ACharacterBase::DeactivateADSAbility);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ACharacterBase::ActivateReloadAbility);

	PlayerInputComponent->BindAction("AbilityRocket", IE_Pressed, this, &ACharacterBase::LaunchRocket);
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

	// GetWorldTimerManager().SetTimer(TimerHandle_Fire, this, &ACharacterBase::Fire, 
	// 	PrimaryWeapon->GetWeaponModelData().FireInterval, true, 0.f);

	Fire();
}

void ACharacterBase::OnStopFire()
{
	//GetWorldTimerManager().ClearTimer(TimerHandle_Fire);
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

bool ACharacterBase::CanFire() const
{
	if (bIsPreparedForBattle == false)
	{
		UE_LOG(LogCharacter, Log, TEXT("Not prepared for battle"));
		return false;
	}

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

	if (GetCurrentWeapon())
	{
		return GetCurrentWeapon()->CanFire();
	}

	return true;
}

bool ACharacterBase::CanReload() const
{
	bool ret = true;
	if (bIsPreparedForBattle == false)
	{
		ret = false;
	}

	if (IsValid(GetCurrentWeapon()) == false)
	{
		// return GetCurrentWeapon()->CanReload();
		ret = false;
	}

	if (GetCurrentWeapon()->CanReload() == false)
	{
		ret = false;
	}

	return ret;
}

void ACharacterBase::ActivateADSAbility()
{
	if (GameplayAbility_ADSHandle.IsValid() && IsValid(AbilitySystem))
	{
		if (AbilitySystem->TryActivateAbility(GameplayAbility_ADSHandle) == false)
		{
			UE_LOG(LogCharacter, Warning, TEXT("Activate fire ability failed"));
		}
	}
}

void ACharacterBase::DeactivateADSAbility()
{
	if (GameplayAbility_ADSHandle.IsValid() && IsValid(AbilitySystem))
	{
		AbilitySystem->CancelAbilityHandle(GameplayAbility_ADSHandle);

		UE_LOG(LogCharacter, Log, TEXT("Cancel ADS Ability"));
	}
}

void ACharacterBase::SwitchToAiming()
{
	if (bIsPreparedForBattle == false)
	{
		UE_LOG(LogCharacter, Warning, TEXT("Not prepared for battle"));
		return;
	}

	// if (GetLocalRole() < ROLE_Authority)
	// {
	// 	ServerSwitchToAiming();
	// }

	if (IsValid(PrimaryWeapon) == false)
	{
		UE_LOG(LogCharacter, Warning, TEXT("PrimaryWeapon is not valid"));
		return;
	}
		
	bWantsToAim = true;
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
	if (bIsPreparedForBattle == false)
	{
		UE_LOG(LogCharacter, Warning, TEXT("Not prepared for battle"));
		return;
	}

	// if (GetLocalRole() < ROLE_Authority)
	// {
	// 	ServerRecoveryFromAiming();
	// }

	if (IsValid(PrimaryWeapon) == false)
	{
		UE_LOG(LogCharacter, Warning, TEXT("PrimaryWeapon is not valid"));
		return;
	}
		
	bWantsToAim = false;
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

	// DOREPLIFETIME(ACharacterBase, Health);
	// DOREPLIFETIME(ACharacterBase, Armor);
	DOREPLIFETIME(ACharacterBase, bIsFiring);
	DOREPLIFETIME(ACharacterBase, bIsAiming);
	DOREPLIFETIME(ACharacterBase, PrimaryWeapon);
	DOREPLIFETIME(ACharacterBase, WalkSpeed);
	DOREPLIFETIME(ACharacterBase, bIsDead);
	DOREPLIFETIME(ACharacterBase, bIsPreparingFire);	
	DOREPLIFETIME(ACharacterBase, bWantsToAim);
	DOREPLIFETIME(ACharacterBase, bIsPreparedForBattle);
	DOREPLIFETIME(ACharacterBase, GameplayAbility_FireHandle);
	DOREPLIFETIME(ACharacterBase, GameplayAbility_ADSHandle);
	DOREPLIFETIME(ACharacterBase, GameplayAbility_ReloadHandle);
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

			if (AttributeSetHealth->GetHealth() <= 0.f)
			{
				KillToDeath();
			}
		}
	}
}

void ACharacterBase::ChangeLife(float Value)
{
	// TODO: Use GameplayEffect to modify health and armor

	// if (AttributeSetArmor == nullptr || AttributeSetHealth == nullptr)
	// {
	// 	return;
	// }

	// const float OldArmor = AttributeSetArmor->GetCurrentArmor();
	// const float OldHealth = AttributeSetHealth->GetCurrentHealth();

	// if (GetArmor() > 0)
	// {
	// 	Armor += Value;
	// 	if (Armor < 0)
	// 	{
	// 		Health += Armor;
	// 		Armor = 0;
	// 	}
	// }
	// else 
	// {
	// 	Health += Value;
	// }

	// if (Health < 0) 
	// {
	// 	Health = 0;
	// }
	
	// UE_LOG(LogCharacter, Log, TEXT("OldArmor: %f, CurrArmor: %f, OldHealth: %f, CurrHealth: %f"), 
	// 	OldArmor, Armor, OldHealth, Health);
}

void ACharacterBase::FaceToTarget(const FVector& TargetLocation)
{

}

void ACharacterBase::AIAttack()
{
	SwitchToAiming();

	OnStartFire();
}

void ACharacterBase::AIStopAttack()
{
	RecoveryFromAiming();

	OnStopFire();
}

bool ACharacterBase::IsAI() const
{
	return Cast<AAIControllerBase>(GetController()) != nullptr;
}

void ACharacterBase::LaunchRocket()
{

}

void ACharacterBase::KillToDeath()
{
	bIsDead = true;
	UE_LOG(LogCharacter, Log, TEXT("Character is killed to death"));

	AfterCharacterDeath();
}


void ACharacterBase::AfterCharacterDeath_Implementation()
{
	if (AAIControllerBase* AIC = Cast<AAIControllerBase>(GetController()))
	{
		UBrainComponent* AIBrain = AIC->GetBrainComponent();
		if (AIBrain == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Not found brain component"));
			return;
		}

		AIBrain->StopLogic(TEXT("Character Death"));
	}
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

UAttributeSetHealth* ACharacterBase::GetAttributeSetHealth() const
{
	return AttributeSetHealth;
}

void ACharacterBase::ActivateReloadAbility()
{
	if (GameplayAbility_ReloadHandle.IsValid() && IsValid(AbilitySystem))
	{
		if (AbilitySystem->TryActivateAbility(GameplayAbility_ReloadHandle) == false)
		{
			UE_LOG(LogCharacter, Warning, TEXT("Activate reload ability failed"));
		}
	}
}

void ACharacterBase::ActivateFireAbility()
{
	if (GameplayAbility_FireHandle.IsValid() && IsValid(AbilitySystem))
	{
		if (AbilitySystem->TryActivateAbility(GameplayAbility_FireHandle) == false)
		{
			UE_LOG(LogCharacter, Warning, TEXT("Activate fire ability failed"));
		}
	}
}

void ACharacterBase::CancelFireAbility()
{
	if (GameplayAbility_FireHandle.IsValid() && IsValid(AbilitySystem))
	{
		AbilitySystem->CancelAbilityHandle(GameplayAbility_FireHandle);

		UE_LOG(LogCharacter, Log, TEXT("Cancel fire ability"));
	}
}