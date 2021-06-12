// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include <activation.h>

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"	
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_r"));

	HitBox1 = CreateDefaultSubobject<USceneComponent>(TEXT("HitBox1"));
	HitBox2 = CreateDefaultSubobject<USceneComponent>(TEXT("HitBox2"));
	HitBox3 = CreateDefaultSubobject<USceneComponent>(TEXT("HitBox3"));

	HitBox1->SetupAttachment(Weapon);
	HitBox2->SetupAttachment(Weapon);
	HitBox3->SetupAttachment(Weapon);
}

void APlayerCharacter::HitBoxOn()
{

	HitBox1Location = HitBox1->GetComponentLocation();
	HitBox2Location = HitBox2->GetComponentLocation();
	HitBox3Location = HitBox3->GetComponentLocation();
	HitBoxActive = true;
}

void APlayerCharacter::HitBoxOff()
{
	HitBoxActive = false;
	HitActors.Empty();
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction(("BasicAttack"), IE_Pressed, this, &APlayerCharacter::BasicAttack);
	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &APlayerCharacter::LockOn);
	PlayerInputComponent->BindAction("AltAttack", IE_Pressed, this, &APlayerCharacter::AltAttack);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacter::Dash);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::MoveForward(float Value)
{
	ForwardInput = Value;
	switch (State)
	{
		case Idle: case Walking: case Jumping:
			if ((Controller != nullptr) && (Value != 0.0f))
			{
				// find out which way is forward
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				// get forward vector
				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				AddMovementInput(Direction, Value);
			}
			SetStateFromBasicMovement();
			break;
		default:
			break;
	}

}

void APlayerCharacter::MoveRight(float Value)
{
	RightInput = Value;
	switch (State)
	{
		case Idle: case Walking: case Jumping:
		if ( (Controller != nullptr) && (Value != 0.0f) )
		{
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
	
			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}
		SetStateFromBasicMovement();
		break;
		
		default:
			break;
	}

}

void APlayerCharacter::SetStateFromBasicMovement()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		if (ForwardInput == 0 && RightInput == 0)
			State = Idle;
		else
		{
			State = Walking;
			StopAnimMontage();
			ResetCounter();
		}
	}
	else
		State = Jumping;
	
}

void APlayerCharacter::BasicAttack()
{
	switch (State)
	{
		case Idle: case Walking: 
			Combo();
			bIsAltAttack = false;
			break;
		case Dashing:
			ResetMovement();
			bIsAltAttack = false;
			PlayAnimMontage(DashAttack);
			PlayerAttackType = Medium;
			ComboCounter++;
			State = Attacking;
			break;
		case Attacking:
			AttackInputBuffer = true;
			bIsAltAttack = false;
		default:
			break;
	}
}

void APlayerCharacter::MidAttack()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	if (AttackInputBuffer)
	{
		AttackInputBuffer = false;
		if (bIsAltAttack)
		{
			AltCombo();
		}
		else
			Combo();
	}
	else
		State = Idle;
}

void APlayerCharacter::ChooseComboAttack(int32 Counter)
{
	switch (Counter)
	{
		case 0:
			PlayAnimMontage(BAttack1);
			ComboCounter++;
			PlayerAttackType = Medium;
			break;
		case 1:
			PlayAnimMontage(BAttack2);
			ComboCounter++;
			PlayerAttackType = Medium;
			break;
		case 2:
			PlayAnimMontage(BAttack3);
			ComboCounter = 0;
			PlayerAttackType = Heavy;
			break;
		default:
			break;
			
	}
}

void APlayerCharacter::ChooseAltComboAttack(int32 Counter)
{
	switch (Counter)
	{
	case 0:
		PlayAnimMontage(AltAttack1);
		AltComboCounter++;
		PlayerAttackType = Light;
		break;
	case 1:
		PlayAnimMontage(AltAttack2);
		AltComboCounter++;
		PlayerAttackType = Medium;
		break;
	case 2:
		PlayAnimMontage(AltAttack3);
		AltComboCounter = 0;
		PlayerAttackType = Medium;
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		break;
	default:
		break;
			
	}
}

void APlayerCharacter::Combo()
{
	ChooseComboAttack(ComboCounter);
	State = Attacking;
}

void APlayerCharacter::AltCombo()
{
	ChooseAltComboAttack(AltComboCounter);
	State = Attacking;
}

void APlayerCharacter::ResetAttack()
{
	ResetCounter();
	State = Idle;
}

void APlayerCharacter::ResetCounter()
{
	ComboCounter = 0;
	AltComboCounter = 0;
}

void APlayerCharacter::TraceHits()
{
	HitBoxTraceLocation(HitBox1Location, HitBox1);
	HitBoxTraceLocation(HitBox2Location, HitBox2);
	HitBoxTraceLocation(HitBox3Location, HitBox3);
}

void APlayerCharacter::HitBoxTraceLocation(FVector& HitBoxLocation, USceneComponent* HitBox)
{
	SphereTraces(HitBoxLocation, HitBox->GetComponentLocation(), 50.0f, HitActor);
	HitBoxLocation = HitBox->GetComponentLocation();
}

// Back up hitbox for low FPS
void APlayerCharacter::BackupHitBox()
{
	SphereTraces(GetActorLocation() + GetActorForwardVector() * 50, GetActorLocation() + GetActorForwardVector() * 200, 75.0f, HitActor);
}

void APlayerCharacter::SphereTraces(FVector StartLocation, FVector EndLocation, float SphereSize, AActor* &Actor )
{
	FHitResult Hit;
	//const FName TraceTag("TraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereSize);
	FCollisionQueryParams Params;
	//Params.TraceTag = TraceTag;

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	Params.AddIgnoredActor(this);
	GetWorld()->SweepSingleByObjectType(Hit, StartLocation, EndLocation, FQuat(0,0,0,0), TraceObjects, Sphere, Params);

	if (Hit.GetActor())
	{
		if (!HitActors.Contains(Hit.GetActor()))
		{
			Actor = Hit.GetActor();
			HitActors.Add(Actor);
			ICombatInterface* HitActorInterface = Cast<ICombatInterface>(Actor);
			if (HitActorInterface)
			{
				HitActorInterface->TakeDamage(PlayerAttackType, this);
			}
			HitStun();
		}
	}
}

void APlayerCharacter::TakeDamage(TEnumAsByte<EPAttackType> AttackType, AActor* DamagingActor)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DamagingActor->GetActorLocation());
	SetActorRotation(FRotator(0,LookAtRotation.Yaw, 0));
	
	switch (AttackType)
	{
		case Light:
			PlayAnimMontage(LightHitReaction);
			
			break;
		case Medium:
			PlayAnimMontage(MediumHitReaction);
			break;
		case Heavy:
			{
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				PlayAnimMontage(HeavyHitReaction);
				float KnockdownAnimTime = GetCurrentMontage()->GetPlayLength();
				KnockdownTimer;
				GetWorldTimerManager().SetTimer(KnockdownTimer, this, &APlayerCharacter::KnockdownAnimDelay, KnockdownAnimTime);
				break;	
			}
		default:
			break;
	}
}

void APlayerCharacter::KnockdownAnimDelay()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void APlayerCharacter::LockOn()
{
	FHitResult Hit;
	//const FName TraceTag("TraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(750.0f);
	FCollisionQueryParams Params;
	//Params.TraceTag = TraceTag;

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	Params.AddIgnoredActor(this);
	
	GetWorld()->SweepSingleByObjectType(Hit, GetActorLocation(), GetActorLocation(), FQuat(0,0,0,0), TraceObjects,Sphere, Params);
	if (Hit.GetActor())
	{
		GEngine->AddOnScreenDebugMessage(0, 1, FColor::Yellow, FString::Printf(TEXT("%s"), *Hit.GetActor()->GetName()));
		LockedOnCharacter = Hit.GetActor();
		bIsLockedOn = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	

}

void APlayerCharacter::HitStun()
{
	if (PlayerAttackType == Heavy)
	{
		CustomTimeDilation = 0.05;
		LockedOnCharacter->CustomTimeDilation = 0.05;
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &APlayerCharacter::HitStunEnd, 0.3f);
	}
	else
	{
		CustomTimeDilation = 0.3;
		LockedOnCharacter->CustomTimeDilation = 0.3;
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &APlayerCharacter::HitStunEnd, 0.125);
	}
}

void APlayerCharacter::HitStunEnd()
{
	CustomTimeDilation = 1;
	LockedOnCharacter->CustomTimeDilation = 1;
}

void APlayerCharacter::AltAttack()
{
	switch (State)
	{
	case Idle: case Walking:
		AltCombo();
		bIsAltAttack = true;
		break;
	case Attacking:
		AttackInputBuffer = true;
		bIsAltAttack = true;
	default:
		break;
	}
}

void APlayerCharacter::SetupDash()
{
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	GetCharacterMovement()->GroundFriction = 0.0f;
	HitBoxOff();
	ResetCounter();
}

void APlayerCharacter::Dash()
{
	switch (State)
	{
		case Idle: case Walking: case Attacking:
			State = Dashing;
			SetupDash();
			if (ForwardInput == 0 && RightInput == 0)
			{
				PlayAnimMontage(DashBack);
				FVector Impulse = (GetActorForwardVector() * -1.0f) * 1000.0f;
				GetCharacterMovement()->AddImpulse(Impulse, true);
			}
			else
			{
				if (RightInput == 0)
				{
					PlayAnimMontage(ForwardInput >= 1 ?	DashForward : DashBack);
					FVector Impulse = (GetActorForwardVector() * ForwardInput) * 1000.0f;
					GetCharacterMovement()->AddImpulse(Impulse, true);
				}
				else
				{
					PlayAnimMontage(RightInput >= 1 ? DashRight : DashLeft);
					FVector InputDashDirection = (GetActorForwardVector() * ForwardInput) + (GetActorRightVector() * RightInput);
					UKismetMathLibrary::Vector_Normalize(InputDashDirection);

					FVector Impulse = InputDashDirection * 1000.0f;
					GetCharacterMovement()->AddImpulse(Impulse, true);
				}
			}
			break;
		default:
			break;
	}
	
}

void APlayerCharacter::ResetMovement()
{
	State = Idle;
	GetCharacterMovement()->GroundFriction = 8.0f;
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, FString::Printf(TEXT("%i"), State));

	if (HitBoxActive)
	{
		TraceHits();
	}

	if (bIsLockedOn)
	{
		
		FRotator PlayerLookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedOnCharacter->GetActorLocation());
		GetController()->SetControlRotation(PlayerLookAtRotation);
		FRotator CamLookAtRotation = UKismetMathLibrary::FindLookAtRotation(FollowCamera->GetComponentLocation(),LockedOnCharacter->GetActorLocation());
		
		FollowCamera->SetWorldRotation(FMath::RInterpTo(FollowCamera->GetComponentRotation(), CamLookAtRotation, DeltaSeconds, 7.0f));
	}
	
}

