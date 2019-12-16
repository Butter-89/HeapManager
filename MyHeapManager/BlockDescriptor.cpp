#include "BlockDescriptor.h"

BlockDescriptor* const BlockDescriptor::GetNextBlock()
{
	return pNextBlock;
}

BlockDescriptor::BlockDescriptor(BlockDescriptor* i_thisAddr)
{
	m_pThisBlock = i_thisAddr;	//the pointer to the address of this block
	m_blockType = freeBlock;
	m_pBlockStartAddr = nullptr;	//the start address of the allocated memory in heap
	m_pMemInUseStartAddr = m_pBlockStartAddr;
	pNextBlock = nullptr;		//pointer to the next block descriptor
	//std::cout << "Block Descriptor Constructor called\n";
	m_size = 0;
	m_usedSize = 0;
}

BlockDescriptor::BlockDescriptor(BlockDescriptor* i_thisAddr, size_t i_blockSize)
{
	m_pThisBlock = i_thisAddr;
	m_blockType = freeBlock;
	m_pBlockStartAddr = nullptr;
	m_pMemInUseStartAddr = m_pBlockStartAddr;
	pNextBlock = nullptr;
	//std::cout << "Block Descriptor Constructor called\n";
	m_size = i_blockSize;
	m_usedSize = 0;
}

BlockDescriptor::~BlockDescriptor()
{
	m_pBlockStartAddr = nullptr;
	m_pMemInUseStartAddr = m_pBlockStartAddr;
	pNextBlock = nullptr;
	std::cout << "Block Descriptor destructor called\n";
}

BlockType BlockDescriptor::GetBlockType() const
{
	return m_blockType;
}

void BlockDescriptor::SetNextBlock(BlockDescriptor* i_nextBlock)
{
	pNextBlock = i_nextBlock;
}

void BlockDescriptor::SetBlockSize(size_t i_size)
{
	m_size = i_size;
}
size_t BlockDescriptor::GetBlockSize()
{
	return m_size;
}
size_t BlockDescriptor::GetUsedSize()
{
	return m_usedSize;
}

void BlockDescriptor::SetStartMemAddr(void* i_startAddr)
{
	m_pBlockStartAddr = i_startAddr;
}
void* BlockDescriptor::GetStartMemAddr()
{
	return m_pBlockStartAddr;
}
void BlockDescriptor::SetUsedMemAddr(void* i_usedAddr)
{
	m_pMemInUseStartAddr = i_usedAddr;
}
void* BlockDescriptor::GetMemInUseAddr()
{
	return m_pMemInUseStartAddr;
}

void BlockDescriptor::ShowThisBlock()
{
	std::cout << "Free Memory Block - " << m_pBlockStartAddr << " Size: " << m_size << " bytes\n";
	//std::cout << "(Block Starts at: " << m_pBlockStartAddr << ", Block Size: " << m_size << " bytes\n";
	//std::cout << "\n";
}


void BlockDescriptor::CombineAbutBlocks(BlockDescriptor* pre_block)
{
	pre_block->SetBlockSize(pre_block->m_size + pre_block->GetNextBlock()->m_size);	//sum the size of abut blocks
	pre_block->SetNextBlock(pre_block->GetNextBlock()->GetNextBlock());		//set the next block of the newly combined block
	pre_block->GetNextBlock()->pNextBlock = nullptr;
	pre_block->GetNextBlock()->SetBlockSize(0);
}

void BlockDescriptor::ShowAllocatedMemory()
{
	std::cout << "Outstanding Memory block starts at: " << m_pMemInUseStartAddr << " with size: " << m_usedSize << " bytes\n";
		/*alinged: " <<
		reinterpret_cast<uintptr_t>(m_pMemInUseStartAddr) - reinterpret_cast<uintptr_t>(m_pBlockStartAddr) <<
		"(Whole Block Starts at: " << m_pBlockStartAddr << "\n";*/
	//std::cout << "(Whole Block Starts at: " << m_pBlockStartAddr << ", Block Size: " << m_size << " bytes, alinged: " <<
	//	reinterpret_cast<uintptr_t>(m_pMemInUseStartAddr) - reinterpret_cast<uintptr_t>(m_pBlockStartAddr) << "\n";
	//std::cout << "\n";
}

void BlockDescriptor::SetOutstandingBlock(size_t i_blockSize, void* i_startAddr, BlockDescriptor* i_nextOutstanding)
{
	m_blockType = outstandingBlock;
	m_size = i_blockSize;
	m_usedSize = m_size;
	m_pBlockStartAddr = i_startAddr;
	m_pMemInUseStartAddr = m_pBlockStartAddr;
	SetNextBlock(i_nextOutstanding);
}

void BlockDescriptor::SetOutstandingBlock(size_t i_blockSize, void* i_startAddr, void* i_alignedAddr, BlockDescriptor* i_nextOutstanding)
{
	m_blockType = outstandingBlock;
	m_size = i_blockSize + (reinterpret_cast<uintptr_t>(i_alignedAddr) - reinterpret_cast<uintptr_t>(i_startAddr));	//the total size including the alignment space
	m_usedSize = i_blockSize;	//the actual allocated space 
	m_pBlockStartAddr = i_startAddr;
	m_pMemInUseStartAddr = i_alignedAddr;
	SetNextBlock(i_nextOutstanding);
}

void BlockDescriptor::SetFreeBlock(size_t i_blockSize, void* i_startAddr, BlockDescriptor* i_nextFree)
{
	m_blockType = freeBlock;
	m_size = i_blockSize;
	m_pBlockStartAddr = i_startAddr;
	m_pMemInUseStartAddr = m_pBlockStartAddr;
	SetNextBlock(i_nextFree);
}

void BlockDescriptor::ResetBlock()
{
	m_blockType = freeBlock;
	m_pBlockStartAddr = nullptr;
	m_pMemInUseStartAddr = m_pBlockStartAddr;
	pNextBlock = nullptr;
	m_size = 0;
}