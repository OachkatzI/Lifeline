// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

// I copied the entire FDynamicMeshBuilder code because I don't know how to draw with PDI in c++
// all i did here was to set "Mesh.bSelectable = false" so that drawn boxes are not clickable in editor!

#include "Utilities/LevelStreamAssistantUtilsRendering.h"
#include "RenderingThread.h"
#include "RenderResource.h"
#include "UniformBuffer.h"
#include "VertexFactory.h"
#include "MeshBatch.h"
#include "SceneManagement.h"
#include "ResourcePool.h"

namespace LevelStreamAssistantUtils
{
	class FGlobalDynamicMeshPoolPolicy2
	{
	public:
		/** Buffers are created with a simple byte size */
		typedef uint32 CreationArguments;
		enum
		{
			NumSafeFrames = 3, /** Number of frames to leaves buffers before reclaiming/reusing */
			NumPoolBucketSizes = 16, /** Number of pool buckets */
			NumToDrainPerFrame = 100, /** Max. number of resources to cull in a single frame */
			CullAfterFramesNum = 10 /** Resources are culled if unused for more frames than this */
		};

		/** Get the pool bucket index from the size
		 * @param Size the number of bytes for the resource
		 * @returns The bucket index.
		 */
		uint32 GetPoolBucketIndex(uint32 Size)
		{
			unsigned long Lower = 0;
			unsigned long Upper = NumPoolBucketSizes;
			unsigned long Middle;

			do
			{
				Middle = (Upper + Lower) >> 1;
				if (Size <= BucketSizes[Middle - 1])
				{
					Upper = Middle;
				}
				else
				{
					Lower = Middle;
				}
			} while (Upper - Lower > 1);

			check(Size <= BucketSizes[Lower]);
			check((Lower == 0) || (Size > BucketSizes[Lower - 1]));

			return Lower;
		}

		/** Get the pool bucket size from the index
		 * @param Bucket the bucket index
		 * @returns The bucket size.
		 */
		uint32 GetPoolBucketSize(uint32 Bucket)
		{
			check(Bucket < NumPoolBucketSizes);
			return BucketSizes[Bucket];
		}

	private:
		/** The bucket sizes */
		static uint32 BucketSizes[NumPoolBucketSizes];
	};

	uint32 FGlobalDynamicMeshPoolPolicy2::BucketSizes[NumPoolBucketSizes] = {
		64, 128, 256, 512, 1024, 2048, 4096,
		8 * 1024, 16 * 1024, 32 * 1024, 64 * 1024, 128 * 1024, 256 * 1024,
		512 * 1024, 1 * 1024 * 1024, 2 * 1024 * 1024
	};
	
	typedef int32 DynamicMeshIndexType2;
	class FGlobalDynamicMeshIndexPolicy2 : public FGlobalDynamicMeshPoolPolicy2
	{
	public:
		enum
		{
			NumSafeFrames = FGlobalDynamicMeshPoolPolicy2::NumSafeFrames,
			NumPoolBuckets = FGlobalDynamicMeshPoolPolicy2::NumPoolBucketSizes,
			NumToDrainPerFrame = FGlobalDynamicMeshPoolPolicy2::NumToDrainPerFrame,
			CullAfterFramesNum = FGlobalDynamicMeshPoolPolicy2::CullAfterFramesNum
		};

		/** Creates the resource
		 * @param Args The buffer size in bytes.
		 * @returns A suitably sized buffer or NULL on failure.
		 */
		FIndexBufferRHIRef CreateResource(FGlobalDynamicMeshPoolPolicy2::CreationArguments Args)
		{
			FGlobalDynamicMeshPoolPolicy2::CreationArguments BufferSize = GetPoolBucketSize(GetPoolBucketIndex(Args));
			// The use of BUF_Static is deliberate - on OS X the buffer backing-store orphaning & reallocation will dominate execution time
			// so to avoid this we don't reuse a buffer for several frames, thereby avoiding the pipeline stall and the reallocation cost.
			FRHIResourceCreateInfo CreateInfo;
			FIndexBufferRHIRef VertexBuffer = RHICreateIndexBuffer(sizeof(DynamicMeshIndexType2), BufferSize, BUF_Static, CreateInfo);
			return VertexBuffer;
		}

		/** Gets the arguments used to create resource
		 * @param Resource The buffer to get data for.
		 * @returns The arguments used to create the buffer.
		 */
		FGlobalDynamicMeshPoolPolicy2::CreationArguments GetCreationArguments(FIndexBufferRHIRef Resource)
		{
			return (Resource->GetSize());
		}

		/** Frees the resource
		 * @param Resource The buffer to prepare for release from the pool permanently.
		 */
		void FreeResource(FIndexBufferRHIRef Resource)
		{
		}
	};

	class FGlobalDynamicMeshIndexPool2 : public TRenderResourcePool<FIndexBufferRHIRef, FGlobalDynamicMeshIndexPolicy2, FGlobalDynamicMeshPoolPolicy2::CreationArguments>
	{
	public:
		/** Destructor */
		virtual ~FGlobalDynamicMeshIndexPool2()
		{
		}

	public: // From FTickableObjectRenderThread
		virtual TStatId GetStatId() const override
		{
			RETURN_QUICK_DECLARE_CYCLE_STAT(FGlobalDynamicMeshIndexPool2, STATGROUP_Tickables);
		}
	};
	TGlobalResource<FGlobalDynamicMeshIndexPool2> GDynamicMeshIndexPool2;

	class FGlobalDynamicMeshVertexPolicy2 : public FGlobalDynamicMeshPoolPolicy2
	{
	public:
		enum
		{
			NumSafeFrames = FGlobalDynamicMeshPoolPolicy2::NumSafeFrames,
			NumPoolBuckets = FGlobalDynamicMeshPoolPolicy2::NumPoolBucketSizes,
			NumToDrainPerFrame = FGlobalDynamicMeshPoolPolicy2::NumToDrainPerFrame,
			CullAfterFramesNum = FGlobalDynamicMeshPoolPolicy2::CullAfterFramesNum
		};

		/** Creates the resource
		 * @param Args The buffer size in bytes.
		 * @returns A suitably sized buffer or NULL on failure.
		 */
		FVertexBufferRHIRef CreateResource(FGlobalDynamicMeshPoolPolicy2::CreationArguments Args)
		{
			FGlobalDynamicMeshPoolPolicy2::CreationArguments BufferSize = GetPoolBucketSize(GetPoolBucketIndex(Args));
			FRHIResourceCreateInfo CreateInfo;
			FVertexBufferRHIRef VertexBuffer = RHICreateVertexBuffer(BufferSize, BUF_Volatile | BUF_ShaderResource, CreateInfo);
			return VertexBuffer;
		}

		/** Gets the arguments used to create resource
		 * @param Resource The buffer to get data for.
		 * @returns The arguments used to create the buffer.
		 */
		FGlobalDynamicMeshPoolPolicy2::CreationArguments GetCreationArguments(FVertexBufferRHIRef Resource)
		{
			return (Resource->GetSize());
		}

		/** Frees the resource
		 * @param Resource The buffer to prepare for release from the pool permanently.
		 */
		void FreeResource(FVertexBufferRHIRef Resource)
		{
		}
	};

	class FGlobalDynamicMeshVertexPool2 : public TRenderResourcePool<FVertexBufferRHIRef, FGlobalDynamicMeshVertexPolicy2, FGlobalDynamicMeshPoolPolicy2::CreationArguments>
	{
	public:
		/** Destructor */
		virtual ~FGlobalDynamicMeshVertexPool2()
		{
		}

	public: // From FTickableObjectRenderThread
		virtual TStatId GetStatId() const override
		{
			RETURN_QUICK_DECLARE_CYCLE_STAT(FGlobalDynamicMeshVertexPool2, STATGROUP_Tickables);
		}
	};
	TGlobalResource<FGlobalDynamicMeshVertexPool2> GDynamicMeshVertexPool2;

	/** The index buffer type used for dynamic meshes. */
	class FPooledDynamicMeshIndexBuffer2 : public FDynamicPrimitiveResource, public FIndexBuffer
	{
	public:
		TArray<DynamicMeshIndexType2> Indices;

		// FRenderResource interface.
		virtual void InitRHI() override
		{
			uint32 SizeInBytes = Indices.Num() * sizeof(DynamicMeshIndexType2);
			if (SizeInBytes <= FGlobalDynamicMeshIndexPolicy2().GetPoolBucketSize(FGlobalDynamicMeshIndexPolicy2::NumPoolBuckets - 1))
			{
				IndexBufferRHI = GDynamicMeshIndexPool2.CreatePooledResource(SizeInBytes);
			}
			else
			{
				FRHIResourceCreateInfo CreateInfo;
				IndexBufferRHI = RHICreateIndexBuffer(sizeof(DynamicMeshIndexType2), SizeInBytes, BUF_Volatile, CreateInfo);
			}

			// Write the indices to the index buffer.
			void* Buffer = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(DynamicMeshIndexType2), RLM_WriteOnly);
			FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(DynamicMeshIndexType2));
			RHIUnlockIndexBuffer(IndexBufferRHI);
		}

		virtual void ReleaseRHI() override
		{
			if (IndexBufferRHI->GetSize() <= FGlobalDynamicMeshIndexPolicy2().GetPoolBucketSize(FGlobalDynamicMeshIndexPolicy2::NumPoolBuckets - 1))
			{
				GDynamicMeshIndexPool2.ReleasePooledResource(IndexBufferRHI);
				IndexBufferRHI = NULL;
			}
			FIndexBuffer::ReleaseRHI();
		}

		// FDynamicPrimitiveResource interface.
		virtual void InitPrimitiveResource() override
		{
			InitResource();
		}
		virtual void ReleasePrimitiveResource() override
		{
			ReleaseResource();
			delete this;
		}
	};

	static FVertexBufferRHIRef AllocVertexBuffer2(uint32 Stride, uint32 NumElements)
	{
		FVertexBufferRHIRef VertexBufferRHI;
		uint32 SizeInBytes = NumElements * Stride;
		if (SizeInBytes <= FGlobalDynamicMeshIndexPolicy2().GetPoolBucketSize(FGlobalDynamicMeshIndexPolicy2::NumPoolBuckets - 1))
		{
			VertexBufferRHI = GDynamicMeshVertexPool2.CreatePooledResource(SizeInBytes);
		}
		else
		{
			FRHIResourceCreateInfo CreateInfo;
			VertexBufferRHI = RHICreateVertexBuffer(SizeInBytes, BUF_Volatile | BUF_ShaderResource, CreateInfo);
		}

		return VertexBufferRHI;
	}

	static void ReleaseVertexBuffer2(FVertexBuffer& VertexBuffer)
	{
		if (IsValidRef(VertexBuffer.VertexBufferRHI)
			&& VertexBuffer.VertexBufferRHI->GetSize() <= FGlobalDynamicMeshVertexPolicy2().GetPoolBucketSize(FGlobalDynamicMeshVertexPolicy2::NumPoolBuckets - 1))
		{
			GDynamicMeshVertexPool2.ReleasePooledResource(VertexBuffer.VertexBufferRHI);
			VertexBuffer.VertexBufferRHI = nullptr;
		}
	}

	/** The vertex buffer type used for dynamic meshes. */
	class FPooledDynamicMeshVertexBuffer2 : public FDynamicPrimitiveResource, public FRenderResource
	{
	public:
		FVertexBuffer PositionBuffer;
		FVertexBuffer TangentBuffer;
		FVertexBuffer TexCoordBuffer;
		FVertexBuffer ColorBuffer;

		FShaderResourceViewRHIRef TangentBufferSRV;
		FShaderResourceViewRHIRef TexCoordBufferSRV;
		FShaderResourceViewRHIRef ColorBufferSRV;
		FShaderResourceViewRHIRef PositionBufferSRV;

		TArray<FDynamicMeshVertex> Vertices;

		FPooledDynamicMeshVertexBuffer2(uint32 InNumTexCoords, uint32 InLightmapCoordinateIndex, bool InUse16bitTexCoord) : NumTexCoords(InNumTexCoords), LightmapCoordinateIndex(InLightmapCoordinateIndex), Use16bitTexCoord(InUse16bitTexCoord)
		{
			check(NumTexCoords > 0 && NumTexCoords <= MAX_STATIC_TEXCOORDS);
			check(LightmapCoordinateIndex < NumTexCoords);
		}

		// FRenderResource interface.
		virtual void InitRHI() override
		{
			uint32 TextureStride = sizeof(FVector2D);
			EPixelFormat TextureFormat = PF_G32R32F;

			if (Use16bitTexCoord)
			{
				TextureStride = sizeof(FVector2DHalf);
				TextureFormat = PF_G16R16F;
			}

			PositionBuffer.VertexBufferRHI = AllocVertexBuffer2(sizeof(FVector), Vertices.Num());
			TangentBuffer.VertexBufferRHI = AllocVertexBuffer2(sizeof(FPackedNormal), 2 * Vertices.Num());
			TexCoordBuffer.VertexBufferRHI = AllocVertexBuffer2(TextureStride, NumTexCoords * Vertices.Num());
			ColorBuffer.VertexBufferRHI = AllocVertexBuffer2(sizeof(FColor), Vertices.Num());

			if (RHISupportsManualVertexFetch(GMaxRHIShaderPlatform))
			{
				TangentBufferSRV = RHICreateShaderResourceView(TangentBuffer.VertexBufferRHI, 4, PF_R8G8B8A8_SNORM);
				TexCoordBufferSRV = RHICreateShaderResourceView(TexCoordBuffer.VertexBufferRHI, TextureStride, TextureFormat);
				ColorBufferSRV = RHICreateShaderResourceView(ColorBuffer.VertexBufferRHI, 4, PF_R8G8B8A8);

				PositionBufferSRV = RHICreateShaderResourceView(PositionBuffer.VertexBufferRHI, sizeof(float), PF_R32_FLOAT);
			}

			void* TexCoordBufferData = RHILockVertexBuffer(TexCoordBuffer.VertexBufferRHI, 0, NumTexCoords * TextureStride * Vertices.Num(), RLM_WriteOnly);
			FVector2D* TexCoordBufferData32 = !Use16bitTexCoord ? static_cast<FVector2D*>(TexCoordBufferData) : nullptr;
			FVector2DHalf* TexCoordBufferData16 = Use16bitTexCoord ? static_cast<FVector2DHalf*>(TexCoordBufferData) : nullptr;

			// Copy the vertex data into the vertex buffers.
			FVector* PositionBufferData = static_cast<FVector*>(RHILockVertexBuffer(PositionBuffer.VertexBufferRHI, 0, sizeof(FVector) * Vertices.Num(), RLM_WriteOnly));
			FPackedNormal* TangentBufferData = static_cast<FPackedNormal*>(RHILockVertexBuffer(TangentBuffer.VertexBufferRHI, 0, 2 * sizeof(FPackedNormal) * Vertices.Num(), RLM_WriteOnly));
			FColor* ColorBufferData = static_cast<FColor*>(RHILockVertexBuffer(ColorBuffer.VertexBufferRHI, 0, sizeof(FColor) * Vertices.Num(), RLM_WriteOnly));

			for (int32 i = 0; i < Vertices.Num(); i++)
			{
				PositionBufferData[i] = Vertices[i].Position;
				TangentBufferData[2 * i + 0] = Vertices[i].TangentX;
				TangentBufferData[2 * i + 1] = Vertices[i].TangentZ;
				ColorBufferData[i] = Vertices[i].Color;

				for (uint32 j = 0; j < NumTexCoords; j++)
				{
					if (Use16bitTexCoord)
					{
						TexCoordBufferData16[NumTexCoords * i + j] = FVector2DHalf(Vertices[i].TextureCoordinate[j]);
					}
					else
					{
						TexCoordBufferData32[NumTexCoords * i + j] = Vertices[i].TextureCoordinate[j];
					}
				}
			}

			RHIUnlockVertexBuffer(PositionBuffer.VertexBufferRHI);
			RHIUnlockVertexBuffer(TangentBuffer.VertexBufferRHI);
			RHIUnlockVertexBuffer(TexCoordBuffer.VertexBufferRHI);
			RHIUnlockVertexBuffer(ColorBuffer.VertexBufferRHI);
		}

		void InitResource() override
		{
			FRenderResource::InitResource();
			PositionBuffer.InitResource();
			TangentBuffer.InitResource();
			TexCoordBuffer.InitResource();
			ColorBuffer.InitResource();
		}

		void ReleaseResource() override
		{
			FRenderResource::ReleaseResource();
			PositionBuffer.ReleaseResource();
			TangentBuffer.ReleaseResource();
			TexCoordBuffer.ReleaseResource();
			ColorBuffer.ReleaseResource();
		}

		virtual void ReleaseRHI() override
		{
			ReleaseVertexBuffer2(PositionBuffer);
			ReleaseVertexBuffer2(TangentBuffer);
			ReleaseVertexBuffer2(TexCoordBuffer);
			ReleaseVertexBuffer2(ColorBuffer);
		}

		// FDynamicPrimitiveResource interface.
		virtual void InitPrimitiveResource() override
		{
			InitResource();
		}

		virtual void ReleasePrimitiveResource() override
		{
			ReleaseResource();
			delete this;
		}

		const uint32 GetNumTexCoords() const
		{
			return NumTexCoords;
		}

		const uint32 GetLightmapCoordinateIndex() const
		{
			return LightmapCoordinateIndex;
		}

		const bool GetUse16bitTexCoords() const
		{
			return Use16bitTexCoord;
		}
	private:
		const uint32 NumTexCoords;
		const uint32 LightmapCoordinateIndex;
		const bool Use16bitTexCoord;
	};

	/** The vertex factory type used for dynamic meshes. */
	class FPooledDynamicMeshVertexFactory2 : public FDynamicPrimitiveResource, public FLocalVertexFactory
	{
	public:

		/** Initialization constructor. */
		FPooledDynamicMeshVertexFactory2(ERHIFeatureLevel::Type InFeatureLevel, const FPooledDynamicMeshVertexBuffer2* InVertexBuffer) : FLocalVertexFactory(InFeatureLevel, "FPooledDynamicMeshVertexFactory2"), VertexBuffer(InVertexBuffer) {}

		void InitResource() override
		{
			FLocalVertexFactory* VertexFactory = this;
			const FPooledDynamicMeshVertexBuffer2* PooledVertexBuffer = VertexBuffer;
			ENQUEUE_RENDER_COMMAND(InitDynamicMeshVertexFactory)(
				[VertexFactory, PooledVertexBuffer](FRHICommandListImmediate& RHICmdList)
				{
					FDataType Data;
					Data.PositionComponent = FVertexStreamComponent(
						&PooledVertexBuffer->PositionBuffer,
						0,
						sizeof(FVector),
						VET_Float3
					);

					Data.NumTexCoords = PooledVertexBuffer->GetNumTexCoords();
					{
						Data.LightMapCoordinateIndex = PooledVertexBuffer->GetLightmapCoordinateIndex();
						Data.TangentsSRV = PooledVertexBuffer->TangentBufferSRV;
						Data.TextureCoordinatesSRV = PooledVertexBuffer->TexCoordBufferSRV;
						Data.ColorComponentsSRV = PooledVertexBuffer->ColorBufferSRV;
						Data.PositionComponentSRV = PooledVertexBuffer->PositionBufferSRV;
					}

					{
						EVertexElementType UVDoubleWideVertexElementType = VET_None;
						EVertexElementType UVVertexElementType = VET_None;
						uint32 UVSizeInBytes = 0;
						if (PooledVertexBuffer->GetUse16bitTexCoords())
						{
							UVSizeInBytes = sizeof(FVector2DHalf);
							UVDoubleWideVertexElementType = VET_Half4;
							UVVertexElementType = VET_Half2;
						}
						else
						{
							UVSizeInBytes = sizeof(FVector2D);
							UVDoubleWideVertexElementType = VET_Float4;
							UVVertexElementType = VET_Float2;
						}

						int32 UVIndex;
						uint32 UvStride = UVSizeInBytes * PooledVertexBuffer->GetNumTexCoords();
						for (UVIndex = 0; UVIndex < (int32)PooledVertexBuffer->GetNumTexCoords() - 1; UVIndex += 2)
						{
							Data.TextureCoordinates.Add
							(
								FVertexStreamComponent(
									&PooledVertexBuffer->TexCoordBuffer,
									UVSizeInBytes * UVIndex,
									UvStride,
									UVDoubleWideVertexElementType,
									EVertexStreamUsage::ManualFetch
								)
							);
						}

						// possible last UV channel if we have an odd number
						if (UVIndex < (int32)PooledVertexBuffer->GetNumTexCoords())
						{
							Data.TextureCoordinates.Add(FVertexStreamComponent(
								&PooledVertexBuffer->TexCoordBuffer,
								UVSizeInBytes * UVIndex,
								UvStride,
								UVVertexElementType,
								EVertexStreamUsage::ManualFetch
							));
						}

						Data.TangentBasisComponents[0] = FVertexStreamComponent(&PooledVertexBuffer->TangentBuffer, 0, 2 * sizeof(FPackedNormal), VET_PackedNormal, EVertexStreamUsage::ManualFetch);
						Data.TangentBasisComponents[1] = FVertexStreamComponent(&PooledVertexBuffer->TangentBuffer, sizeof(FPackedNormal), 2 * sizeof(FPackedNormal), VET_PackedNormal, EVertexStreamUsage::ManualFetch);
						Data.ColorComponent = FVertexStreamComponent(&PooledVertexBuffer->ColorBuffer, 0, sizeof(FColor), VET_Color, EVertexStreamUsage::ManualFetch);
					}
					VertexFactory->SetData(Data);
				});

			FLocalVertexFactory::InitResource();
		}

		// FDynamicPrimitiveResource interface.
		void InitPrimitiveResource() override
		{
			InitResource();
		}

		void ReleasePrimitiveResource() override
		{
			ReleaseResource();
			delete this;
		}

	private:
		const FPooledDynamicMeshVertexBuffer2* VertexBuffer;

	};

	/** The primitive uniform buffer used for dynamic meshes. */
	class FDynamicMeshPrimitiveUniformBuffer2 : public FDynamicPrimitiveResource, public TUniformBuffer<FPrimitiveUniformShaderParameters>
	{
	public:

		// FDynamicPrimitiveResource interface.
		virtual void InitPrimitiveResource()
		{
			InitResource();
		}
		virtual void ReleasePrimitiveResource()
		{
			ReleaseResource();
			delete this;
		}
	};

	FDynamicMeshBuilder2::FDynamicMeshBuilder2(ERHIFeatureLevel::Type InFeatureLevel, uint32 InNumTexCoords, uint32 InLightmapCoordinateIndex, bool InUse16bitTexCoord)
		: FeatureLevel(InFeatureLevel)
	{
		VertexBuffer = new FPooledDynamicMeshVertexBuffer2(InNumTexCoords, InLightmapCoordinateIndex, InUse16bitTexCoord);
		IndexBuffer = new FPooledDynamicMeshIndexBuffer2;
	}

	FDynamicMeshBuilder2::~FDynamicMeshBuilder2()
	{
		//Delete the resources if they have not been already. At this point they are only valid if Draw() has never been called,
		//so the resources have not been passed to the rendering thread.  Also they do not need to be released,
		//since they are only initialized when Draw() is called.
		delete VertexBuffer;
		delete IndexBuffer;
	}

	int32 FDynamicMeshBuilder2::AddVertex(
		const FVector& InPosition,
		const FVector2D& InTextureCoordinate,
		const FVector& InTangentX,
		const FVector& InTangentY,
		const FVector& InTangentZ,
		const FColor& InColor
	)
	{
		int32 VertexIndex = VertexBuffer->Vertices.Num();
		FDynamicMeshVertex* Vertex = new(VertexBuffer->Vertices) FDynamicMeshVertex;
		Vertex->Position = InPosition;
		Vertex->TextureCoordinate[0] = InTextureCoordinate;
		Vertex->TangentX = InTangentX;
		Vertex->TangentZ = InTangentZ;
		Vertex->TangentZ.Vector.W = GetBasisDeterminantSignByte(InTangentX, InTangentY, InTangentZ);
		Vertex->Color = InColor;

		return VertexIndex;
	}

	/** Adds a triangle to the mesh. */
	void FDynamicMeshBuilder2::AddTriangle(int32 V0, int32 V1, int32 V2)
	{
		IndexBuffer->Indices.Add(static_cast<DynamicMeshIndexType2>(V0));
		IndexBuffer->Indices.Add(static_cast<DynamicMeshIndexType2>(V1));
		IndexBuffer->Indices.Add(static_cast<DynamicMeshIndexType2>(V2));
	}

	void FDynamicMeshBuilder2::Draw(FPrimitiveDrawInterface* PDI, const FMatrix& LocalToWorld, const FMaterialRenderProxy* MaterialRenderProxy, uint8 DepthPriorityGroup, bool bDisableBackfaceCulling, bool bReceivesDecals, const FHitProxyId HitProxyId)
	{
		// Only draw non-empty meshes.
		if (VertexBuffer->Vertices.Num() > 0 && IndexBuffer->Indices.Num() > 0)
		{
			// Register the dynamic resources with the PDI.
			PDI->RegisterDynamicResource(VertexBuffer);

			if (IndexBuffer && IndexBuffer->Indices.Num())
			{
				PDI->RegisterDynamicResource(IndexBuffer);
			}

			// Create the vertex factory.
			FPooledDynamicMeshVertexFactory2* VertexFactory = new FPooledDynamicMeshVertexFactory2(FeatureLevel, VertexBuffer);
			PDI->RegisterDynamicResource(VertexFactory);

			// Create the primitive uniform buffer.
			FDynamicMeshPrimitiveUniformBuffer2* PrimitiveUniformBuffer = new FDynamicMeshPrimitiveUniformBuffer2();
			FPrimitiveUniformShaderParameters PrimitiveParams = GetPrimitiveUniformShaderParameters(
				LocalToWorld,
				LocalToWorld,
				LocalToWorld.GetOrigin(),
				FBoxSphereBounds(EForceInit::ForceInit),
				FBoxSphereBounds(EForceInit::ForceInit),
				bReceivesDecals,
				false,
				false,
				false,
				false,
				/* bDrawsVelocity = */ true,
				GetDefaultLightingChannelMask(),
				0,
				INDEX_NONE,
				INDEX_NONE,
				false
			);


			if (IsInGameThread())
			{
				BeginSetUniformBufferContents(*PrimitiveUniformBuffer, PrimitiveParams);
			}
			else
			{
				PrimitiveUniformBuffer->SetContents(PrimitiveParams);
			}
			PDI->RegisterDynamicResource(PrimitiveUniformBuffer);

			const bool HasValidIndexBuffer = IndexBuffer && IndexBuffer->Indices.Num();
			// Draw the mesh.
			FMeshBatch Mesh;
			FMeshBatchElement& BatchElement = Mesh.Elements[0];
			BatchElement.IndexBuffer = HasValidIndexBuffer ? IndexBuffer : nullptr;
			Mesh.VertexFactory = VertexFactory;
			Mesh.MaterialRenderProxy = MaterialRenderProxy;
			BatchElement.PrimitiveUniformBufferResource = PrimitiveUniformBuffer;
			// previous l2w not used so treat as static
			BatchElement.FirstIndex = 0;
			BatchElement.NumPrimitives = HasValidIndexBuffer ? (IndexBuffer->Indices.Num() / 3) : (VertexBuffer->Vertices.Num() / 3);
			BatchElement.MinVertexIndex = 0;
			BatchElement.MaxVertexIndex = VertexBuffer->Vertices.Num() - 1;
			Mesh.ReverseCulling = LocalToWorld.Determinant() < 0.0f ? true : false;
			Mesh.bDisableBackfaceCulling = bDisableBackfaceCulling;
			Mesh.Type = PT_TriangleList;
			Mesh.DepthPriorityGroup = DepthPriorityGroup;
			Mesh.BatchHitProxyId = HitProxyId;

			// highlights should not be selectable.
			Mesh.bSelectable = false;

			PDI->DrawMesh(Mesh);

			// Clear the resource pointers so they cannot be overwritten accidentally.
			// These resources will be released by the PDI.
			VertexBuffer = NULL;
			IndexBuffer = NULL;
		}
	}
}