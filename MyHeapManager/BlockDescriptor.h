#pragma once
#include <Windows.h>
#include <assert.h>
#include <iostream>

enum BlockType
{
	freeBlock = 0, outstandingBlock = 1
};

class BlockDescriptor
{
private:
	BlockDescriptor*	m_pThisBlock;
	BlockDescriptor*	pNextBlock;
	void*				m_pBlockStartAddr;
	void*				m_pMemInUseStartAddr;
	size_t				m_size;
	size_t				m_usedSize;
	BlockType			m_blockType;
public:
	//constructors & destructor
	BlockDescriptor(BlockDescriptor* i_thisAddr);
	BlockDescriptor(BlockDescriptor* i_thisAddr, size_t i_blockSize);
	~BlockDescriptor();
	//Linked list operation
	BlockDescriptor* const GetNextBlock();
	void SetNextBlock(BlockDescriptor* i_nextBlock);
	//general
	BlockType GetBlockType() const;
	void ShowThisBlock();
	void ShowAllocatedMemory();
	void SetBlockSize(size_t i_size);
	size_t GetBlockSize();
	size_t GetUsedSize();
	void SetStartMemAddr(void* i_startAddr);
	void* GetStartMemAddr();
	void SetUsedMemAddr(void* i_usedAddr);
	void* GetMemInUseAddr();
	void CombineAbutBlocks(BlockDescriptor* pre_block);
	//memory allocation
	void SetOutstandingBlock(size_t i_blockSize, void* i_startAddr, BlockDescriptor* i_nextOutstanding);
	void SetOutstandingBlock(size_t i_blockSize, void* i_startAddr, void* i_alignedAddr, BlockDescriptor* i_nextOutstanding);
	void SetFreeBlock(size_t i_blockSize, void* i_startAddr, BlockDescriptor* i_nextFree);
	void ResetBlock();
};