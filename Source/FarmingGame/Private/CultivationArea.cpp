// Fill out your copyright notice in the Description page of Project Settings.


#include "CultivationArea.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FarmingGame/FarmingGameCharacter.h"

// Sets default values
ACultivationArea::ACultivationArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PlantedCrop = nullptr;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent); 

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACultivationArea::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ACultivationArea::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ACultivationArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACultivationArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACultivationArea::PlantCrop(AActor* Crop)
{
	if (!Crop)  // ✅ Prevent crashes if Crop is null
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Crop is NULL! Cannot plant."));
		return;
	}

	if (!PlantedCrop && IsPlayerInside())
	{
		PlantedCrop = Crop;
		UE_LOG(LogTemp, Warning, TEXT("🌱 Crop planted in Cultivation Area!"));
	}
	else if (!IsPlayerInside())
	{
		UE_LOG(LogTemp, Error, TEXT("❌ You must be inside a Cultivation Area to plant crops!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ This Cultivation Area already has a crop!"));
	}
}


void ACultivationArea::ClearCrop()
{
	UE_LOG(LogTemp, Warning, TEXT("🛑 Cultivation Area cleared! Ready for new crop."));
	PlantedCrop = nullptr;
}

void ACultivationArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	AFarmingGameCharacter* Player = Cast<AFarmingGameCharacter>(OtherActor);
	if (Player)
	{
		Player->CultivationArea = this;  // ✅ Now Player always knows which area they are in.
		UE_LOG(LogTemp, Warning, TEXT("✅ Player entered cultivation area and assigned CultivationArea!"));
	}
}

void ACultivationArea::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("❌ Player left cultivation area!"));
	
}

bool ACultivationArea::IsPlayerInside()
{
	TArray<AActor*> OverlappingActors;
	CollisionBox->GetOverlappingActors(OverlappingActors, AFarmingGameCharacter::StaticClass());

	return OverlappingActors.Num() > 0;
}

