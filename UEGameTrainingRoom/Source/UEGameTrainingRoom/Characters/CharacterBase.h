// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "HurtableInterface.h"
#include "LifeInterface.h"


#include "CharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterHealthChanged, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterArmorChanged, float, NewArmor, float, MaxArmor);

UCLASS()
class UEGAMETRAININGROOM_API ACharacterBase : public ACharacter, public IHurtable, public ILife
{
	GENERATED_BODY()

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
	FORCEINLINE FCharacterHealthChanged& GetHealthChangedDelegator() { return OnHealthChangedDelegator; }
	FORCEINLINE FCharacterArmorChanged& GetArmorChangedDelegator() { return OnArmorChangedDelegator; }
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

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	virtual bool CanFire();
	virtual bool CanReload();

	virtual void SwitchToAiming();
	virtual void RecoveryFromAiming();

	virtual void OnStartFire();
	virtual void OnStopFire();

	virtual void KillToDeath();

	UFUNCTION(BlueprintNativeEvent)
	void AfterCharacterDeath();
	virtual void AfterCharacterDeath_Implementation();

	UFUNCTION()
	void OnRep_PrimaryWeapon(class AWeaponBase* LastWeapon);

	virtual void InterplateMoveSpeed();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_HealthChanged(float OldHealth);

	UFUNCTION()
	void OnRep_ArmorChanged(float OldArmor);

	void SetPreparedForBattle();

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

	UFUNCTION(Client, Unreliable)
	void ClientHideHeadUpUI();
	void ClientHideHeadUpUI_Implementation();

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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PrimaryWeapon, Category = Weapons)
	class AWeaponBase* PrimaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Weapons)
	bool bIsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Weapons)
	bool bIsFiring;

	// 初始武器类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapons)
	TSubclassOf<AWeaponBase> InitialWeaponClass;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_HealthChanged, Category = Attributes)
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Attributes)
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_ArmorChanged, Category = Attributes)
	float Armor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Attributes)
	float MaxArmor;

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

	UPROPERTY(BlueprintAssignable, Category = EventDispachers)
	FCharacterHealthChanged OnHealthChangedDelegator;

	UPROPERTY(BlueprintAssignable, Category = EventDispachers)
	FCharacterArmorChanged OnArmorChangedDelegator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	class UWidgetComponent* HeadUpWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	class UAbilityComponent* AbilityComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UISettings)
	TSubclassOf<class UUserWidget> HeadUpUIClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animations)
	class UAnimationAsset* HitReactAnim;

private:
	FTimerHandle TimerHandle_Fire;
	FTimerHandle TimerHandle_SetPrepareForBattle;
};
