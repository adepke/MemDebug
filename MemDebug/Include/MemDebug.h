#pragma once

#include <map>

#ifdef MEMDEBUG
#include "../Source/AllocatorExternal.h"

namespace MemDebug
{
	size_t GetTotalAllocated()
	{
		return Allocator::Get().TotalAllocated;
	}

	size_t GetTotalDeallocated()
	{
		return Allocator::Get().TotalDeallocated;
	}

	size_t GetCurrentlyAllocated()
	{
		return Allocator::Get().CurrentlyAllocated;
	}

	size_t GetLastAllocated()
	{
		return Allocator::Get().LastAllocated;
	}

	size_t GetLastDeallocated()
	{
		return Allocator::Get().LastDeallocated;
	}

	std::map<void*, Block>* GetBlocks()
	{
		return &Allocator::Get().BlockMap;
	}
}  // namespace MemDebug

// Push a Heap Name Onto the Name Stack.
#define PUSH_HEAP_NAME(Name) MemDebug::Allocator::Get().PushName(Name)
// Pop a Heap Name Off the Name Stack.
#define POP_HEAP_NAME() MemDebug::Allocator::Get().PopName()
// Empty the Heap Name Stack.
#define CLEAR_HEAP_NAMES() MemDebug::Allocator::Get().ClearNames()

#else

#include "../Source/Block.h"

namespace MemDebug
{
	size_t GetTotalAllocated()
	{
		return 0;
	}

	size_t GetTotalDeallocated()
	{
		return 0;
	}

	size_t GetCurrentlyAllocated()
	{
		return 0;
	}

	size_t GetLastAllocated()
	{
		return 0;
	}

	size_t GetLastDeallocated()
	{
		return 0;
	}

	std::map<void*, Block>* GetBlocks()
	{
		return nullptr;
	}
}  // namespace MemDebug

#define PUSH_HEAP_NAME(Ignored) void(0)
#define POP_HEAP_NAME() void(0)
#define CLEAR_HEAP_NAMES() void(0)

#endif