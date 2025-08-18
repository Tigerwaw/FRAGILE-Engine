#pragma once
#include <array>

namespace Utilities
{
	template <typename T, int MAXSIZE>
	class StaticStack
	{
	public:
		StaticStack();
		T& Push(T& aItem);
		T& Push(T&& aItem);
		T Pop();
		T& Peek();

		void Clear();
		int Size() const;
		int MaxSize() const;
		bool IsFull() const;
		bool IsEmpty() const;
	private:
		std::array<T, MAXSIZE> myArray;
		int myLastAddedIndex = -1;
		int myMaxSize = 0;
	};

	template <typename T, int MAXSIZE>
	StaticStack<T, MAXSIZE>::StaticStack()
	{
		myMaxSize = MAXSIZE;
		myArray.fill(T());
	}

	template <typename T, int MAXSIZE>
	T& StaticStack<T, MAXSIZE>::Push(T& aItem)
	{
		assert(!IsFull());

		++myLastAddedIndex;
		myArray[myLastAddedIndex] = aItem;
		return myArray[myLastAddedIndex];
	}

	template <typename T, int MAXSIZE>
	T& StaticStack<T, MAXSIZE>::Push(T&& aItem)
	{
		assert(!IsFull());

		++myLastAddedIndex;
		myArray[myLastAddedIndex] = std::move(aItem);
		return myArray[myLastAddedIndex];
	}

	template <typename T, int MAXSIZE>
	T StaticStack<T, MAXSIZE>::Pop()
	{
		assert(!IsEmpty());

		int index = myLastAddedIndex;
		--myLastAddedIndex;
		return myArray[index];
	}

	template <typename T, int MAXSIZE>
	T& StaticStack<T, MAXSIZE>::Peek()
	{
		assert(!IsEmpty());

		return myArray[myLastAddedIndex];
	}

	template <typename T, int MAXSIZE>
	void StaticStack<T, MAXSIZE>::Clear()
	{
		myLastAddedIndex = -1;
	}

	template <typename T, int MAXSIZE>
	int StaticStack<T, MAXSIZE>::Size() const
	{
		return myLastAddedIndex + 1;
	}

	template <typename T, int MAXSIZE>
	int StaticStack<T, MAXSIZE>::MaxSize() const
	{
		return myMaxSize;
	}

	template <typename T, int MAXSIZE>
	bool StaticStack<T, MAXSIZE>::IsFull() const
	{
		return myLastAddedIndex + 1 >= myMaxSize;
	}

	template <typename T, int MAXSIZE>
	bool StaticStack<T, MAXSIZE>::IsEmpty() const
	{
		return myLastAddedIndex < 0;
	}
}