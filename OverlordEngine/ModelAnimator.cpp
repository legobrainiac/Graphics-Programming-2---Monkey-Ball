#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(std::vector<DirectX::XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_TickCount(0),
m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	UNREFERENCED_PARAMETER(clipNumber);
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;

	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (!(clipNumber < GetClipCount()))
	{
		Reset();
		Logger::LogWarning(L"Clip number out of bounds");
		return;
	}
	else
	{
		const auto clip = m_pMeshFilter->m_AnimationClips[clipNumber];
		SetAnimation(clip);
	}
}

void ModelAnimator::SetAnimation(std::wstring clipName)
{
	UNREFERENCED_PARAMETER(clipName);
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;

	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	for (const auto& it : m_pMeshFilter->m_AnimationClips)
	{
		if (it.Name == clipName)
		{
			SetAnimation(it);
			return;
		}
	}

	Reset();
	Logger::LogWarning(L"Clip with name" + clipName + L" not found...");
}

void ModelAnimator::SetAnimation(AnimationClip clip)
{
	UNREFERENCED_PARAMETER(clip);
	m_ClipSet = true;
	m_CurrentClip = clip;

	Reset(false);

	//TODO: complete
	//Set m_ClipSet to true
	//Set m_CurrentClip

	//Call Reset(false)
}

void ModelAnimator::Reset(bool pause)
{
	UNREFERENCED_PARAMETER(pause);
	//TODO: complete
	//If pause is true, set m_IsPlaying to false
	m_IsPlaying = (pause) ? false : m_IsPlaying;

	//Set m_TickCount to zero
	m_TickCount = 0;

	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.f;

	if (m_ClipSet)
	{
		const auto transforms = m_CurrentClip.Keys[0].BoneTransforms;
		m_Transforms.assign(transforms.cbegin(), transforms.cend()); // = { it.cbegin(), it.cend() }
	}
	else
	{
		DirectX::XMFLOAT4X4 identity{};
		DirectX::XMStoreFloat4x4(&identity, DirectX::XMMatrixIdentity());
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identity);
	}
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		const auto dt = gameContext.pGameTime->GetElapsed();

		//1. 
		//Calculate the passedTicks (see the lab document)
		//auto passedTicks = ...
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		const auto passedTicks = fmod(dt * m_CurrentClip.TicksPerSecond * m_AnimationSpeed, m_CurrentClip.Duration);
		
		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount

		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0.f)
				m_TickCount += m_CurrentClip.Duration;
		}
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.Duration)
				m_TickCount -= m_CurrentClip.Duration;
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount

		keyA = m_CurrentClip.Keys[0];
		keyB = m_CurrentClip.Keys[m_CurrentClip.Keys.size() - 1];

		for (const auto& key : m_CurrentClip.Keys)
		{
			if (key.Tick < m_TickCount && key.Tick > keyA.Tick)
				keyA = key;
			if (key.Tick > m_TickCount && key.Tick < keyB.Tick)
				keyB = key;

			// Could do this with +1 and overflow the index to the start, 
			//  I'll keep it like this though
		}

		// Calculate blend factor
		float interval = keyB.Tick - keyA.Tick;
		float fraction = m_TickCount - keyA.Tick; // Only interested in the fraction part
		float blendFactor = fraction / interval;

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		//Clear the m_Transforms vector
		
		m_Transforms.clear();

		for (UINT transformIndex = 0;
			transformIndex < keyA.BoneTransforms.size();
			++transformIndex)
		{
			using namespace DirectX;

			auto transformA = keyA.BoneTransforms[transformIndex];
			auto transformB = keyB.BoneTransforms[transformIndex];

			// Decompose transform A
			XMVECTOR positionA;
			XMVECTOR rotationA;
			XMVECTOR scaleA;
			XMMatrixDecompose(&scaleA, &rotationA, &positionA, XMLoadFloat4x4(&transformA));

			// Decompose transform B
			XMVECTOR positionB;
			XMVECTOR rotationB;
			XMVECTOR scaleB;
			XMMatrixDecompose(&scaleB, &rotationB, &positionB, XMLoadFloat4x4(&transformB));

			// Interpolated transform
			auto positionI = XMVectorLerp(positionA, positionB, blendFactor);
			auto rotationI = XMQuaternionSlerp(rotationA, rotationB, blendFactor);
			auto scaleI = XMVectorLerp(scaleA, scaleB, blendFactor);

			// T*R*S
			auto tMat = XMMatrixTranslationFromVector(positionI);
			auto rMat = XMMatrixRotationQuaternion(rotationI);
			auto sMat = XMMatrixScalingFromVector(scaleI);

			XMFLOAT4X4 finalTransform{};
			XMStoreFloat4x4(&finalTransform, sMat * rMat * tMat);

			m_Transforms.push_back(finalTransform);
		}

		//FOR every boneTransform in a key (So for every bone)
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector
	}
}
