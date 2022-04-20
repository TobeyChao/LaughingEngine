//#include "../DefaultMultiTypeFixedChunk.h"
//#include <iostream>
//
//int main()
//{
//	using t = TypeList<int, float, bool>;
//	DefaultMultiTypeFixedChunk::Type ChunkAlloc;
//	ChunkAlloc.Init<t>();
//
//	ChunkHandle* addr1 = ChunkAlloc.Allocate();
//	int* ret11 = ChunkAlloc.Create<int>(addr1, 2);
//	float* ret12 = ChunkAlloc.Create<float>(addr1, 1.1f);
//	bool* ret13 = ChunkAlloc.Create<bool>(addr1, false);
//
//	ChunkHandle* addr2 = ChunkAlloc.Allocate();
//	int* ret21 = ChunkAlloc.Create<int>(addr2, 3);
//	float* ret22 = ChunkAlloc.Create<float>(addr2, 3.1f);
//	bool* ret23 = ChunkAlloc.Create<bool>(addr2, true);
//
//	ChunkHandle* addr3 = ChunkAlloc.Allocate();
//	int* ret31 = ChunkAlloc.Create<int>(addr3, 4);
//	float* ret32 = ChunkAlloc.Create<float>(addr3, 4.1f);
//	bool* ret33 = ChunkAlloc.Create<bool>(addr3, false);
//
//	// Free之后会调整内存排布，访问数据需要重新Get
//	ChunkAlloc.Free(addr2);
//	addr2 = nullptr;
//
//	ret31 = ChunkAlloc.Get<int>(addr3);
//	ret32 = ChunkAlloc.Get<float>(addr3);
//	ret33 = ChunkAlloc.Get<bool>(addr3);
//
//	ChunkAlloc.Free(addr1);
//	addr1 = nullptr;
//	ChunkAlloc.Free(addr3);
//	addr3 = nullptr;
//
//	return 0;
//}