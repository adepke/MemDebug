/*
	Copyright (c) 2018 Andrew Depke
*/
#pragma once

#include <cstddef>
#include <new>
#include <stack>
#include <map>

#ifdef MEMDEBUG_THROW_ON_DELETE_NULLPTR
#include <exception>
#endif

#include "Block.h"

namespace MemDebug
{
	class Allocator
	{
#ifdef MEMDEBUG_THROW_ON_DELETE_NULLPTR
	protected:
		static void UnexpectedHandler()
		{
			// Rethrow Exception, Don't Abort.
			throw;
		}
#endif

	public:
		Allocator()
		{
#ifdef MEMDEBUG_THROW_ON_DELETE_NULLPTR
			set_unexpected(UnexpectedHandler);
#endif
		}
		~Allocator()
		{
			Destructing = true;
		}

		static Allocator& Get()
		{
			static Allocator AllocatorReference;

			return AllocatorReference;
		}

		Allocator(const Allocator&) = delete;
		void operator=(const Allocator&) = delete;

	private:
		bool Destructing = false;

	private:
		std::stack<const char*> NameStack;

	public:
		std::map<void*, Block> BlockMap;

		size_t TotalAllocated = 0;
		size_t TotalDeallocated = 0;
		size_t CurrentlyAllocated = 0;
		size_t LastAllocated = 0;
		size_t LastDeallocated = 0;

		void PushName(const char* Name)
		{
			if (std::strlen(Name) < MAX_HEAP_NAME_LENGTH)
			{
				NameStack.push(Name);
			}
		}

		void PopName()
		{
			if (!NameStack.empty())
			{
				NameStack.pop();
			}
		}

		const char* GetActiveName()
		{
			if (!NameStack.empty())
			{
				return NameStack.top();
			}

			else
			{
				return nullptr;
			}
		}

		void ClearNames()
		{
			while (!NameStack.empty())
			{
				NameStack.pop();
			}
		}

		void RegisterAllocation(void* BaseAddress, size_t Size, const char* Filename, int Line) throw()
		{
			TotalAllocated += Size;
			CurrentlyAllocated += Size;
			LastAllocated = Size;

			Block NewAllocation;
			NewAllocation.Size = Size;
			memset(NewAllocation.Name, 0, MAX_HEAP_NAME_LENGTH);

			auto* ActiveName = GetActiveName();
			if (ActiveName)
			{
				memcpy(NewAllocation.Name, ActiveName, strlen(ActiveName));
			}
			
			memset(NewAllocation.Filename, 0, MAX_FILENAME_LENGTH);
			if (Filename && strlen(Filename) <= MAX_FILENAME_LENGTH)
			{
				memcpy(NewAllocation.Filename, Filename, strlen(Filename));
			}

			NewAllocation.Line = Line;

			BlockMap[BaseAddress] = NewAllocation;
		}

		void RegisterDeallocation(void* BaseAddress) throw()
		{
			if (!Destructing)
			{
				auto BlockIter = BlockMap.find(BaseAddress);
				if (BlockIter != BlockMap.end())
				{
					auto Size = BlockIter->second.Size;

					TotalDeallocated += Size;
					CurrentlyAllocated -= Size;
					LastDeallocated = Size;

					BlockMap.erase(BlockIter);
				}
			}
		}
	};
}  // namespace MemDebug

void* operator new(size_t Size, const char* Filename, int Line) throw()
{
	void* Pointer = malloc(Size);

	if (Pointer)
	{
		MemDebug::Allocator::Get().RegisterAllocation(Pointer, Size, Filename, Line);
	}

	return Pointer;
}

void operator delete(void* Pointer) throw()
{
	if (Pointer)
	{
		free(Pointer);

		MemDebug::Allocator::Get().RegisterDeallocation(Pointer);
	}

	else
	{
#ifdef MEMDEBUG_THROW_ON_DELETE_NULLPTR
#pragma warning(suppress: 4297)
		throw std::runtime_error("Attempted to Delete Nullptr");
#endif
	}
}

#define new new(__FILE__, __LINE__)