// Fill out your copyright notice in the Description page of Project Settings.

#include "MicroWave.h"
#include "WallDenotation.h"
#include "FunctionLibrary/MWHelper.h"

#include "ObjectEditorUtils.h"

// Sets default values
AWallDenotation::AWallDenotation(const FObjectInitializer& OI)
{
	USceneComponent* SceneComponent = OI.CreateDefaultSubobject<USceneComponent>(this, TEXT("PositionComponent"));
	RootComponent = SceneComponent;

#if WITH_EDITOR
	SpriteComponent = OI.CreateEditorOnlyDefaultSubobject<UBillboardComponent>(this, TEXT("Sprite"));
	if (!IsRunningCommandlet() && (SpriteComponent != NULL))
	{
		static ConstructorHelpers::FObjectFinderOptional<UTexture2D> SpriteTexture(TEXT("/Engine/EditorResources/S_Route"));

		SpriteComponent->Sprite = SpriteTexture.Get();
		SpriteComponent->RelativeScale3D = FVector(2.f, 2.f, 2.f);
		SpriteComponent->bHiddenInGame = true;
		SpriteComponent->bVisible = true;

		SpriteComponent->AttachParent = RootComponent;
		SpriteComponent->SetAbsolute(false, false, true);
		SpriteComponent->bIsScreenSizeScaled = true;
	}
#endif

	// Add two corners
	Corners.Add(FVector(0.f, -100.f, 20.f));
	Corners.Add(FVector(0.f, 100.f, 20.f));
}

//void AWallDenotation::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}

#if WITH_EDITOR
void AWallDenotation::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (nullptr != PropertyChangedEvent.Property) // ! this will never happen, since there is a bug: Property is always null
	{
		const FName CategoryName = FObjectEditorUtils::GetCategoryFName(PropertyChangedEvent.Property);
		if (CategoryName == TEXT("Corners"))
		{
			OnCornerMoved();
		}
	}
}
#endif

FVector AWallDenotation::GetCornerWorldLocation(int32 CornerIndex) const
{
	if (CornerIndex < 0 || CornerIndex > this->Corners.Num() - 1)
	{
		UE_LOG(LogTemp, Error, TEXT("CornerIndex out of bounds"));
		return FVector::ZeroVector;
	}

	// Convert local corner coords to world coords
	FVector WorldLocation = GetTransform().GetRotation() * Corners[CornerIndex];
	WorldLocation += GetTransform().GetTranslation();

	return WorldLocation;
}

void AWallDenotation::OnCornerMoved()
{
	UE_LOG(LogTemp, Log, TEXT("A corner has been moved"));
}
