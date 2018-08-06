/*
	Copyright (c) 2018 Andrew Depke
*/
#pragma once

#include <cstddef>
#include <new>
#include <stack>

#define MAX_HEAP_NAME_LENGTH 16

#define MEMDEBUG_NAMED_SIGNATURE 0xFCFD
#define MEMDEBUG_UNNAMED_SIGNATURE 0xFCFC

namespace MemDebug
{
	class Allocator
	{
	public:
		Allocator() {}

		static Allocator& Get()
		{
			static Allocator AllocatorReference;

			return AllocatorReference;
		}

		Allocator(const Allocator&) = delete;
		void operator=(const Allocator&) = delete;

	private:
		std::stack<const char*> NameStack;

	public:
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
			NameStack.pop();
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

		void RegisterAllocation(size_t Size) throw()
		{
			TotalAllocated += Size;
			CurrentlyAllocated += Size;
			LastAllocated = Size;
		}

		void RegisterAllocation(size_t Size, const char* Filename, int Line) throw()
		{
			TotalAllocated += Size;
			CurrentlyAllocated += Size;
			LastAllocated = Size;
		}

		void RegisterDeallocation(size_t Size) throw()
		{
			TotalDeallocated += Size;
			CurrentlyAllocated -= Size;
			LastDeallocated = Size;
		}

		void RegisterDeallocation(size_t Size, const char* Filename, int Line) throw()
		{
			TotalDeallocated += Size;
			CurrentlyAllocated -= Size;
			LastDeallocated = Size;
		}
	};

	enum FreeType
	{
		Native,
		Unnamed,
		Named
	};
}

void* MallocTracked(size_t Size)
{
	auto ActiveName = MemDebug::Allocator::Get().GetActiveName();

	if (ActiveName)
	{
		Size += (sizeof(unsigned short) + sizeof(size_t) + sizeof(char) * MAX_HEAP_NAME_LENGTH);  // Header: Signature + Block Size + Name
		void* Pointer = malloc(Size);
		auto* PointerSignature = (unsigned short*)Pointer;
		auto* PointerSize = (size_t*)(PointerSignature + 1);
		auto* PointerName = (char*)(PointerSize + 1);
		*PointerSignature = MEMDEBUG_NAMED_SIGNATURE;  // Append Signature.
		*PointerSize = Size;  // Store the Size in the Header After the Signature.
		memset(PointerName, 0, sizeof(char) * MAX_HEAP_NAME_LENGTH);  // Zero Out Name.
		memcpy(PointerName, ActiveName, strlen(ActiveName));  // Add Heap Region Name.

		return (void*)(PointerName + (sizeof(char) * MAX_HEAP_NAME_LENGTH));  // Return the Pointer After the Header.
	}

	else
	{
		Size += (sizeof(unsigned short) + sizeof(size_t));  // Header: Signature + Block Size
		void* Pointer = malloc(Size);
		auto* PointerSignature = (unsigned short*)Pointer;
		auto* PointerSize = (size_t*)(PointerSignature + 1);
		*PointerSignature = MEMDEBUG_UNNAMED_SIGNATURE;  // Append Signature.
		*PointerSize = Size;  // Store the Size in the Header After the Signature.

		return (void*)(PointerSize + 1);  // Return the Pointer After the Header.
	}
}

void FreeTracked(void* Pointer, MemDebug::FreeType FreeType = MemDebug::Native /* Prevent Calls to free() From Getting Offset and Deleting Garbage */)
{
	switch (FreeType)
	{
	case MemDebug::Native:
		free(Pointer);
		break;
	case MemDebug::Unnamed:
		free((void*)((unsigned short*)(((size_t*)Pointer) - 1) - 1));
		break;
	case MemDebug::Named:
		free((void*)((unsigned short*)((size_t*)(((char*)Pointer) - (sizeof(char) * MAX_HEAP_NAME_LENGTH)) - 1) - 1));
		break;
	}
}

#define malloc(Size) MallocTracked(Size)
#define free(Pointer) FreeTracked(Pointer)

void* operator new(size_t Size, const char* Filename, int Line) throw()
{
	MemDebug::Allocator::Get().RegisterAllocation(Size, Filename, Line);

	return MallocTracked(Size);
}

void operator delete(void* Pointer) throw()
{
	if (Pointer)
	{
		size_t Size = 0;
		MemDebug::FreeType FreeType = MemDebug::Native;

		auto Signature = *((unsigned short*)(((size_t*)Pointer) - 1) - 1);

		if (Signature == MEMDEBUG_UNNAMED_SIGNATURE)
		{
			FreeType = MemDebug::Unnamed;

			Size = *(((size_t*)Pointer) - 1);  // Read the Block Size from the Tracked Malloc Header.
			Size -= (sizeof(unsigned short) + sizeof(size_t));  // Don't Factor the Addition to the Header as Part of the Allocated Memory.
		}

		else if ((Signature = *((unsigned short*)((size_t*)(((char*)Pointer) - (sizeof(char) * MAX_HEAP_NAME_LENGTH)) - 1) - 1)) == MEMDEBUG_NAMED_SIGNATURE)
		{
			FreeType = MemDebug::Named;

			Size = *((size_t*)(((char*)Pointer) - MAX_HEAP_NAME_LENGTH) - 1);  // Read the Block Size from the Tracked Malloc Header.
			Size -= (sizeof(unsigned short) + sizeof(size_t) + sizeof(char) * MAX_HEAP_NAME_LENGTH);  // Don't Factor the Addition to the Header as Part of the Allocated Memory.
		}

		MemDebug::Allocator::Get().RegisterDeallocation(Size);

		FreeTracked(Pointer, FreeType);
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