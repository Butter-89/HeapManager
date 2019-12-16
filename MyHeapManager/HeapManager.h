#pragma once
#include "BlockDescriptor.h"

class HeapManager
{
private:
	HeapManager* p_managerAddr;
	BlockDescriptor* p_currentDescriptor;
	BlockDescriptor* p_outstandingBlockStartAddr;
	BlockDescriptor* p_freeBlockStartAddr;
	void* p_firstUsableAddr;
	void* p_currentMemPtr; //ptr to the actual usable memory space (for actual allocation by block descriptor)

	unsigned int numTotalDescriptors;
	unsigned int numOutstandingBlocks;
	unsigned int numFreeBlocks;
	size_t totalMemorySize;	//the total memory size of the heap (both descriptors and memory blocks)
	size_t usableMemorySize;

public:
	HeapManager(unsigned int i_numDescriptors, void* i_pMemory, size_t i_sizeMemory);
	~HeapManager();
	void InitFreeBlocks();
	void ShowFreeBlocks() const;
	void ShowOutstandingBlocks() const;
	void* alloc(size_t i_size, unsigned int i_alignment);
	void* alloc(size_t i_size);
	void* AllocateWithCurrentBlock(BlockDescriptor* pCurrent, size_t i_size, unsigned int i_alignment);
	void* AllocateWithCurrentBlock(BlockDescriptor* pCurrent, size_t i_size);
	bool Free(void* i_ptr);
	bool InsertFreeLinkNode(void* i_ptr);
	//void* Allocate(size_t i_size);
	void* RoundUp(void* i_addr, unsigned int i_align);
	BlockDescriptor* GetFirstFreeDescriptor();
	BlockDescriptor* FindDescriptor(void* i_ptr);
	BlockDescriptor* PickOutDescriptor(void* i_ptr);
	//size_t AlignmentOffset(void* i_addr, unsigned int i_align);
	void Collect();
	BlockDescriptor* Contains(void* i_ptr) const;
	BlockDescriptor* IsAllocated(void* i_ptr) const;
	void SortLinkedList();
	void SwapDescriptor(BlockDescriptor* i_blockA, BlockDescriptor* i_blockB);
	BlockDescriptor* GetFreeBlock();
	bool CheckFreeBlock();	//check if there is available free 0 size block
	bool SetRemaining(BlockDescriptor* i_block, void* i_startMemAddr, size_t i_size);
	void MergeBlocks(BlockDescriptor* i_currentBlock, BlockDescriptor* i_nextBlock);
};