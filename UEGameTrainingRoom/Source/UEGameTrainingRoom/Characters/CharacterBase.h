// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "HurtableInterface.h"
#include "LifeInterface.h"
#include "GameplayAbilitySpec.h"


#include "CharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterHealthChanged, float, NewHealth, float, MaxHealth);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterArmorChanged, float, NewArmor, float, MaxArmor);

enum class EAbilityInputID: uint8
{
	None,
	Jump
};

UCLASS()
class UEGAMETRAININGROOM_API ACharacterBase : 
	public ACharacter, 
	public IHurtable, public ILife, 
	public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|UI")
	TSubclassOf<class UPlayerHeadUpUI> HeadUpUIClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character|Attributes")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Abilities")
	TSubclassOf<class UGameplayAbility> FireAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Abilities")
	TSubclassOf<class UGameplayAbility> ADSAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Abilities")
	TSubclassOf<class UGameplayAbility> ReloadAbility;

	// 初始武器类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Weapons")
	TSubclassOf<AWeaponBase> InitialWeaponClass;

public:
	// Sets default values for this character's properties
	ACharacterBase();

	virtual void PossessedBy(class AController* InController) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void InitializeWeapon();

	virtual void InitializeAttributes();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Fire();

	// [Interface Implementation]
	UFUNCTION(BlueprintCallable)
	virtual void TakeHurt(const class AWeaponBase* SourceWeapon, float Distance) override;

	// [Interface Implementation]
	UFUNCTION(BlueprintCallable)
	virtual void ChangeLife(float Value) override;

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE class AWeaponBase* GetCurrentWeapon() const { return PrimaryWeapon; }
	// FORCEINLINE FCharacterHealthChanged& GetHealthChangedDelegator() { return OnHealthChangedDelegator; }
	// FORCEINLINE FCharacterArmorChanged& GetArmorChangedDelegator() { return OnArmorChangedDelegator; }
	FORCEINLINE class UAnimationAsset* GetHitReactAnim() const { return HitReactAnim; }

	// TODO
	UFUNCTION(BlueprintCallable)
	void FaceToTarget(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable)
	void AIAttack();

	UFUNCTION(BlueprintCallable)
	void AIStopAttack();

	UFUNCTION(BlueprintCallable)
	bool IsAI() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsAlive() const { return !bIsDead; }

	UFUNCTION(BlueprintCallable)
	void LaunchRocket();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual class UAttributeSetHealth* GetAttributeSetHealth() const;

	// virtual class UAttributeSetArmor* GetAttributeSetArmor() const;

	FORCEINLINE class UPlayerHeadUpUI* GetHeadUpUIInstance() const
	{
		return HeadUpUIInstance;
	}

	virtual void OnStartFire();
	virtual void OnStopFire();
	virtual bool CanFire() const;

	void ActivateFireAbility();

	void CancelFireAbility();
		
	virtual void KillToDeath();

	void ActivateADSAbility();

	void DeactivateADSAbility();

	void ActivateReloadAbility();

	FORCEINLINE bool GetIsPreparedForBattle() const { return bIsPreparedForBattle; }

	virtual void SwitchToAiming();
	virtual void RecoveryFromAiming();

	virtual bool CanReload() const;

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	UFUNCTION(BlueprintNativeEvent)
	void AfterCharacterDeath();

	virtual void AfterCharacterDeath_Implementation();

	UFUNCTION()
	void OnRep_PrimaryWeapon(class AWeaponBase* LastWeapon);

	virtual void InterplateMoveSpeed();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// UFUNCTION()
	// void OnRep_HealthChanged(float OldHealth);

	// UFUNCTION()
	// void OnRep_ArmorChanged(float OldArmor);

	void SetPreparedForBattle();

	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnClientAttributeDataPreparedEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void OnServerAttributeDataPreparedEvent();

private:
	void InitializeASCFromPlayerState();

	// 初始化头顶UI
	void InitializeHeadUpUI();

	void BindAbilityInput();

	void InitializeDefaultAbilities();

protected: // RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerModifyMoveSpeed(float NewSpeed);
	bool ServerModifyMoveSpeed_Validate(float NewSpeed);
	void ServerModifyMoveSpeed_Implementation(float NewSpeed);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInterplateMoveSpeed();
	bool ServerInterplateMoveSpeed_Validate();
	void ServerInterplateMoveSpeed_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwitchToAiming();
	bool ServerSwitchToAiming_Validate();
	void ServerSwitchToAiming_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRecoveryFromAiming();
	bool ServerRecoveryFromAiming_Validate();
	void ServerRecoveryFromAiming_Implementation();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

protected:
	// AllowPrivateAcces=true：可以在蓝图中使用C++的私有成员 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// 主武器实例
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_PrimaryWeapon, Category = Weapons)
	class AWeaponBase* PrimaryWeapon;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = Weapons)
	bool bIsAiming;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = Weapons)
	bool bIsFiring;

	// 是否准备射击
	UPROPERTY(BlueprintReadWrite, Replicated, Category = Weapons)
	bool bIsPreparingFire;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = Attributes)
	uint8 bIsDead : 1;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = Attributes)
	uint8 bWantsToAim : 1;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = Attributes)
	uint8 bIsPreparedForBattle : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CharacterAnim)
	class UAnimMontage* AimMontage;

	// 移动速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = Attributes)
	float WalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	class UWidgetComponent* HeadUpWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	class UAbilityComponent* AbilityComp;

	UPROPERTY()
	class UPlayerHeadUpUI* HeadUpUIInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animations)
	class UAnimationAsset* HitReactAnim;

	UPROPERTY(Transient, BlueprintReadWrite)
	class UAbilitySystemComponent* AbilitySystem;

	UPROPERTY(Transient, BlueprintReadWrite)
	class UAttributeSetHealth* AttributeSetHealth;

private:
	FTimerHandle TimerHandle_Fire;
	FTimerHandle TimerHandle_SetPrepareForBattle;
	bool bAbilityInputBound = false;
	bool bAbilityGranted = false;

protected:
	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayAbilitySpecHandle GameplayAbility_FireHandle;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayAbilitySpecHandle GameplayAbility_ADSHandle;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayAbilitySpecHandle GameplayAbility_ReloadHandle;
};
