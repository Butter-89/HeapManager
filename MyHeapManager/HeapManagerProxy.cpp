#include "HeapManagerProxy.h"
#include <iostream>

HeapManager* HeapManagerProxy::CreateHeapManager(void* i_pMemory, size_t i_sizeMemory, unsigned int i_numDescriptors)
{
	//std::cout << "Create heap manager here\n";
	HeapManager* heapManager = new (i_pMemory) HeapManager(i_numDescriptors, i_pMemory, i_sizeMemory);
	//std::cout << "pMemory: " << i_pMemory << "\n";
	//std::cout << "Heap Manager size: " << sizeof(*heapManager) << "\n";
	return heapManager;
}

void HeapManagerProxy::Destroy(HeapManager* i_pManager)
{
	i_pManager->~HeapManager();
}

void* HeapManagerProxy::alloc(HeapManager* i_pManager, size_t i_size)
{
	void* pUserMem = nullptr;
	pUserMem = i_pManager->alloc(i_size);
	if (pUserMem)
	{
		//std::cout << pUserMem << '\n';
		return pUserMem;
	}
	else
	{
		//std::cout << pUserMem << '\n';
		return nullptr;
	}
}

void* HeapManagerProxy::alloc(HeapManager* i_pManager, size_t i_size, unsigned int i_alignment)
{
	void* pUserMem = nullptr;
	pUserMem = i_pManager->alloc(i_size, i_alignment);
	if (pUserMem)
	{
		//std::cout << pUserMem << '\n';
		return pUserMem;
	}
	else
	{
		//std::cout << pUserMem << '\n';
		return nullptr;
	}
		
}

bool HeapManagerProxy::free(HeapManager* i_pManager, void* i_ptr)
{
	return i_pManager->Free(i_ptr);
}

void HeapManagerProxy::Collect(HeapManager* i_pManager)
{
	i_pManager->Collect();
}

bool HeapManagerProxy::Contains(const HeapManager* i_pManager, void* i_ptr)
{
	BlockDescriptor* success = i_pManager->Contains(i_ptr);
	if (success)
		return true;
	else
		return false;
}

bool HeapManagerProxy::IsAllocated(const HeapManager* i_pManager, void* i_ptr)
{
	BlockDescriptor* success = i_pManager->IsAllocated(i_ptr);
	if (success)
		return true;
	else
		return false;
}

size_t HeapManagerProxy::GetLargestFreeBlock(const HeapManager* i_pManager)
{

	return size_t();
}

size_t HeapManagerProxy::GetTotalFreeMemory(const HeapManager* i_pManager)
{

	return size_t();
}

void HeapManagerProxy::ShowFreeBlocks(const HeapManager* i_pManager)
{
	i_pManager->ShowFreeBlocks();
}

void HeapManagerProxy::ShowOutstandingAllocations(const HeapManager* i_pManager)
{
	i_pManager->ShowOutstandingBlocks();
}
