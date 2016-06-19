#include "profiler/profiler.h"
#include <fstream>
#include <list>
#include <iostream>
#include <map>
#include <stack>
#include <vector>
#include <iterator>
#include <algorithm> 
#include <ctime>
#include <chrono>
#include "shared_allocator/cached_allocator.hpp"




struct BlocksTree;
////////////////////////////////////////////////////////////
salloc::local_cached_allocator<BlocksTree> ALLOC;

template <class T>
class singletone_allocator
{
protected:

	typedef singletone_allocator<T> ThisType;
	typedef ::std::vector<T*> MemoryCache;

	MemoryCache m_memoryCache; ///< Memory cache of objects

public:

	typedef T                         value_type;

	typedef value_type*                  pointer;
	typedef const value_type*      const_pointer;
	typedef void*                   void_pointer;
	typedef const void*       const_void_pointer;

	typedef value_type&                reference;
	typedef const value_type&    const_reference;

	typedef size_t                     size_type;
	typedef ptrdiff_t            difference_type;

	/** \brief Auxiliary struct to convert this type to single_cached_allocator of other type. */
	template <class U>
	struct rebind
	{
		typedef singletone_allocator<U> other;
	};

	/** \brief Returns an actual adress of value.

    \note Uses std::addressof */
	pointer address(reference _value) const throw()
	{
		return ::std::addressof(_value);
	}

	/** \brief Returns an actual adress of const value.

    \note Uses std::addressof */
	const_pointer address(const_reference _value) const throw()
	{
		return ::std::addressof(_value);
	}

	/** \brief Default constructor.

    Does nothing. */
	singletone_allocator() throw()
	{
	}

	/** \brief Empty copy constructor.

    Does nothing. */
	singletone_allocator(const ThisType&) throw()
	{
	}

	/** \brief Empty copy constructor.

    Does nothing. */
	template <class U>
	singletone_allocator(const U&) throw()
	{
	}

	/** \brief Move constructor.

    Moves reserved memory. */
	singletone_allocator(ThisType&& _rvalueAlloc) : m_memoryCache(::std::move(_rvalueAlloc.m_memoryCache))
	{
	}

	/** \brief Empty assignment operator.

    Does nothing and returns reference to this allocator. */
	template <class U>
	ThisType& operator=(const U&)
	{
		return *this;
	}

	~singletone_allocator()
	{
		private_clear();
	}

	/** \brief Clear memory cache.

    Also deallocates all cached memory. */
	void clear()
	{
		private_clear();
		m_memoryCache.clear();
	}

	/** \brief Swaps two allocators with their cache.

    \param _anotherAlloc Reference to another allocator. */
	void swap(ThisType& _anotherAlloc)
	{
		m_memoryCache.swap(_anotherAlloc.m_memoryCache);
	}

	/** \brief Reserve memory for certain number of elements.

    Reserves memory for number of buffers of specified size.

    \param _arraySize Required number of elements to reserve (size of one memory buffer).
    \param _reservationsNumber Required number of memory buffers. */
	void reserve(size_type _arraySize, size_type _reservationsNumber)
	{
		m_memoryCache.reserve(m_memoryCache.size() + _reservationsNumber);
		for (size_type i = 0; i < _reservationsNumber; ++i)
		{
			m_memoryCache.push_back(ALLOC.allocate(_arraySize));
		}
	}

	/** \brief Returns number of elements in allocated memory.

    \param _memory Pointer to allocated memory.

    \warning Please, notice that the number of ELEMENTS will be returned (not number of BYTES).

    \note This function made template to avoid compiler errors for allocators that do not have size() function. */
	template <class U>
	inline size_type size(const U* _memory) const
	{
		return ALLOC.size(_memory);
	}

	/** \brief Stores pointer to memory in cache to be used later.

    \param _memory Pointer to allocated memory. */
	void deallocate(pointer _memory, size_type = 0)
	{
		//printf("deallocate\n");
		m_memoryCache.push_back(_memory);
	}

	/** \brief Truly deallocates memory.

    \param _memory Pointer to allocated memory. */
	inline void deallocate_force(pointer _memory, size_type = 0)
	{
		ALLOC.deallocate(_memory, 0);
	}

	/** \brief Allocate elements.

    \param _number Required number of elements. */
	pointer allocate(size_type _number = 1)
	{
		//printf("allocate %d\n", _number);
		if (!m_memoryCache.empty())
		{
			pointer pMem = m_memoryCache.back();
			m_memoryCache.pop_back();
			return _number < 2 ? pMem : ALLOC.allocate(_number, pMem);
		}

		return ALLOC.allocate(_number);
	}

	/** \brief Allocate elements using hint.

    \param _number Required number of elements.
    \param _currentMemory Pointer to memory allocated earlier (it contains size which will be used as a hint). */
	pointer allocate(size_type _number, void* _currentMemory)
	{
		//printf("allocate_hint %d %lx\n", _number, _currentMemory);
		if (!m_memoryCache.empty())
		{
			pointer pMem = m_memoryCache.back();
			m_memoryCache.pop_back();
			return _number < 2 ? pMem : ALLOC.allocate(_number, pMem);
		}

		return ALLOC.allocate(_number, _currentMemory);
	}

	/** \brief Construct new object on preallocated memory using default constructor.

    \param _singleObject Pointer to preallocated memory. */
	inline void construct(T* _singleObject)
	{
		ALLOC.construct(_singleObject);
	}

	/** \brief Construct new object on preallocated memory using copy-constructor.

    \param _singleObject Pointer to preallocated memory.
    \param _value Const-reference to another object instance to be coped from.

    \note Declared as template function to make it possible to use this allocator with
    types without public copy-constructor. */
	template <class U>
	inline void construct(U* _singleObject, const U& _value)
	{
		ALLOC.construct(_singleObject, _value);
	}

	/** \brief Construct new object on preallocated memory using move-constructor.

    \param _singleObject Pointer to preallocated memory.
    \param _value Rvalue-reference to another object instance to be moved from.

    \note Declared as template function to make it possible to use this allocator with
    types without public move-constructor. */
	template <class U>
	inline void construct(U* _singleObject, U&& _value)
	{
		ALLOC.construct(_singleObject, ::std::forward<U&&>(_value));
	}

	/** \brief Construct new object on preallocated memory using arguments list.

    \param _singleObject Pointer to preallocated memory.
    \param _constructorArguments Variadic arguments list to be used by object constructor.

    \note Declared as template function to make it possible to use this allocator with
    types without specific constructor with arguments. */
	template <class U, class... TArgs>
	inline void construct(U* _singleObject, TArgs&&... _constructorArguments)
	{
		::new (static_cast<void*>(_singleObject)) U(::std::forward<TArgs>(_constructorArguments)...);
	}

	template <class U>
	inline void destroy(U* _singleObject)
	{
		ALLOC.destroy(_singleObject);
	}

	size_t max_size() const throw()
	{
		return (size_t)(-1) / sizeof(T);
	}

protected:

	inline void private_clear()
	{
		for (auto pMem : m_memoryCache)
		{
			ALLOC.deallocate(pMem, 0);
		}
	}

};

/** Operator == for STL compatibility. */
template <class T>
inline bool operator == (const singletone_allocator<T>&, const singletone_allocator<T>&) throw()
{
	return true;
}

template <class T, class U>
inline bool operator == (const singletone_allocator<T>&, const singletone_allocator<U>&) throw()
{
	return false;
}
////////////////////////////////////////////////////////////






struct BlocksTree
{
	profiler::SerilizedBlock* node;
	std::vector<BlocksTree, singletone_allocator<BlocksTree> > children;
	//std::vector<BlocksTree > children;
	BlocksTree* parent;
	BlocksTree(){
		node = nullptr;
		parent = nullptr;
		children.reserve(6);
	}

	BlocksTree(BlocksTree&& that)
	{
		makeMove(std::forward<BlocksTree&&>(that));
	}

	BlocksTree& operator=(BlocksTree&& that)
	{
		makeMove(std::forward<BlocksTree&&>(that));
		return *this;
	}

	~BlocksTree(){
		if (node){
			delete node;
		}
		node = nullptr;
		parent = nullptr;
	}

	bool operator < (const BlocksTree& other) const 
	{
		if (!node || !other.node){
			return false;
		}
		return node->block()->getBegin() < other.node->block()->getBegin();
	}

private:
	void makeMove(BlocksTree&& that)
	{
		node = that.node;
		parent = that.parent;

		children = std::move(that.children);

		that.node = nullptr;
		that.parent = nullptr;
	}

};

int main()
{
	std::ifstream inFile("test.prof", std::fstream::binary);

	if (!inFile.is_open()){
		return -1;
	}

	typedef std::map<size_t, BlocksTree> thread_blocks_tree_t;

	thread_blocks_tree_t threaded_trees;

	int blocks_counter = 0;

	ALLOC.reserve(6, 1400);
	ALLOC.reserve(1400, 1);
	auto start = std::chrono::system_clock::now();
	while (!inFile.eof()){
		uint16_t sz = 0;
		inFile.read((char*)&sz, sizeof(sz));
		if (sz == 0)
		{
			inFile.read((char*)&sz, sizeof(sz));
			continue;
		}
		char* data = new char[sz];
		inFile.read((char*)&data[0], sz);
		profiler::BaseBlockData* baseData = (profiler::BaseBlockData*)data;

		BlocksTree& root = threaded_trees[baseData->getThreadId()];

		BlocksTree tree;
		tree.node = new profiler::SerilizedBlock(sz, data);
		blocks_counter++;

		if (root.children.empty()){
			root.children.push_back(std::move(tree));
		}
		else{
			BlocksTree& back = root.children.back();
			auto t1 = back.node->block()->getEnd();
			auto mt0 = tree.node->block()->getBegin();
			if (mt0 < t1)//parent - starts earlier than last ends
			{
				auto lower = std::lower_bound(root.children.begin(), root.children.end(), tree);

				std::move(lower, root.children.end(), std::back_inserter(tree.children));
				
				root.children.erase(lower, root.children.end());

			}

			root.children.push_back(std::move(tree));
		}
		

		delete[] data;

	}
	auto end = std::chrono::system_clock::now();

	std::cout << "Blocks count: " << blocks_counter << std::endl;
	std::cout << "dT =  " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " usec" << std::endl;
	return 0;
}
