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
		size_t Size;
		char Name[MAX_HEAP_NAME_LENGTH];
		char Filename[MAX_FILENAME_LENGTH];
		int Line;
	};
}  // namespace MemDebug