// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

// I copied the entire FDynamicMeshBuilder code because I don't know how to draw with PDI in c++
// all i did here was to set "Mesh.bSelectable = false" so that drawn boxes are not clickable in editor!

#pragma once

#include "CoreMinimal.h"

class FPrimitiveDrawInterface;
class FMaterialRenderProxy;
class FHitProxyId;
namespace ERHIFeatureLevel { enum Type; }

namespace LevelStreamAssistantUtils
{
	/**
	 * A utility used to construct dynamically generated meshes, and render them to a FPrimitiveDrawInterface.
	 * Note: This is meant to be easy to use, not fast.  It moves the data around more than necessary, and requires dynamically allocating RHI
	 * resources.  Exercise caution.
	 */
	class FDynamicMeshBuilder2
	{
	public:
		FDynamicMeshBuilder2(ERHIFeatureLevel::Type InFeatureLevel, uint32 InNumTexCoords = 1, uint32 InLightmapCoordinateIndex = 0, bool InUse16bitTexCoord = false);
		~FDynamicMeshBuilder2();

		int32 AddVertex(
			const FVector& InPosition,
			const FVector2D& InTextureCoordinate,
			const FVector& InTangentX,
			const FVector& InTangentY,
			const FVector& InTangentZ,
			const FColor& InColor
		);

		void AddTriangle(int32 V0, int32 V1, int32 V2);

		/**
		 * Draws the mesh to the given primitive draw interface.
		 * @param PDI - The primitive draw interface to draw the mesh on.
		 * @param LocalToWorld - The local to world transform to apply to the vertices of the mesh.
		 * @param FMaterialRenderProxy - The material instance to render on the mesh.
		 * @param DepthPriorityGroup - The depth priority group to render the mesh in.
		 * @param HitProxyId - Hit proxy to use for this mesh.  Use INDEX_NONE for no hit proxy.
		 */
		void Draw(FPrimitiveDrawInterface* PDI, const FMatrix& LocalToWorld, const FMaterialRenderProxy* MaterialRenderProxy, uint8 DepthPriorityGroup, bool bDisableBackfaceCulling = false, bool bReceivesDecals = true, const FHitProxyId HitProxyId = FHitProxyId());

	private:
		class FPooledDynamicMeshIndexBuffer2* IndexBuffer = nullptr;
		class FPooledDynamicMeshVertexBuffer2* VertexBuffer = nullptr;
		ERHIFeatureLevel::Type FeatureLevel;
	};
}
