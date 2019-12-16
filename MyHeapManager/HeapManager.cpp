#include "HeapManager.h"
#include <iostream>

//#define DEBUG

HeapManager::HeapManager(unsigned int i_numDescriptors, void* i_pMemory, size_t i_sizeMemory)
{
	p_managerAddr = (HeapManager*)i_pMemory;
	p_currentDescriptor = nullptr;
	p_freeBlockStartAddr = nullptr;
	p_outstandingBlockStartAddr = nullptr;
	numTotalDescriptors = i_numDescriptors;
	numFreeBlocks = numTotalDescriptors;
	numOutstandingBlocks = 0;
	totalMemorySize = i_sizeMemory;
	usableMemorySize = 0;
	p_firstUsableAddr = p_managerAddr + (sizeof(HeapManager) + sizeof(BlockDescriptor) * numTotalDescriptors); //???
	p_currentMemPtr = p_firstUsableAddr;
	std::cout << "Heap Manager init...\n";
	InitFreeBlocks();
}

HeapManager::~HeapManager()
{
	std::cout << "Manager destroyed\n";
}

void HeapManager::InitFreeBlocks()
{
	//std::cout << sizeof(*p_managerAddr) << "\n";
	//std::cout << p_managerAddr << "\n";
	p_currentDescriptor = (BlockDescriptor*)(p_managerAddr + 1);
	//std::cout << p_currentDescriptor << "\n";
	p_freeBlockStartAddr = p_currentDescriptor;
	//std::cout << p_managerAddr +  totalSize -1<< "\n";
	//std::cout << p_managerAddr + totalSize - (HeapManager*)p_freeBlockStartAddr << "\n";
	//std::cout << sizeof(BlockDescriptor) << "\n";
	for (unsigned int i = 0; i < numTotalDescriptors; i++)
	{
		//const size_t maxTestAllocationSize = 1024;
		//size_t	sizeAlloc = 1 + (rand() & (maxTestAllocationSize - 1));		//should not randomly allocate here!

		BlockDescriptor* descriptor = new (p_currentDescriptor) BlockDescriptor(p_currentDescriptor);
		p_currentDescriptor++;
		descriptor->SetNextBlock(p_currentDescriptor);
	}
	//std::cout << p_firstUsableAddr << "\n";

	//p_firstUsableAddr = (void*)(p_currentDescriptor++);
	//std::cout << p_firstUsableAddr << "\n";
	usableMemorySize = totalMemorySize - sizeof(HeapManager) - sizeof(BlockDescriptor) * numTotalDescriptors;
	//Init the first block
	p_freeBlockStartAddr->SetBlockSize(usableMemorySize);
	p_freeBlockStartAddr->SetStartMemAddr(p_firstUsableAddr);
	p_freeBlockStartAddr->SetUsedMemAddr(p_firstUsableAddr);
	//std::cout << "First usable at: " << p_firstUsableAddr << "\n";
	//p_freeBlockStartAddr->ShowAllocatedMemory();
}

void HeapManager::ShowFreeBlocks() const
{
	//std::cout << "Total memory size: " << totalMemorySize << " bytes\n";
	//std::cout << "Manager & descriptor memory size: " << sizeof(HeapManager) + sizeof(BlockDescriptor) * numTotalDescriptors << " bytes\n";
	std::cout << "Free Memory blocks: \n";

	BlockDescriptor* pCurrent = p_freeBlockStartAddr;
	if (p_freeBlockStartAddr != nullptr)
	{
		do
		{
			if (pCurrent->GetBlockSize() != 0)
			{
				pCurrent->ShowThisBlock();
			}
			pCurrent = pCurrent->GetNextBlock();
		} while ((pCurrent->GetNextBlock() != nullptr));
	}
}

void HeapManager::ShowOutstandingBlocks() const
{
	//std::cout << "Total memory size: " << totalMemorySize << " bytes\n";
	//std::cout << "Manager & descriptor memory size: " << sizeof(HeapManager) + sizeof(BlockDescriptor) * numTotalDescriptors << " bytes\n";
	std::cout << "Outstanding Memory blocks (allocated): \n";
	BlockDescriptor* pCurrent = p_outstandingBlockStartAddr;
	if (p_outstandingBlockStartAddr != nullptr)
	{
		do
		{
			pCurrent->ShowAllocatedMemory();
			pCurrent = pCurrent->GetNextBlock();
		} while ((pCurrent->GetNextBlock() != nullptr));
	}

}

void* HeapManager::alloc(size_t i_size, unsigned int i_align)
{
	//std::cout << "\n";
	//std::cout << "Allocate mem with size: " << i_size << " and aligned in: " << i_align << "\n";
	assert(i_size);

	BlockDescriptor* pCurrent = p_freeBlockStartAddr;
	int i = 0;
	while (pCurrent)
	{
		void* pUserMem = AllocateWithCurrentBlock(pCurrent, i_size, i_align);
		if (pUserMem)
			return pUserMem;

		pCurrent = pCurrent->GetNextBlock();
		//std::cout << "Looking for the next free block\n";
		i++;
		//std::cout << i << '\n';
	}

	return nullptr;
}

void* HeapManager::alloc(size_t i_size)
{
	//std::cout << "\n";
	//std::cout << "Allocate mem with size: " << i_size << "\n";
	assert(i_size);

	BlockDescriptor* pCurrent = p_freeBlockStartAddr;
	int i = 0;
	while (pCurrent)
	{
		void* pUserMem = AllocateWithCurrentBlock(pCurrent, i_size);
		if (pUserMem)
			return pUserMem;

		pCurrent = pCurrent->GetNextBlock();
		//std::cout << "Looking for the next free block\n";
		i++;
		//std::cout << i << '\n';
	}

	return nullptr;
}

inline void* HeapManager::RoundUp(void* i_addr, unsigned int i_align)
{
	uintptr_t align = static_cast<uintptr_t>(i_align);
	//uintptr_t align_1 = static_cast<uintptr_t>(i_align - 1);
	//uintptr_t not_align_1 = static_cast<uintptr_t>(~(i_align - 1));
	void* aligned_addr = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(i_addr) + (align - 1)) & static_cast<uintptr_t>(~(align - 1)));
	return aligned_addr;
}

BlockDescriptor* HeapManager::GetFirstFreeDescriptor()
{
	return p_freeBlockStartAddr;
}

void* HeapManager::AllocateWithCurrentBlock(BlockDescriptor* pCurrent, size_t i_size, unsigned int i_alignment)
{
	void* pAllocatedMem = nullptr;
	if (pCurrent->GetBlockSize() >= i_size)
	{
		void* start_addr = pCurrent->GetStartMemAddr();
		void* aligned_addr = nullptr;
		aligned_addr = RoundUp(start_addr, i_alignment);
		size_t fullBlockSize = pCurrent->GetBlockSize();
#ifdef DEBUG

		std::cout << "Size of this free block: " << fullBlockSize << "\n";
		std::cout << "Mem block Start addr: " << start_addr << "\n";
		std::cout << "Aligned addr: " << aligned_addr << "\n";
		int i = 0;
		BlockDescriptor* pFreeBlocks = nullptr;
		pFreeBlocks = p_freeBlockStartAddr;
		while (pFreeBlocks->GetNextBlock()->GetBlockSize() != 0)
		{
			i++;
			pFreeBlocks = pFreeBlocks->GetNextBlock();
		}
		std::cout << "Last active block size: " << pFreeBlocks->GetBlockSize() << ", with block number" << i << "\n";
		//std::cout << "Size with alignment: " << reinterpret_cast<uintptr_t>(start_addr) + fullBlockSize - 1 - reinterpret_cast<uintptr_t>(aligned_addr) << "\n";

#endif // DEBUG
		if (reinterpret_cast<uintptr_t>(start_addr) + fullBlockSize - 1 - reinterpret_cast<uintptr_t>(aligned_addr) >= i_size
			&& reinterpret_cast<uintptr_t>(start_addr) + fullBlockSize - 1 >= reinterpret_cast<uintptr_t>(aligned_addr))			//
		{
			//the remaining size of the block can contain the size of memory needed
			//Get the remaining free size
			size_t remainingMemSize = pCurrent->GetBlockSize() - i_size - (reinterpret_cast<uintptr_t>(aligned_addr) - reinterpret_cast<uintptr_t>(start_addr));//???
			size_t currentTotalSize = i_size + (reinterpret_cast<uintptr_t>(aligned_addr) - reinterpret_cast<uintptr_t>(start_addr));
			//Trim current block to make it into an outstanding block
			pCurrent->SetBlockSize(pCurrent->GetBlockSize() - remainingMemSize);
			//Store the next next block as free block
			BlockDescriptor* nextFreeBlock = pCurrent->GetNextBlock();

			//Set up a new free block behind and connect them
			//std::cout << "Tring to allocate..\n";
			if (CheckFreeBlock())
			{
				assert(CheckFreeBlock());
				void* nextFreeStartAddr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(aligned_addr) + i_size);
				SetRemaining(pCurrent, nextFreeStartAddr, remainingMemSize);
				//Set current block as outstanding
				if (p_outstandingBlockStartAddr == nullptr)		//if it's the first outstanding block 
				{
					pCurrent->SetOutstandingBlock(i_size, start_addr, aligned_addr, nullptr);
					p_outstandingBlockStartAddr = pCurrent;
				}
				else	//else, set the current one as the first outstanding block in the list
				{
					pCurrent->SetOutstandingBlock(i_size, start_addr, aligned_addr, p_outstandingBlockStartAddr);
					p_outstandingBlockStartAddr = pCurrent;
				}
				pAllocatedMem = pCurrent->GetMemInUseAddr();
				pCurrent->ShowAllocatedMemory();

				//return the allocated memory

				pCurrent = nullptr;
				return pAllocatedMem;
			}
			else
			{
				return nullptr;
			}
		}
	}
	else
	{
		//std::cout << "Allocation failed\n";
		return nullptr;
	}
	return nullptr;
}

void* HeapManager::AllocateWithCurrentBlock(BlockDescriptor* pCurrent, size_t i_size)
{
	void* pAllocatedMem = nullptr;
	if (pCurrent->GetBlockSize() >= i_size)
	{
		void* start_addr = pCurrent->GetStartMemAddr();
		size_t fullBlockSize = pCurrent->GetBlockSize();
#ifdef DEBUG

		std::cout << "Size of this free block: " << fullBlockSize << "\n";
		std::cout << "Mem block Start addr: " << start_addr << "\n";
		std::cout << "Aligned addr: " << aligned_addr << "\n";
		int i = 0;
		BlockDescriptor* pFreeBlocks = nullptr;
		pFreeBlocks = p_freeBlockStartAddr;
		while (pFreeBlocks->GetNextBlock()->GetBlockSize() != 0)
		{
			i++;
			pFreeBlocks = pFreeBlocks->GetNextBlock();
		}
		std::cout << "Last active block size: " << pFreeBlocks->GetBlockSize() << ", with block number" << i << "\n";
		//std::cout << "Size with alignment: " << reinterpret_cast<uintptr_t>(start_addr) + fullBlockSize - 1 - reinterpret_cast<uintptr_t>(aligned_addr) << "\n";

#endif // DEBUG
		if (fullBlockSize >= i_size)			//
		{
			//std::cout << "Available aligned size: " << reinterpret_cast<uintptr_t>(start_addr) + fullBlockSize - 1 - reinterpret_cast<uintptr_t>(aligned_addr) << "\n";
			//the size of the block can contain the size of memory needed
			//Get the remaining free size
			size_t remainingMemSize = pCurrent->GetBlockSize() - i_size;//
			//Trim current block to make it into an outstanding block
			pCurrent->SetBlockSize(pCurrent->GetBlockSize() - remainingMemSize);
			//std::cout << "Current block size after trimming: " << pCurrent->GetBlockSize() << "\n";
			//Store the next next block as free block
			BlockDescriptor* nextFreeBlock = pCurrent->GetNextBlock();
			//Set up a new free block behind and connect them
			//std::cout << "Tring to allocate without alignment...\n";
			//see if there is enough available free block at the end
			//assert(CheckFreeBlock());
			if (CheckFreeBlock())
			{
				void* nextFreeStartAddr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(start_addr) + i_size);
				SetRemaining(pCurrent, nextFreeStartAddr, remainingMemSize);

				//Set current block as outstanding
				if (p_outstandingBlockStartAddr == nullptr)		//if it's the first outstanding block 
				{
					p_outstandingBlockStartAddr = pCurrent;
					pCurrent->SetOutstandingBlock(i_size, start_addr, nullptr);
				}
				else	//else, set the current one as the first outstanding block in the list
				{
					pCurrent->SetOutstandingBlock(i_size, start_addr, p_outstandingBlockStartAddr);
					p_outstandingBlockStartAddr = pCurrent;
				}

				pAllocatedMem = pCurrent->GetMemInUseAddr();
				/*std::cout << "Allocated mem addr: " << pAllocatedMem << " with size "
					<< pCurrent->GetBlockSize() << " Allocation success.\n";*/
				pCurrent->ShowAllocatedMemory();


				//Connect to the first free block


				//pCurrent->SetNextBlock(nextFreeBlock);
				//pCurrent->GetNextBlock()->SetBlockSize(remainingMemSize);
				//pCurrent->GetNextBlock()->SetStartMemAddr(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(start_addr) + i_size));
				//p_freeBlockStartAddr = pCurrent->GetNextBlock();
				//std::cout << "Next free block size: " << pCurrent->GetNextBlock()->GetBlockSize() << "\n";
				//return the allocated memory

				pCurrent = nullptr;
				return pAllocatedMem;
			}
			else
				return nullptr;
		}
	}
	else
	{
		//std::cout << "Allocation failed\n";
		return nullptr;
	}
	return nullptr;
}

bool HeapManager::Free(void* i_ptr)
{
	bool success = InsertFreeLinkNode(i_ptr);
	//std::cout << "Freeing...\n";
	if (success)
	{
		//std::cout << "Memory at " << i_ptr << " has been freed.\n";
		return true;
	}
	else
	{
		std::cout << "Memory at " << i_ptr << " is not contained in the heap.\n";
		return false;
	}
}

void HeapManager::Collect()
{
	//std::cout << "Collecting...\n";
	SortLinkedList();
	BlockDescriptor* pCurrent = p_freeBlockStartAddr;
	BlockDescriptor* pEndFreeBlock = p_freeBlockStartAddr;
	//Get the free block at the end of the list
	while (pEndFreeBlock->GetNextBlock() != nullptr)
	{
		pEndFreeBlock = pEndFreeBlock->GetNextBlock();
		assert(pEndFreeBlock);
	}
	//std::cout << "Currently the last block descriptor in the list: " << pEndFreeBlock << "\n";
	while (pCurrent->GetNextBlock() != nullptr && pCurrent->GetNextBlock()->GetBlockSize() != 0)
	{


		void* start_addr = pCurrent->GetStartMemAddr();
		size_t size = pCurrent->GetBlockSize();
		if (reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(start_addr) + size) == pCurrent->GetNextBlock()->GetStartMemAddr())
		{
			//std::cout << "Collecting\n"; //<< pCurrent->GetStartMemAddr() << "\n";
			BlockDescriptor* combinedBlock = pCurrent->GetNextBlock();
			assert(combinedBlock);
			pCurrent->SetFreeBlock(size + combinedBlock->GetBlockSize(), start_addr, combinedBlock->GetNextBlock());
			pEndFreeBlock->SetNextBlock(combinedBlock);
			combinedBlock->ResetBlock();
			pEndFreeBlock = combinedBlock;

			//std::cout << "Currently the last block descriptor: " << pEndFreeBlock << "\n";
		}
		else
		{
			if (pCurrent->GetNextBlock() != nullptr)
			{
				pCurrent = pCurrent->GetNextBlock();
			}
		}
		assert(pCurrent);
	}

	//ShowFreeBlocks();
}

BlockDescriptor* HeapManager::Contains(void* i_ptr) const
{
	//Search the starting address in the free list
	BlockDescriptor* pCurrent = p_freeBlockStartAddr;
	while (pCurrent != nullptr)
	{
		if (i_ptr >= pCurrent->GetStartMemAddr() && i_ptr <
			reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pCurrent->GetStartMemAddr()) +
				pCurrent->GetBlockSize()))
		{
			//std::cout << "Memory at " << i_ptr << " is contained.\n";
			return pCurrent;
		}
		pCurrent = pCurrent->GetNextBlock();
	}
	//Search the starting address in the outstanding list
	pCurrent = p_outstandingBlockStartAddr;
	while (pCurrent != nullptr)
	{
		if (i_ptr >= pCurrent->GetStartMemAddr() && i_ptr <
			reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pCurrent->GetStartMemAddr()) +
				pCurrent->GetBlockSize()))
		{
			//std::cout << "Memory at " << i_ptr << " is contained.\n";
			return pCurrent;
		}
		pCurrent = pCurrent->GetNextBlock();
	}
	std::cout << "Memory at " << i_ptr << " is not contained.\n";
	return nullptr;
}

BlockDescriptor* HeapManager::IsAllocated(void* i_ptr) const
{
	//Search the starting address in the outstanding list
	BlockDescriptor* pCurrent = p_outstandingBlockStartAddr;
	while (pCurrent != nullptr)
	{
		if (i_ptr >= pCurrent->GetStartMemAddr() && i_ptr <
			reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pCurrent->GetStartMemAddr()) +
				pCurrent->GetBlockSize()))
		{
			//std::cout << "Memory at " << i_ptr << " is allocated at " << pCurrent << ".\n";
			return pCurrent;
		}
		pCurrent = pCurrent->GetNextBlock();
	}
	std::cout << "Memory at " << i_ptr << " is not found in allocated list.\n";
	return nullptr;
}

//Find the correct position for the node to insert
bool HeapManager::InsertFreeLinkNode(void* i_ptr)
{
	BlockDescriptor* pCurrent = p_outstandingBlockStartAddr;
	BlockDescriptor* pTarget = IsAllocated(i_ptr);
	BlockDescriptor* pPrevTarget = nullptr;
	if (pTarget == nullptr)
	{
		std::cout << "Can't find the target block!\n";
		return false;
	}

	if (pTarget == p_outstandingBlockStartAddr)		//target at the list start
	{
		//std::cout << " Freed at descriptor list start: " << p_outstandingBlockStartAddr << "\n";
		//assert(p_outstandingBlockStartAddr->GetNextBlock());
		if (p_outstandingBlockStartAddr->GetNextBlock() != nullptr)
			p_outstandingBlockStartAddr = p_outstandingBlockStartAddr->GetNextBlock();
		else
			p_outstandingBlockStartAddr = nullptr;
		//std::cout << "New outstanding start: " << p_outstandingBlockStartAddr << ", next outstanding: " << p_outstandingBlockStartAddr->GetNextBlock() << "\n";
		pTarget->SetNextBlock(nullptr);
		//std::cout << "New outstanding start: " << p_outstandingBlockStartAddr << ", next outstanding: " << p_outstandingBlockStartAddr->GetNextBlock() << "\n";
	}
	else		//target in the middle of the list
	{
		//std::cout << "Freed in middle list: " << p_outstandingBlockStartAddr << "\n";
		//std::cout << "!!!!!\n";
		while (pCurrent->GetNextBlock() != nullptr)
		{
			//std::cout << "!!!!!\n";
			//std::cout << pCurrent->GetNextBlock() << " , " << pTarget << " .\n";
			if (pCurrent->GetNextBlock() == pTarget)
			{
				if (pTarget->GetNextBlock() != nullptr)
				{
					pCurrent->SetNextBlock(pCurrent->GetNextBlock()->GetNextBlock());
					pTarget->SetNextBlock(nullptr);
					break;
				}
				else
				{
					pCurrent->SetNextBlock(nullptr);
					pTarget->SetNextBlock(nullptr);
					break;
				}
			}
			pCurrent = pCurrent->GetNextBlock();
		}
	}

	assert(pTarget != p_freeBlockStartAddr);
	pTarget->SetNextBlock(p_freeBlockStartAddr);
	p_freeBlockStartAddr = pTarget;

	//
	/*pCurrent = p_freeBlockStartAddr;
	while (pCurrent->GetNextBlock() != nullptr)
	{
		if (pCurrent->GetStartMemAddr() < i_ptr && pCurrent->GetNextBlock()->GetStartMemAddr() > i_ptr)
		{
			pTarget->SetNextBlock(pCurrent->GetNextBlock());
			pCurrent->SetNextBlock(pTarget);
			break;
			return true;
		}
		pCurrent = pCurrent->GetNextBlock();
	}
	pCurrent->SetNextBlock(pTarget);*/
	return true;
}

BlockDescriptor* HeapManager::FindDescriptor(void* i_ptr)
{
	//Search the starting address in the free list
	BlockDescriptor* pCurrent = p_freeBlockStartAddr;
	while (pCurrent->GetNextBlock() != nullptr)
	{
		if (i_ptr >= pCurrent->GetStartMemAddr() && i_ptr <
			reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pCurrent->GetStartMemAddr()) +
				pCurrent->GetBlockSize()))
		{
			//std::cout << "Memory at " << i_ptr << " is contained.\n";
			return pCurrent;
		}
		pCurrent = pCurrent->GetNextBlock();
	}
	//Search the starting address in the outstanding list
	pCurrent = p_outstandingBlockStartAddr;
	while (pCurrent->GetNextBlock() != nullptr)
	{
		if (i_ptr >= pCurrent->GetStartMemAddr() && i_ptr <
			reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pCurrent->GetStartMemAddr()) +
				pCurrent->GetBlockSize()))
		{
			//std::cout << "Memory at " << i_ptr << " is contained.\n";
			return pCurrent;
		}
		pCurrent = pCurrent->GetNextBlock();
	}
	//std::cout << "Memory at " << i_ptr << " is not found.\n";
	return nullptr;
}

BlockDescriptor* HeapManager::PickOutDescriptor(void* i_ptr)
{
	//Search the starting address in the free list

	BlockDescriptor* pPrevBlock = p_freeBlockStartAddr;
	BlockDescriptor* pCurrent = p_freeBlockStartAddr->GetNextBlock();
	while (pCurrent && pCurrent->GetNextBlock() != nullptr)
	{
		//std::cout << "Looking at free node: " << pCurrent << "...\n";
		if (i_ptr >= pCurrent->GetStartMemAddr() && i_ptr <
			reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pCurrent->GetStartMemAddr()) +
				pCurrent->GetBlockSize()))
		{
			//std::cout << "Memory at " << i_ptr << " is picked from free list.\n";
			pPrevBlock->SetNextBlock(pCurrent->GetNextBlock());
			pCurrent->SetNextBlock(nullptr);
			return pCurrent;
		}
		pPrevBlock = pCurrent;
		pCurrent = pCurrent->GetNextBlock();
	}
	//Search the starting address in the outstanding list
	pPrevBlock = p_outstandingBlockStartAddr;
	pCurrent = p_outstandingBlockStartAddr->GetNextBlock();
	while (pCurrent && pCurrent->GetNextBlock() != nullptr)
	{
		//std::cout << "Looking at outstanding node: " << pCurrent << "...\n";
		if (i_ptr >= pCurrent->GetStartMemAddr() && i_ptr <
			reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pCurrent->GetStartMemAddr()) +
				pCurrent->GetBlockSize()))
		{
			//std::cout << "Memory at " << i_ptr << " is picked from allocated list.\n";
			pPrevBlock->SetNextBlock(pCurrent->GetNextBlock());
			pCurrent->SetNextBlock(nullptr);
			return pCurrent;
		}
		pPrevBlock = pCurrent;
		pCurrent = pCurrent->GetNextBlock();
	}
	std::cout << "Memory at " << i_ptr << " is not found.\n";
	return nullptr;
}

void HeapManager::SortLinkedList()
{
	//std::cout << "Before sorting: \n";
	//ShowFreeBlocks();
	int swapped;
	BlockDescriptor* ptr1;
	BlockDescriptor* ptr2 = nullptr;
	if (p_freeBlockStartAddr == nullptr)
		return;

	do
	{
		swapped = 0;
		ptr1 = p_freeBlockStartAddr;
		while (ptr1->GetNextBlock() != ptr2)
		{
			/*std::cout << "Before sorting: \n";
			ShowFreeBlocks();*/
			void* current_startAddr = ptr1->GetStartMemAddr();
			void* next_startAddr = ptr1->GetNextBlock()->GetStartMemAddr();
			//assert(cuurent_startAddr);
			//assert(next_startAddr);
			if (ptr1->GetNextBlock()->GetBlockSize() != 0)
			{
				if (current_startAddr > next_startAddr)
				{
					SwapDescriptor(ptr1, ptr1->GetNextBlock());
					swapped = 1;
				}
			}
			else
				break;

			ptr1 = ptr1->GetNextBlock();
		}
		ptr2 = ptr1;
		//std::cout << "Sorting!!!!!\n";
	} while (swapped);
	//std::cout << "After sorting: \n";
	//ShowFreeBlocks();
	//std::cout << "Sorting complete.\n";
}

inline void HeapManager::SwapDescriptor(BlockDescriptor* i_blockA, BlockDescriptor* i_blockB)
{
	BlockDescriptor* a_next = i_blockA->GetNextBlock();
	BlockDescriptor* b_next = i_blockB->GetNextBlock();
	//BlockDescriptor* temp_block = i_blockB;
	size_t temp_blockSize = i_blockA->GetBlockSize();
	void* temp_startAddr = i_blockA->GetStartMemAddr();

	i_blockA->SetFreeBlock(i_blockB->GetBlockSize(), i_blockB->GetStartMemAddr(), a_next);
	i_blockB->SetFreeBlock(temp_blockSize, temp_startAddr, b_next);
}

BlockDescriptor* HeapManager::GetFreeBlock()
{
	//std::cout << "Trying to get another free block\n";
	//ShowFreeBlocks();
	BlockDescriptor* pCurrent = p_freeBlockStartAddr;
	BlockDescriptor* pResult = nullptr;
	while (pCurrent->GetNextBlock() != nullptr)
	{
		//std::cout << "Getting free block!!!!!\n";
		//If the next block is not used (0 size and null start address)
		if (pCurrent->GetNextBlock()->GetBlockSize() == 0)
		{
			pResult = pCurrent->GetNextBlock();
			//Then set up the nodes connection
			assert(pCurrent != pCurrent->GetNextBlock());
			pCurrent->SetNextBlock(pResult->GetNextBlock());
			//break;
			return pResult;
		}
		
		pCurrent = pCurrent->GetNextBlock();
	}
	//Can't find available one - should be nullptr
	return pResult;
}

inline bool HeapManager::CheckFreeBlock()
{
	//std::cout << "Checking available free block...\n";
	//ShowFreeBlocks();
	BlockDescriptor* pCurrent = p_freeBlockStartAddr;
	while (pCurrent->GetNextBlock() != nullptr)
	{
		//std::cout << "Getting free block!!!!!\n";
		//If the next block is not used (0 size and null start address)
		if (pCurrent->GetNextBlock()->GetBlockSize() == 0)
		{
			//break;
			return true;
		}

		pCurrent = pCurrent->GetNextBlock();
	}
	//Can't find available one - should be nullptr
	return false;
}

bool HeapManager::SetRemaining(BlockDescriptor* i_allocatedBlock, void* i_startMemAddr, size_t i_size)
{
	BlockDescriptor* freeBlock = GetFreeBlock();
	if (freeBlock)
	{
		assert(freeBlock);
		//if the current allocated block is the first free block?
		if (i_allocatedBlock == p_freeBlockStartAddr)
		{
			freeBlock->SetFreeBlock(i_size, i_startMemAddr, i_allocatedBlock->GetNextBlock());
			assert(freeBlock != p_freeBlockStartAddr);
			p_freeBlockStartAddr = freeBlock;
		}
		else
		{
			//find the block's previous block
			BlockDescriptor* pPrevBlock = p_freeBlockStartAddr;
			assert(pPrevBlock->GetNextBlock());
			while (pPrevBlock->GetNextBlock()->GetBlockSize() != 0)
			{
				//std::cout << "Setting remaining!!!!!\n";
				if (pPrevBlock->GetNextBlock() == i_allocatedBlock)
					break;

				assert(pPrevBlock != pPrevBlock->GetNextBlock());
				pPrevBlock = pPrevBlock->GetNextBlock();
			}
			//set up
			freeBlock->SetFreeBlock(i_size, i_startMemAddr, i_allocatedBlock->GetNextBlock());
			assert(pPrevBlock != freeBlock);
			pPrevBlock->SetNextBlock(freeBlock);
		}

		return true;
	}
	else
	{
		return false;
	}
}

void HeapManager::MergeBlocks(BlockDescriptor* i_currentBlock, BlockDescriptor* i_nextBlock)
{

}