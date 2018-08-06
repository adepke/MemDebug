/*
	Copyright (c) 2018 Andrew Depke
*/
#pragma once

#include <map>

#ifdef MEMDEBUG
#include "../Source/AllocatorExternal.h"

namespace MemDebug
{
	/*
	* Total Memory Allocated in Bytes This Session.
	*/
	size_t GetTotalAllocated()
	{
		return Allocator::Get().TotalAllocated;
	}

	/*
	* Total Memory Released in Bytes This Session.
	*/
	size_t GetTotalDeallocated()
	{
		return Allocator::Get().TotalDeallocated;
	}

	/*
	* Total Memory Allocated and Not Yet Released in Bytes This Session.
	*/
	size_t GetCurrentlyAllocated()
	{
		return Allocator::Get().CurrentlyAllocated;
	}

	/*
	* Most Recent Allocation in Bytes.
	*/
	size_t GetLastAllocated()
	{
		return Allocator::Get().LastAllocated;
	}

	/*
	* Most Recent Release in Bytes.
	*/
	size_t GetLastDeallocated()
	{
		return Allocator::Get().LastDeallocated;
	}

	/*
	* Retrieve a List of All Currently Allocated Memory Blocks, Includes Debugging Information for Each Block.
	*/
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
	/*
	* Total Memory Allocated in Bytes This Session.
	*/
	size_t GetTotalAllocated()
	{
		return 0;
	}

	/*
	* Total Memory Released in Bytes This Session.
	*/
	size_t GetTotalDeallocated()
	{
		return 0;
	}

	/*
	* Total Memory Allocated and Not Yet Released in Bytes This Session.
	*/
	size_t GetCurrentlyAllocated()
	{
		return 0;
	}

	/*
	* Most Recent Allocation in Bytes.
	*/
	size_t GetLastAllocated()
	{
		return 0;
	}

	/*
	* Most Recent Release in Bytes.
	*/
	size_t GetLastDeallocated()
	{
		return 0;
	}

	/*
	* Retrieve a List of All Currently Allocated Memory Blocks, Includes Debugging Information for Each Block.
	*/
	std::map<void*, Block>* GetBlocks()
	{
		return nullptr;
	}
}  // namespace MemDebug

// Push a Heap Name Onto the Name Stack.
#define PUSH_HEAP_NAME(Ignored) (void)0
// Pop a Heap Name Off the Name Stack.
#define POP_HEAP_NAME() (void)0
// Empty the Heap Name Stack.
#define CLEAR_HEAP_NAMES() (void)0

#endif