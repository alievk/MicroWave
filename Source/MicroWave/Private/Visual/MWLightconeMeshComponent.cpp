// K.A. Aliev

#include "MicroWave.h"
#include "Visual/MWLightconeMeshComponent.h"

UMWLightconeMeshComponent::UMWLightconeMeshComponent(const FObjectInitializer& OI)
	: Super(OI)
{
	// Default values
	InnerRadius = 10;
	OuterRadius = 200; // must be always greater that InnerRadius
	Distance = 1000;
	MeshUSegments = 10;
	MeshVSegments = 32;
	UTiling = VTiling = 1.f;

	// No collision
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	// Can tick
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bIsActive = true;

	// No shadow
	CastShadow = false;
	bCastDynamicShadow = false;
	bCastStaticShadow = false;
}

void UMWLightconeMeshComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateMesh();
}

void UMWLightconeMeshComponent::PostInitProperties()
{
	Super::PostInitProperties();

}

float UMWLightconeMeshComponent::GetOpeningAngle() const
{
	return Distance != 0.f ? FMath::Atan((OuterRadius - InnerRadius) / Distance) : MAX_FLT;
}

FVector UMWLightconeMeshComponent::GetFocalPoint() const
{
	FVector sightOrt = GetComponentRotation().Vector();
	FVector coneOrig = GetComponentLocation();

	float focus = 1e6; // in the worst case
	if (!FMath::IsNearlyEqual(OuterRadius, InnerRadius))
	{
		focus = InnerRadius * Distance / (OuterRadius - InnerRadius);
	}

	return coneOrig - sightOrt * focus;
}

bool UMWLightconeMeshComponent::PointInViewangle(const FVector& Point) const
{
	// Calculate dot product of two vector and check the angle between them
	
	FVector toPoint = (Point - GetFocalPoint()).GetSafeNormal();
	FVector sightOrt = GetComponentRotation().Vector();
	float dotProd = toPoint | sightOrt;
	float cosA = FMath::Cos(GetOpeningAngle());

	return dotProd > cosA;
}

bool UMWLightconeMeshComponent::LightconeHitsActor(const AActor* Actor) const
{
	// Loop over the actors hit by the cone and compare with Actor

	if (Actor)
	{
		for (AActor* hitActor : HitActors)
		{
			if (Actor == hitActor)
			{
				return true;
			}
		}
	}

	return false;
}

bool UMWLightconeMeshComponent::LightconeHitsComponent(const UPrimitiveComponent* Component) const
{
	// Loop over the components hit by the cone and compare with Actor

	if (Component)
	{
		for (UPrimitiveComponent* hitComponent : HitComponents)
		{
			if (Component == hitComponent)
			{
				return true;
			}
		}
	}

	return false;
}

void UMWLightconeMeshComponent::SetAlarm(bool NewState)
{
	if (NewState != bIsAlarm)
	{
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(GetMaterial(0));
		if (MID)
		{
			// Let the material decide what to do when the alarm state changes
			MID->SetScalarParameterValue(TEXT("IsAlarm"), float(NewState));
		}

		bIsAlarm = NewState;
	}
}

void UMWLightconeMeshComponent::AdjustLight(USpotLightComponent* Light)
{
	check(Light)

	const float shrinkInnerAngle = 0.7f;
	Light->SetOuterConeAngle( FMath::RadiansToDegrees(GetOpeningAngle()) );
	Light->SetInnerConeAngle( Light->OuterConeAngle * shrinkInnerAngle );

	// copy lightcone's color
	UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(GetMaterial(0));
	if (MID)
	{
		FLinearColor lightColor;
		MID->GetVectorParameterValue(TEXT("NormalColor"), lightColor);

		// set maximum brigtness of color
		FLinearColor hsv = lightColor.LinearRGBToHSV();
		hsv.B = 1.f;

		lightColor = hsv.HSVToLinearRGB();
		Light->SetLightColor(lightColor);
	}

	Light->SetAttenuationRadius(Distance);
}

void UMWLightconeMeshComponent::UpdateMesh()
{
	// The idea is to cast cone edges to the world and cut those edges of the cone where
	// they hit the world. The resulting mesh should look like the outline of a spot light.
	// !! Change !! Mesh edges are not cut anymore, instead Vector Color Alpha componet used to
	// mask vertices of the intersecting edge.

	if (!GetOwner())
	{
		return;
	}

	if (OuterRadius < InnerRadius)
	{
		Swap(OuterRadius, InnerRadius);
	}

	if (Distance < 0.f)
	{
		Distance = -Distance;
	}

	if (MeshVSegments < 3)
	{
		MeshVSegments = 3;
	}

	TArray<FVector> EdgeStart, EdgeEnd;
	const float edgeLength = FMath::Sqrt(FMath::Square(OuterRadius - InnerRadius) + Distance*Distance);

	// Build non-transformed full distance cone edges.
	float ang = 0;
	float angStep = FMath::DegreesToRadians(360.f / MeshVSegments); // angle step

	for (uint32 i = 0; i < (uint32)MeshVSegments; ++i)
	{
		float uY = InnerRadius*cos(ang);
		float uZ = InnerRadius*sin(ang);
		float bY = OuterRadius*cos(ang);
		float bZ = OuterRadius*sin(ang);

		EdgeStart.Add(FVector(0, uY, uZ));
		EdgeEnd.Add(FVector(Distance, bY, bZ));

		ang += angStep;
	}

	HitActors.Empty();
	HitComponents.Empty();

	// distance between the edge origin and the point where it hits another mesh.
	TArray<float> newEdgeLength;
	
	for (uint32 i = 0; i < (uint32)MeshVSegments; ++i)
	{
		// Transform the cone edges to correspond the "light source"'s transform.
		FVector wEdgeStart, wEdgeEnd; // edges in world space
		FTransform toWorld = GetComponentTransform();
		wEdgeStart = toWorld.GetRotation()*EdgeStart[i];
		wEdgeStart += toWorld.GetTranslation();
		wEdgeEnd = toWorld.GetRotation()*EdgeEnd[i];
		wEdgeEnd += toWorld.GetTranslation();

		// Cast the transformed cone edges to the world
		FHitResult hit;
		FCollisionQueryParams params;
		if (this->GetOwner())
		{
			params.AddIgnoredActor(this->GetOwner()); // ignore the owner actor
		}

		GetWorld()->LineTraceSingle(hit, wEdgeStart, wEdgeEnd, ECollisionChannel::ECC_Visibility, params);

		// Cut the local-space edges where they would hit the world.
		if (hit.bBlockingHit)
		{
			newEdgeLength.Add( (hit.ImpactPoint - wEdgeStart).Size() );

			HitActors.Add(hit.GetActor());
			HitComponents.Add(hit.GetComponent());
		}
		else
		{
			newEdgeLength.Add(edgeLength);
		}
	}
	
	// Build triangular mesh of the resulting cone edges.
	TArray<FProceduralMeshTriangle> mesh;
	FProceduralMeshTriangle tri;

	for (uint32 i = 0; i < (uint32)MeshVSegments; ++i)
	{
		//
		// V21_ _ V22
		//   |\ 2 |
		//   | \  |
		//   |1 \ |
		//   |_ _\|
		// V11    V12
		//
		// Tri1: V11, V21, V12
		// Tri2: V12, V21, V22
		//

		uint32 ii = i + 1; // next edge index
		if (MeshVSegments - 1 == i) // if this is the last edge, the next is the first edge.
		{
			ii = 0;
		}

		for (uint32 k = 0; k < (uint32)MeshUSegments; ++k) // U-segments
		{
			const float meshUStep = edgeLength / MeshUSegments;

			const FVector iOrt = (EdgeEnd[i] - EdgeStart[i]).GetSafeNormal();
			const FVector iiOrt = (EdgeEnd[ii] - EdgeStart[ii]).GetSafeNormal();

			const FVector v11 = EdgeStart[i] + iOrt * k * meshUStep;
			const FVector v21 = v11 + iOrt * meshUStep;
			const FVector v12 = EdgeStart[ii] + iiOrt * k * meshUStep;
			const FVector v22 = v12 + iiOrt * meshUStep;

			const float stepU = UTiling / MeshUSegments;
			const float stepV = VTiling / MeshVSegments;
			const float tU1 = k * stepU;
			const float tU2 = (k + 1) * stepU;
			const float tV1 = i * stepV; // V-coord of i-th edge
			const float tV2 = (i + 1) * stepV; // V-coord of i+1-th edge

			const FColor vDefCol(0, 0, 0);
			const uint8 maskedAlpha = 0;
			const uint8 unmaskedAlpha = 255;
			const uint8 v11Alpha = newEdgeLength[i] > (v11 - EdgeStart[i]).Size() ? unmaskedAlpha : maskedAlpha;
			const uint8 v21Alpha = newEdgeLength[i] > (v21 - EdgeStart[i]).Size() ? unmaskedAlpha : maskedAlpha;
			const uint8 v12Alpha = newEdgeLength[ii] > (v12 - EdgeStart[ii]).Size() ? unmaskedAlpha : maskedAlpha;
			const uint8 v22Alpha = newEdgeLength[ii] > (v22 - EdgeStart[ii]).Size() ? unmaskedAlpha : maskedAlpha;
			const FColor v11Col = FColor(vDefCol.R, vDefCol.G, vDefCol.B, v11Alpha);
			const FColor v21Col = FColor(vDefCol.R, vDefCol.G, vDefCol.B, v21Alpha);
			const FColor v12Col = FColor(vDefCol.R, vDefCol.G, vDefCol.B, v12Alpha);
			const FColor v22Col = FColor(vDefCol.R, vDefCol.G, vDefCol.B, v22Alpha);

			// Tri 1
			tri.Vertex0.Position = v11;
			tri.Vertex0.U = tU1;
			tri.Vertex0.V = tV1;
			tri.Vertex0.Color = v11Col;

			tri.Vertex1.Position = v21;
			tri.Vertex1.U = tU2;
			tri.Vertex1.V = tV1;
			tri.Vertex1.Color = v21Col;

			tri.Vertex2.Position = v12;
			tri.Vertex2.U = tU1;
			tri.Vertex2.V = tV2;
			tri.Vertex2.Color = v12Col;

			mesh.Add(tri);

			// Tri 2
			tri.Vertex0.Position = v12;
			tri.Vertex0.U = tU1;
			tri.Vertex0.V = tV2;
			tri.Vertex0.Color = v12Col;

			tri.Vertex1.Position = v21;
			tri.Vertex1.U = tU2;
			tri.Vertex1.V = tV1;
			tri.Vertex1.Color = v21Col;

			tri.Vertex2.Position = v22;
			tri.Vertex2.U = tU2;
			tri.Vertex2.V = tV2;
			tri.Vertex2.Color = v22Col;

			mesh.Add(tri);
		}
	}

	UProceduralMeshComponent::SetMesh(mesh);
}

// ///////////////////////////////////////////////////////////
// Backup

// Points where the edges impacts other meshes.
//TArray<FVector> EdgeImpactPoint = EdgeEnd; // By default, no impact.
//float shortestX = Distance;

//DrawDebugLine(GetWorld(), wEdgeStart, hit.ImpactPoint, FColor::Red);
//const float newLength = (hit.ImpactPoint - wEdgeStart).Size();
//EdgeEnd[i] = EdgeStart[i] + (EdgeEnd[i] - EdgeStart[i]).GetSafeNormal() * newLength;
//shortestX = FMath::Min(shortestX, EdgeEnd[i].X);
//EdgeImpactPoint[i] = hit.ImpactPoint;
