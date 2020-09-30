#include "stdafx.h"
#include "MeshFilterLoader.h"
#include "BinaryReader.h"
#include "EffectHelper.h"

//#define MULTI_TEXCOORD
#define OVM_vMAJOR 1
#ifdef MULTI_TEXCOORD
#define OVM_vMINOR 61
#else
#define OVM_vMINOR 1
#endif


MeshFilter * MeshFilterLoader::LoadContent(const std::wstring & assetFile)
{
	auto binReader = new BinaryReader();
	binReader->Open(assetFile);

	if (!binReader->Exists())
		return nullptr;

	//READ OVM FILE
	const int versionMajor = binReader->Read<char>();
	const int versionMinor = binReader->Read<char>();

	if (versionMajor != OVM_vMAJOR || versionMinor != OVM_vMINOR)
	{
		Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::Load() > Wrong OVM version\n\tFile: \"%s\" \n\tExpected version %i.%i, not %i.%i.", assetFile.c_str(), OVM_vMAJOR, OVM_vMINOR, versionMajor, versionMinor);
		delete binReader;
		return nullptr;
	}

	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;

	auto pMesh = new MeshFilter();

	for (;;)
	{
		const auto meshDataType = static_cast<MeshDataType>(binReader->Read<char>());
		if (meshDataType == MeshDataType::END)
			break;

		const auto dataOffset = binReader->Read<unsigned int>();

		switch (meshDataType)
		{
		case MeshDataType::HEADER:
		{
			pMesh->m_MeshName = binReader->ReadString();
			vertexCount = binReader->Read<unsigned int>();
			indexCount = binReader->Read<unsigned int>();

			pMesh->m_VertexCount = vertexCount;
			pMesh->m_IndexCount = indexCount;
		}
		break;
		case MeshDataType::POSITIONS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::POSITION);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 pos;
				pos.x = binReader->Read<float>();
				pos.y = binReader->Read<float>();
				pos.z = binReader->Read<float>();
				pMesh->m_Positions.push_back(pos);
			}
		}
		break;
		case MeshDataType::INDICES:
		{
			for (unsigned int i = 0; i < indexCount; ++i)
			{
				pMesh->m_Indices.push_back(binReader->Read<DWORD>());
			}
		}
		break;
		case MeshDataType::NORMALS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::NORMAL);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 normal;
				normal.x = binReader->Read<float>();
				normal.y = binReader->Read<float>();
				normal.z = binReader->Read<float>();
				pMesh->m_Normals.push_back(normal);
			}
		}
		break;
		case MeshDataType::TANGENTS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::TANGENT);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 tangent;
				tangent.x = binReader->Read<float>();
				tangent.y = binReader->Read<float>();
				tangent.z = binReader->Read<float>();
				pMesh->m_Tangents.push_back(tangent);
			}
		}
		break;
		case MeshDataType::BINORMALS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::BINORMAL);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 binormal;
				binormal.x = binReader->Read<float>();
				binormal.y = binReader->Read<float>();
				binormal.z = binReader->Read<float>();
				pMesh->m_Binormals.push_back(binormal);
			}
		}
		break;
		case MeshDataType::TEXCOORDS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::TEXCOORD);

			auto amountTexCoords = 1;
#ifdef MULTI_TEXCOORD
			amountTexCoords = binReader->Read<USHORT>();
#endif
			pMesh->m_TexCoordCount = amountTexCoords;

			for (unsigned int i = 0; i < vertexCount * amountTexCoords; ++i)
			{
				DirectX::XMFLOAT2 tc;
				tc.x = binReader->Read<float>();
				tc.y = binReader->Read<float>();
				pMesh->m_TexCoords.push_back(tc);
			}
		}
		break;
		case MeshDataType::COLORS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::COLOR);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT4 color;
				color.x = binReader->Read<float>();
				color.y = binReader->Read<float>();
				color.z = binReader->Read<float>();
				color.w = binReader->Read<float>();
				pMesh->m_Colors.push_back(color);
			}
		}
		break;
		case MeshDataType::BLENDINDICES:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::BLENDINDICES);

			//TODO: Start parsing the BlendIndices for every vertex
			//and add them to the corresponding vector
			//pMesh->m_BlendIndices
			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT4 blendIndices;
				blendIndices.x = binReader->Read<float>();
				blendIndices.y = binReader->Read<float>();
				blendIndices.z = binReader->Read<float>();
				blendIndices.w = binReader->Read<float>();
				pMesh->m_BlendIndices.push_back(blendIndices);
			}
		}
		break;
		case MeshDataType::BLENDWEIGHTS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::BLENDWEIGHTS);

			//TODO: Start parsing the BlendWeights for every vertex
			//and add them to the corresponding vector
			//pMesh->m_BlendWeights
			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT4 blendWeights;
				blendWeights.x = binReader->Read<float>();
				blendWeights.y = binReader->Read<float>();
				blendWeights.z = binReader->Read<float>();
				blendWeights.w = binReader->Read<float>();
				pMesh->m_BlendWeights.push_back(blendWeights);
			}
		}
		break;
		case MeshDataType::ANIMATIONCLIPS:
		{
			pMesh->m_HasAnimations = true;

			//TODO: Start parsing the AnimationClips
			//1. Read the clipCount
			auto clipCount = binReader->Read<USHORT>();

			//2. For every clip
			for (int clipIndex = 0; clipIndex < clipCount; ++clipIndex)
			{
				//3. Create a AnimationClip object (clip)
				AnimationClip ac{};

				//4. Read/Assign the ClipName
				ac.Name = binReader->ReadString();

				//5. Read/Assign the ClipDuration
				ac.Duration = binReader->Read<float>();

				//6. Read/Assign the TicksPerSecond
				ac.TicksPerSecond = binReader->Read<float>();

				//7. Read the KeyCount for this clip
				auto keyCount = binReader->Read<USHORT>();

				//8. For every key
				for (int keyIndex = 0; keyIndex < keyCount; ++keyIndex)
				{
					//9. Create a AnimationKey object (key)
					AnimationKey ak{};

					//10. Read/Assign the Tick
					ak.Tick = binReader->Read<float>();

					//11. Read the TransformCount
					auto transformCount = binReader->Read<USHORT>();

					//12. For every transform
					for (int transformIndex = 0; transformIndex < transformCount; ++transformIndex)
					{
						//13. Create a XMFLOAT4X4
						DirectX::XMFLOAT4X4 transform{};

						//14. The following 16 floats are the matrix			values, they are stored by row
						// float0 = readFloat (ROW1) (_11)
						// float1 = readFloat (ROW1) (_12)
						// float2 = readFloat (ROW1)
						// float3 = readFloat (ROW1)
						// float4 = readFloat (ROW2) (_21)
						//...
						// float15 = readFloat (ROW4) (_44)
						//
						//MATRIX:
						// [ float0	float1	float2	float3 ]
						// [ float4	...		...		...	   ]
						// [ ...	...		...		...	   ]
						// [ ...	...		...		float15]
						
						transform._11 = binReader->Read<float>();
						transform._12 = binReader->Read<float>();
						transform._13 = binReader->Read<float>();
						transform._14 = binReader->Read<float>();

						transform._21 = binReader->Read<float>();
						transform._22 = binReader->Read<float>();
						transform._23 = binReader->Read<float>();
						transform._24 = binReader->Read<float>();

						transform._31 = binReader->Read<float>();
						transform._32 = binReader->Read<float>();
						transform._33 = binReader->Read<float>();
						transform._34 = binReader->Read<float>();

						transform._41 = binReader->Read<float>();
						transform._42 = binReader->Read<float>();
						transform._43 = binReader->Read<float>();
						transform._44 = binReader->Read<float>();

						//15. Add The matrix to the BoneTransform vector of the key
						ak.BoneTransforms.push_back(transform);
					}
					//16. Add the key to the key vector of the clip
					ac.Keys.push_back(ak);
				}
				//17. Add the clip to the AnimationClip vector of the MeshFilter (pMesh->m_AnimationClips)
				pMesh->m_AnimationClips.push_back(ac);
			}
		}
		break;
		case MeshDataType::SKELETON:
		{
			//TODO: Complete
			//1. Read/Assign the boneCount (pMesh->m_BoneCount)
			pMesh->m_BoneCount = binReader->Read<USHORT>();

 			//2. Move the buffer to the next block position (don't forget that we already moved the reader ;) )
			binReader->MoveBufferPosition(dataOffset - sizeof(USHORT));
		}
		break;
		default:
			binReader->MoveBufferPosition(dataOffset);
			break;
		}
	}

	delete binReader;

	return pMesh;
}

void MeshFilterLoader::Destroy(MeshFilter* objToDestroy)
{
	SafeDelete(objToDestroy);
}
