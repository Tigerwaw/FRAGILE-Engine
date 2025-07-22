#pragma once
#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <memory>

#include "WinPixEventRuntime/pix3.h"
#include "GraphicsCommandBase.h"
#include "LambdaGraphicsCommand.h"
#include "Render/RenderMesh.h"
#include "Render/RenderAnimatedMesh.h"
#include "Render/RenderInstancedMesh.h"
#include "Render/RenderMeshShadow.h"
#include "Render/RenderAnimatedMeshShadow.h"
#include "Render/RenderInstancedMeshShadow.h"
#include "Render/RenderMeshDebugPass.h"
#include "Render/RenderAnimatedMeshDebugPass.h"
#include "Render/RenderInstancedMeshDebugPass.h"
#include "Render/RenderText.h"
#include "Render/RenderSprite.h"
#include "Render/RenderSpritesheet.h"
#include "Render/RenderDebugLines.h"
#include "Render/RenderFullscreenQuad.h"
#include "Render/RenderParticles.h"
#include "Render/RenderTrail.h"
#include "Render/RenderSkybox.h"
#include "Resource/SetTextureResource.h"
#include "Resource/ClearTextureResource.h"
#include "Resource/SetGBufferAsRenderTarget.h"
#include "Resource/SetGBufferAsResource.h"
#include "Resource/ChangePipelineState.h"
#include "Resource/SetDefaultRenderTarget.h"
#include "Resource/SetRenderTarget.h"
#include "ConstantBuffer/UpdateFrameBuffer.h"
#include "ConstantBuffer/UpdateLightBuffer.h"
#include "ConstantBuffer/UpdateShadowBuffer.h"
#include "ConstantBuffer/UpdatePostProcessBuffer.h"

#include "Debug/BeginEvent.h"
#include "Debug/EndEvent.h"
#include "Debug/SetMarker.h"

class Mesh;

class GraphicsCommandList final
{
public:
	GraphicsCommandList(size_t aMemorySize = 16777216);
	~GraphicsCommandList();

	GraphicsCommandList(const GraphicsCommandList&) = delete;
	GraphicsCommandList(GraphicsCommandList&&) noexcept = delete;
	GraphicsCommandList& operator=(const GraphicsCommandList&) noexcept = delete;
	GraphicsCommandList& operator=(GraphicsCommandList&&) noexcept = delete;

	void Execute();
	void Reset();

	__forceinline bool IsFinished() const { return !isExecuting && isFinished; }
	__forceinline bool IsExecuting() const { return isExecuting; }
	__forceinline bool HasCommands() const { return myNumCommands != 0; }

	template<typename CommandClass, typename ...Args>
	std::enable_if_t<std::is_base_of_v<GraphicsCommandBase, CommandClass>>
	Enqueue(Args... args)
	{
		PIXScopedEvent(PIX_COLOR_INDEX(1), "GraphicsEngine Queue Command");
		const size_t commandSize = sizeof(CommandClass);
		if (myCursor + commandSize > mySize)
		{
			throw std::out_of_range("Graphics Command List ran out of memory! Consider increasing its size!");
		}

		GraphicsCommandBase* ptr = reinterpret_cast<GraphicsCommandBase*>(myData + myCursor);
		myCursor += commandSize;
		::new(ptr) CommandClass(std::forward<Args>(args)...);
		*myLink = ptr;
		myLink = &ptr->Next;
		++myNumCommands;
	}

	void Enqueue(GraphicsCommandFunction&& aLambda)
	{
		if (aLambda)
		{
			Enqueue<LambdaGraphicsCommand>(std::move(aLambda));
		}
	}

private:
	friend class GraphicsCommandListIterator;

	size_t mySize = 0;
	size_t myCursor = 0;
	size_t myNumCommands = 0;

	uint8_t* myData = nullptr;
	GraphicsCommandBase* myRoot = nullptr;
	GraphicsCommandBase** myLink = nullptr;

	bool isExecuting = false;
	bool isFinished = false;
};

class GraphicsCommandListIterator
{
public:
	GraphicsCommandListIterator(const GraphicsCommandList* aCommandList);

	GraphicsCommandBase* Next();

	__forceinline operator bool() const
	{
		return !!myPtr;
	}
private:
	GraphicsCommandBase* myPtr = nullptr;
};

