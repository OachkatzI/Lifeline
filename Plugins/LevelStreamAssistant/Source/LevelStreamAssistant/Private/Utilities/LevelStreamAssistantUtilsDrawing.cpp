// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

// I copied some code from EditorGeometry to build mesh from ABrush and render highlights for ABrush instead of rendering a simple Box.

#include "Utilities/LevelStreamAssistantUtilsDrawing.h"
#include <Utilities/LevelStreamAssistantUtilsRendering.h>
#include "Engine/Brush.h"
#include "EditorModes.h"
#include "Engine/Polys.h"

namespace LevelStreamAssistantUtils
{
	void DrawBox2(const FMatrix& BoxToWorld, const FVector& Radii, const FLinearColor& Color, const FSceneView& View, FPrimitiveDrawInterface& PDI)
	{
		if (!ensure(GEngine && GEngine->GeomMaterial)) return;

		// Allocate the material proxy and register it so it can be deleted properly once the rendering is done with it.
		FDynamicColoredMaterialRenderProxy* ColorInstance = new FDynamicColoredMaterialRenderProxy(GEngine->GeomMaterial->GetRenderProxy(), Color);
		PDI.RegisterDynamicResource(ColorInstance);

		// Calculate verts for a face pointing down Z
		FVector Positions[4] =
		{
			FVector(-1, -1, +1),
			FVector(-1, +1, +1),
			FVector(+1, +1, +1),
			FVector(+1, -1, +1)
		};
		FVector2D UVs[4] =
		{
			FVector2D(0,0),
			FVector2D(0,1),
			FVector2D(1,1),
			FVector2D(1,0),
		};

		// Then rotate this face 6 times
		FRotator FaceRotations[6];
		FaceRotations[0] = FRotator(0, 0, 0);
		FaceRotations[1] = FRotator(90.f, 0, 0);
		FaceRotations[2] = FRotator(-90.f, 0, 0);
		FaceRotations[3] = FRotator(0, 0, 90.f);
		FaceRotations[4] = FRotator(0, 0, -90.f);
		FaceRotations[5] = FRotator(180.f, 0, 0);

		FDynamicMeshBuilder2 MeshBuilder(View.GetFeatureLevel());

		for (int32 f = 0; f < 6; f++)
		{
			FMatrix FaceTransform = FRotationMatrix(FaceRotations[f]);

			int32 VertexIndices[4];
			for (int32 VertexIndex = 0; VertexIndex < 4; VertexIndex++)
			{
				VertexIndices[VertexIndex] = MeshBuilder.AddVertex(
					FaceTransform.TransformPosition(Positions[VertexIndex]),
					UVs[VertexIndex],
					FaceTransform.TransformVector(FVector(1, 0, 0)),
					FaceTransform.TransformVector(FVector(0, 1, 0)),
					FaceTransform.TransformVector(FVector(0, 0, 1)),
					FColor::White
				);
			}

			MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[1], VertexIndices[2]);
			MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[2], VertexIndices[3]);
		}

		MeshBuilder.Draw(&PDI, FScaleMatrix(Radii) * BoxToWorld, ColorInstance, SDPG_World, false, /*bReceivesDecals*/false);
	}

	void RenderPoly(const ABrush& BrushActor, const FLinearColor& Color, const FSceneView& View, FPrimitiveDrawInterface& PDI)
	{
		if (!ensure(GEngine && GEngine->GeomMaterial)) return;

		TSharedPtr<FGeomObject> GeomObject = MakeShareable(new FGeomObject());
		GeomObject->ParentObject = GeomObject;
		GeomObject->ActualBrush = &BrushActor;
		GeomObject->GetFromSource();

		// Allocate the material proxy and register it so it can be deleted properly once the rendering is done with it.
		FDynamicColoredMaterialRenderProxy* ColorInstance = new FDynamicColoredMaterialRenderProxy(GEngine->GeomMaterial->GetRenderProxy(), Color);
		PDI.RegisterDynamicResource(ColorInstance);

		// Render selected filled polygons.
		for (int32 PolyIdx = 0; PolyIdx < GeomObject->PolyPool.Num(); ++PolyIdx)
		{
			const FGeomPoly* GeomPoly = &GeomObject->PolyPool[PolyIdx];

			FDynamicMeshBuilder2 MeshBuilder(View.GetFeatureLevel());

			TArray<FVector> Verts;

			// Look at the edge list and create a list of vertices to render from.

			FVector LastPos(0);

			for (int32 EdgeIdx = 0; EdgeIdx < GeomPoly->EdgeIndices.Num(); ++EdgeIdx)
			{
				const FGeomEdge* GeomEdge = &GeomPoly->ParentObject->EdgePool[GeomPoly->EdgeIndices[EdgeIdx]];

				if (EdgeIdx == 0)
				{
					Verts.Add(GeomPoly->ParentObject->VertexPool[GeomEdge->VertexIndices[0]]);
					LastPos = GeomPoly->ParentObject->VertexPool[GeomEdge->VertexIndices[0]];
				}
				else if (GeomPoly->ParentObject->VertexPool[GeomEdge->VertexIndices[0]].Equals(LastPos))
				{
					Verts.Add(GeomPoly->ParentObject->VertexPool[GeomEdge->VertexIndices[1]]);
					LastPos = GeomPoly->ParentObject->VertexPool[GeomEdge->VertexIndices[1]];
				}
				else
				{
					Verts.Add(GeomPoly->ParentObject->VertexPool[GeomEdge->VertexIndices[0]]);
					LastPos = GeomPoly->ParentObject->VertexPool[GeomEdge->VertexIndices[0]];
				}
			}

			// Draw Polygon Triangles
			const int32 VertexOffset = MeshBuilder.AddVertex(Verts[0], FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1), FColor::White);
			MeshBuilder.AddVertex(Verts[1], FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1), FColor::White);

			for (int32 VertIdx = 2; VertIdx < Verts.Num(); ++VertIdx)
			{
				MeshBuilder.AddVertex(Verts[VertIdx], FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1), FColor::White);
				MeshBuilder.AddTriangle(VertexOffset + VertIdx - 1, VertexOffset, VertexOffset + VertIdx);
			}

			MeshBuilder.Draw(&PDI, GeomObject->ActualBrush->ActorToWorld().ToMatrixWithScale(), ColorInstance, SDPG_World, false, /*bReceivesDecals*/false);
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	//
	// FGeomVertex
	//
	///////////////////////////////////////////////////////////////////////////////

	FVector FGeomVertex::GetMidPoint() const
	{
		return *this;
	}

	FVector* FGeomVertex::GetActualVertex(FPolyVertexIndex& InPVI)
	{
		return &(ParentObject->ActualBrush->Brush->Polys->Element[InPVI.PolyIndex].Vertices[InPVI.VertexIndex]);
	}

	///////////////////////////////////////////////////////////////////////////////
	//
	// FGeomEdge
	//
	///////////////////////////////////////////////////////////////////////////////

	// Returns true if InEdge matches this edge, independant of winding.
	bool FGeomEdge::IsSameEdge(const FGeomEdge& InEdge) const
	{
		return ((VertexIndices[0] == InEdge.VertexIndices[0] && VertexIndices[1] == InEdge.VertexIndices[1]) ||
			(VertexIndices[0] == InEdge.VertexIndices[1] && VertexIndices[1] == InEdge.VertexIndices[0]));
	}


	FVector FGeomEdge::GetMidPoint() const
	{
		const FGeomVertex* wk0 = &(ParentObject->VertexPool[VertexIndices[0]]);
		const FGeomVertex* wk1 = &(ParentObject->VertexPool[VertexIndices[1]]);

		const FVector v0(wk0->X, wk0->Y, wk0->Z);
		const FVector v1(wk1->X, wk1->Y, wk1->Z);

		return (v0 + v1) / 2;
	}

	///////////////////////////////////////////////////////////////////////////////
	//
	// FGeomPoly
	//
	///////////////////////////////////////////////////////////////////////////////

	FVector FGeomPoly::GetMidPoint() const
	{
		FVector Wk(0, 0, 0);
		int32 Count = 0;

		for (int32 e = 0; e < EdgeIndices.Num(); ++e)
		{
			const FGeomEdge* ge = &ParentObject->EdgePool[EdgeIndices[e]];
			Wk += ParentObject->VertexPool[ge->VertexIndices[0]];
			Count++;
			Wk += ParentObject->VertexPool[ge->VertexIndices[1]];
			Count++;
		}

		check(Count);
		return Wk / Count;
	}

	FPoly* FGeomPoly::GetActualPoly()
	{
		TSharedPtr<FGeomObject> Parent = ParentObject;
		check(Parent.IsValid());
		const ABrush* Brush = Parent->ActualBrush;
		check(Brush);
		return &(Brush->Brush->Polys->Element[ActualPolyIndex]);
	}

	///////////////////////////////////////////////////////////////////////////////
	//
	// FGeomObject
	//
	///////////////////////////////////////////////////////////////////////////////

	int32 FGeomObject::AddVertexToPool(int32 InParentPolyIndex, int32 InPolyIndex, int32 InVertexIndex)
	{
		FGeomVertex* gv = NULL;
		FVector CmpVtx = ActualBrush->Brush->Polys->Element[InPolyIndex].Vertices[InVertexIndex];

		// See if the vertex is already in the pool
		for (int32 x = 0; x < VertexPool.Num(); ++x)
		{
			if (FVector::PointsAreNear(VertexPool[x], CmpVtx, 0.5f))
			{
				gv = &VertexPool[x];
				gv->ActualVertexIndices.AddUnique(FPolyVertexIndex(InPolyIndex, InVertexIndex));
				gv->ParentPolyIndices.AddUnique(InParentPolyIndex);
				return x;
			}
		}

		// If not, add it...
		if (gv == NULL)
		{
			new(VertexPool)FGeomVertex(MakeShareable(this));
			gv = &VertexPool[VertexPool.Num() - 1];
			*gv = CmpVtx;
		}

		gv->ActualVertexIndices.AddUnique(FPolyVertexIndex(InPolyIndex, InVertexIndex));
		gv->ParentPolyIndices.AddUnique(InParentPolyIndex);

		return VertexPool.Num() - 1;
	}

	int32 FGeomObject::AddEdgeToPool(FGeomPoly* InPoly, int32 InParentPolyIndex, int32 InVectorIdxA, int32 InVectorIdxB)
	{
		const int32 idx0 = AddVertexToPool(InParentPolyIndex, InPoly->ActualPolyIndex, InVectorIdxA);
		const int32 idx1 = AddVertexToPool(InParentPolyIndex, InPoly->ActualPolyIndex, InVectorIdxB);

		// See if the edge is already in the pool.  If so, leave.
		FGeomEdge* ge = NULL;
		for (int32 e = 0; e < EdgePool.Num(); ++e)
		{
			if (EdgePool[e].VertexIndices[0] == idx0 && EdgePool[e].VertexIndices[1] == idx1)
			{
				EdgePool[e].ParentPolyIndices.Add(InPoly->ParentObject->PolyPool.Find(*InPoly));
				return e;
			}
		}

		// Add a new edge to the pool and set it up.
		new(EdgePool)FGeomEdge(MakeShareable(this));
		ge = &EdgePool[EdgePool.Num() - 1];

		ge->VertexIndices[0] = idx0;
		ge->VertexIndices[1] = idx1;

		ge->ParentPolyIndices.Add(InPoly->ParentObject->PolyPool.Find(*InPoly));

		return EdgePool.Num() - 1;
	}

	/**
	 * Removes all geometry data and reconstructs it from the source brushes.
	 */

	void FGeomObject::GetFromSource()
	{
		PolyPool.Empty();
		EdgePool.Empty();
		VertexPool.Empty();

		for (int32 p = 0; p < ActualBrush->Brush->Polys->Element.Num(); ++p)
		{
			FPoly* poly = &(ActualBrush->Brush->Polys->Element[p]);

			new(PolyPool)FGeomPoly(MakeShareable(this));
			FGeomPoly* gp = &PolyPool[PolyPool.Num() - 1];
			gp->ActualPolyIndex = p;

			for (int32 v = 1; v <= poly->Vertices.Num(); ++v)
			{
				int32 idx = (v == poly->Vertices.Num()) ? 0 : v,
					previdx = v - 1;

				int32 eidx = AddEdgeToPool(gp, PolyPool.Num() - 1, previdx, idx);
				gp->EdgeIndices.Add(eidx);
			}
		}

		ComputeData();
	}

	/**
	 * Recomputes data specific to the geometry data (i.e. normals, mid points, etc)
	 */

	void FGeomObject::ComputeData()
	{
		int32 Count;
		FVector Wk;

		// Polygons

		for (int32 p = 0; p < PolyPool.Num(); ++p)
		{
			FGeomPoly* poly = &PolyPool[p];

			poly->SetNormal(poly->GetActualPoly()->Normal);
			poly->SetMid(poly->GetMidPoint());

		}

		// Vertices (= average normal of all the polygons that touch it)

		FGeomVertex* gv;
		for (int32 v = 0; v < VertexPool.Num(); ++v)
		{
			gv = &VertexPool[v];
			Count = 0;
			Wk = FVector::ZeroVector;

			for (int32 e = 0; e < EdgePool.Num(); ++e)
			{
				FGeomEdge* ge = &EdgePool[e];

				FGeomVertex* v0 = &VertexPool[ge->VertexIndices[0]],
					* v1 = &VertexPool[ge->VertexIndices[1]];

				if (gv == v0 || gv == v1)
				{
					for (int32 p = 0; p < ge->ParentPolyIndices.Num(); ++p)
					{
						FGeomPoly* poly = &PolyPool[ge->ParentPolyIndices[p]];

						Wk += poly->GetNormal();
						Count++;
					}
				}
			}

			gv->SetNormal(Wk / Count);
			gv->SetMid(gv->GetMidPoint());
		}

		// Edges (= average normal of all the polygons that touch it)

		FGeomEdge* ge;
		for (int32 e = 0; e < EdgePool.Num(); ++e)
		{
			ge = &EdgePool[e];
			Count = 0;
			Wk = FVector::ZeroVector;

			for (int32 e2 = 0; e2 < EdgePool.Num(); ++e2)
			{
				FGeomEdge* ge2 = &EdgePool[e2];

				if (ge->IsSameEdge(*ge2))
				{
					for (int32 p = 0; p < ge2->ParentPolyIndices.Num(); ++p)
					{
						FGeomPoly* gp = &PolyPool[ge2->ParentPolyIndices[p]];

						Wk += gp->GetActualPoly()->Normal;
						Count++;

					}
				}
			}

			ge->SetNormal(Wk / Count);
			ge->SetMid(ge->GetMidPoint());
		}
	}

	FVector FGeomObject::GetMidPoint() const
	{
		return ActualBrush->GetActorLocation();
	}
}