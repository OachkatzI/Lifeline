// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

// I copied some code from EditorGeometry to build mesh from ABrush and render highlights for ABrush instead of rendering a simple Box.

#pragma once

#include "CoreMinimal.h"

class ABrush;
class FSceneView;
class FPrimitiveDrawInterface;

namespace LevelStreamAssistantUtils
{
	void DrawBox2(const FMatrix& BoxToWorld, const FVector& Radii, const FLinearColor& Color, const FSceneView& View, FPrimitiveDrawInterface& PDI);

	void RenderPoly(const ABrush& BrushActor, const FLinearColor& Color, const FSceneView& View, FPrimitiveDrawInterface& PDI);

	class FGeomObject;

	/**
	 * Base class for all classes that support storing and editing of geometry.
	 */
	class FGeomBase
	{
	public:
		FGeomBase(TSharedPtr<FGeomObject> ParentObject) : ParentObject(ParentObject){}
		FGeomBase() {}
		virtual ~FGeomBase() {}

		TSharedPtr<FGeomObject> ParentObject;

		/** Returns a location that represents the middle of the object */
		virtual FVector GetMidPoint() const = 0;

		/** Returns true if this geometry objects is a vertex. */
		virtual bool IsVertex() const { return 0; }

		const FVector& GetNormal() const { return Normal; }
		const FVector& GetMid() const { return Mid; }

		void SetNormal(const FVector& InNormal) { Normal = InNormal; }
		void SetMid(const FVector& InMid) { Mid = InMid; }

	protected:
		/** The normal vector for this object. */
		FVector Normal = FVector::ZeroVector;

		/** The mid point for this object. */
		FVector Mid;
	};

	/**
	 * An index pair denoting a polygon and vertex within the parent objects ABrush.
	 */
	struct FPolyVertexIndex
	{
		FPolyVertexIndex()
		{
			PolyIndex = VertexIndex = INDEX_NONE;
		}
		FPolyVertexIndex(int32 InPolyIndex, int32 InVertexIndex)
		{
			PolyIndex = InPolyIndex;
			VertexIndex = InVertexIndex;
		}

		int32 PolyIndex, VertexIndex;

		bool operator==(const FPolyVertexIndex& In) const
		{
			if (In.PolyIndex != PolyIndex
				|| In.VertexIndex != VertexIndex)
			{
				return 0;
			}

			return 1;
		}
	};

	/**
	 * A 3D position.
	 */
	class FGeomVertex : public FGeomBase, public FVector
	{
	public:
		FGeomVertex(TSharedPtr<FGeomObject> ParentObject) : FGeomBase(ParentObject) {
			X = Y = Z = 0;
		}

		virtual FVector GetMidPoint() const;

		virtual bool IsVertex() const { return 1; }

		/** The list of vertices that this vertex represents. */
		TArray<FPolyVertexIndex> ActualVertexIndices;
		FVector* GetActualVertex(FPolyVertexIndex& InPVI);

		/**
		 * Indices into the parent poly pool. A vertex can belong to more
		 * than one poly and this keeps track of that relationship.
		 */
		TArray<int32> ParentPolyIndices;

		/** Assignment simply copies off the vertex position. */
		FGeomVertex& operator=(const FVector& In)
		{
			X = In.X;
			Y = In.Y;
			Z = In.Z;
			return *this;
		}
	};

	/**
	 * The space between 2 adjacent vertices.
	 */
	class FGeomEdge : public FGeomBase
	{
	public:
		FGeomEdge(TSharedPtr<FGeomObject> ParentObject) : FGeomBase(ParentObject) {
			VertexIndices[0] = INDEX_NONE;
			VertexIndices[1] = INDEX_NONE;
		}

		virtual FVector GetMidPoint() const;

		/** Returns true if InEdge matches this edge, independant of winding. */
		bool IsSameEdge(const FGeomEdge& InEdge) const;

		/**
		 * Indices into the parent poly pool. An edge can belong to more
		 * than one poly and this keeps track of that relationship.
		 */
		TArray<int32> ParentPolyIndices;

		/** Indices into the parent vertex pool. */
		int32 VertexIndices[2];
	};

	/**
	 * An individual polygon.
	 */
	class FGeomPoly : public FGeomBase
	{
	public:
		FGeomPoly(TSharedPtr<FGeomObject> ParentObject) : FGeomBase(ParentObject) {}

		virtual FVector GetMidPoint() const;

		/**
		 * The polygon this poly represents.  This is an index into the polygon array inside
		 * the ABrush that is selected in this objects parent FGeomObject.
		 */
		int32 ActualPolyIndex;
		FPoly* GetActualPoly();

		/** Array of indices into the parent objects edge pool. */
		TArray<int32> EdgeIndices;

		bool operator==(const FGeomPoly& In) const
		{
			if (In.ActualPolyIndex != ActualPolyIndex
				|| In.EdgeIndices.Num() != EdgeIndices.Num())
			{
				return 0;
			}

			for (int32 x = 0; x < EdgeIndices.Num(); ++x)
			{
				if (EdgeIndices[x] != In.EdgeIndices[x])
				{
					return 0;
				}
			}

			return 1;
		}
	};

	/**
	 * A group of polygons forming an individual object.
	 */
	class FGeomObject : public FGeomBase
	{
	public:

		virtual FVector GetMidPoint() const override;

		/** Index to the ABrush actor this object represents. */
		const ABrush* ActualBrush = nullptr;

		/** Master lists.  All lower data types refer to the contents of these pools through indices. */
		TArray<FGeomVertex> VertexPool;
		TArray<FGeomEdge> EdgePool;
		TArray<FGeomPoly> PolyPool;

		/** Tells the object to recompute all of it's internal data. */
		void ComputeData();

		int32 AddVertexToPool(int32 InParentPolyIndex, int32 InPolyIndex, int32 InVertexIndex);
		int32 AddEdgeToPool(FGeomPoly* InPoly, int32 InParentPolyIndex, int32 InVectorIdxA, int32 InVectorIdxB);
		virtual void GetFromSource();
	};
}
