// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponModelData.h"

#include "WeaponBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWeapon, Log, All);


UCLASS(Blueprintable)
class UEGAMETRAININGROOM_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	FORCEINLINE const FWeaponModelData& GetWeaponModelData() const { return WeaponData; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FireProcess();

	bool CanFire();

	virtual void PostInitializeComponents() override;

	virtual bool WeaponLineTrace_Single(FHitResult& HitInfo);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitializeWeaponData();

	virtual void ConsumeAmmo();

	virtual bool NeedReload();

	virtual void Reload();

	virtual bool CanReload();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Fire();

	void AttachTo(class ACharacterBase* Pawn);

	class ACharacterBase* GetOwnerCharacter() const;

protected:		// RPC
	// [Server + Local]
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerFire();
	bool ServerFire_Validate();
	void ServerFire_Implementation();

	UFUNCTION(Client, Reliable)
	virtual void ClientFireEffect(bool IsHit, const FHitResult& HitInfo);
	void ClientFireEffect_Implementation(bool IsHit, const FHitResult& HitInfo);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponData)
	FWeaponModelData WeaponData;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = Owner)
	class ACharacterBase* OwnerCharacter;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = WeaponData)
	int32 CurrentAmmo;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = WeaponData)
	int32 TotalLeftAmmo;
};
