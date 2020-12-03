// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"


#include "CharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);

UCLASS()
class UEGAMETRAININGROOM_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

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

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

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

	UFUNCTION()
	void OnRep_PrimaryWeapon(class AWeaponBase* LastWeapon);

	virtual void InterplateMoveSpeed();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PrimaryWeapon, Category = Weapons)
	class AWeaponBase* PrimaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Weapons)
	bool bIsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Weapons)
	bool bIsFiring;

	// 初始武器类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapons)
	TSubclassOf<AWeaponBase> InitialWeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = Attributes)
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Attributes)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = Attributes)
	float Armor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Attributes)
	float MaxArmor;

	// 是否准备射击
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Weapons)
	bool bIsPreparingFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CharacterAnim)
	class UAnimMontage* AimMontage;

	// 移动速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = Attributes)
	float WalkSpeed;

private:
	FTimerHandle TimerHandle_Fire;
};
