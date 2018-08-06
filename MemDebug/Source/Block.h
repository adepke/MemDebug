/*
	Copyright (c) 2018 Andrew Depke
*/
#pragma once

#define MAX_HEAP_NAME_LENGTH 32

#ifdef MAX_PATH
#define MAX_FILENAME_LENGTH MAX_PATH
#else
#define MAX_FILENAME_LENGTH 260
#endif

namespace MemDebug
{
	struct Block
	{
		// Size, in Bytes of the Block.
		size_t Size;
		// Name of the Region in Which the Block was Allocated.
		char Name[MAX_HEAP_NAME_LENGTH];
		// Name of the File Where the Block was Allocated.
		char Filename[MAX_FILENAME_LENGTH];
		// Approximated Line Number of the File Where the Block was Allocated.
		int Line;
	};
}  // namespace MemDebug