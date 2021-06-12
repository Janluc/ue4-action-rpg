// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CombatInterface.h"
#include "PState.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class MYPROJECT_API APlayerCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HitBox1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HitBox2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HitBox3;

	FVector HitBox1Location;
	FVector HitBox2Location;
	FVector HitBox3Location;

	bool HitBoxActive;
	
	public:
	APlayerCharacter();

	void HitBoxOn();
	void HitBoxOff();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EPState> State;
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	float ForwardInput;
	float RightInput;

	int32 ComboCounter = 0;
	int32 AltComboCounter = 0;
	bool AttackInputBuffer;

	void SetStateFromBasicMovement();

	void BasicAttack();

	void MidAttack();

	void ChooseComboAttack(int32 Counter);
	void ChooseAltComboAttack(int32 Counter);

	void Combo();
	void AltCombo();

	void ResetAttack();
	void ResetCounter();

	void TraceHits();
	void HitBoxTraceLocation(FVector &HitBoxLocation, USceneComponent* HitBox);
	void BackupHitBox();

	void SphereTraces(FVector StartLocation, FVector EndLocation, float SphereSize, AActor* &HitActor );

	virtual void TakeDamage(TEnumAsByte<EPAttackType> AttackType, AActor* DamagingActor) override;

	void KnockdownAnimDelay();

	void LockOn();

	void HitStun();
	void HitStunEnd();
	void AltAttack();
	
	AActor* LockedOnCharacter;
	
	FTimerHandle KnockdownTimer;
	bool bIsLockedOn;
	bool bIsAltAttack;

	TEnumAsByte<EPAttackType> PlayerAttackType;
	
	AActor* HitActor;
	TArray<AActor*> HitActors;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* BAttack1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* BAttack2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* BAttack3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* AltAttack1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* AltAttack2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* AltAttack3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* LightHitReaction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* MediumHitReaction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* HeavyHitReaction;
	
	protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);



	

	virtual void Tick(float DeltaSeconds) override;
	/** 
	* Called via input to turn at a given rate. 
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate. 
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);


	protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
