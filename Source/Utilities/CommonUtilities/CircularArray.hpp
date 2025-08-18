#pragma once
#include <array>

namespace Utilities
{
	template <typename T, int MAXSIZE>
	class CircularArray
	{
	public:
		CircularArray();
		T& Push_back(T& aItem);
		T& Emplace_back(T&& aItem);
		T Pop_Front();
		T& Peek_Front();
		T& Peek_Next();
		T& Peek_Latest();
		void Clear();
		int Size() const;
		int MaxSize() const;

		const T& operator[](int aIndex) const;
		T& operator[](int aIndex);
	private:
		std::array<T, MAXSIZE> myArray;
		int myFirstIndex = 0;
		int myLastIndex = 0;
		int myMaxSize = 0;
	};

	template<typename T, int MAXSIZE>
	inline CircularArray<T, MAXSIZE>::CircularArray()
	{
		myMaxSize = MAXSIZE;
	}

	template<typename T, int MAXSIZE>
	inline T& CircularArray<T, MAXSIZE>::Push_back(T& aItem)
	{
		int lastIndex = myLastIndex;
		int newLastIndex = (myLastIndex + 1) % myMaxSize;

		if (newLastIndex == myFirstIndex)
		{
			myFirstIndex = (myFirstIndex + 1) % myMaxSize;
		}

		myArray[myLastIndex] = aItem;
		myLastIndex = newLastIndex;

		return myArray[lastIndex];
	}

	template<typename T, int MAXSIZE>
	inline T& CircularArray<T, MAXSIZE>::Emplace_back(T&& aItem)
	{
		int lastIndex = myLastIndex;
		int newLastIndex = (myLastIndex + 1) % myMaxSize;

		if (newLastIndex == myFirstIndex)
		{
			myFirstIndex = (myFirstIndex + 1) % myMaxSize;
		}

		myArray[myLastIndex] = std::move(aItem);
		myLastIndex = newLastIndex;

		return myArray[lastIndex];
	}

	template<typename T, int MAXSIZE>
	inline T CircularArray<T, MAXSIZE>::Pop_Front()
	{
		assert(myFirstIndex != myLastIndex);

		T& item = this->operator[](0);
		++myFirstIndex;
		if (myFirstIndex >= myMaxSize)
		{
			myFirstIndex = 0;
		}

		return item;
	}

	template<typename T, int MAXSIZE>
	inline T& CircularArray<T, MAXSIZE>::Peek_Front()
	{
		assert(myFirstIndex != myLastIndex);

		return this->operator[](0);
	}

	template<typename T, int MAXSIZE>
	inline T& CircularArray<T, MAXSIZE>::Peek_Next()
	{
		assert(myFirstIndex != myLastIndex);

		return this->operator[](1);
	}

	template<typename T, int MAXSIZE>
	inline T& CircularArray<T, MAXSIZE>::Peek_Latest()
	{
		assert(myFirstIndex != myLastIndex);

		return this->operator[](Size());
	}

	template<typename T, int MAXSIZE>
	inline void CircularArray<T, MAXSIZE>::Clear()
	{
		myFirstIndex = 0;
		myLastIndex = 0;
	}

	template<typename T, int MAXSIZE>
	inline int CircularArray<T, MAXSIZE>::Size() const
	{
		if (myLastIndex < myFirstIndex)
		{
			return myMaxSize - abs(myFirstIndex - myLastIndex);
		}

		return abs(myFirstIndex - myLastIndex);
	}

	template<typename T, int MAXSIZE>
	inline int CircularArray<T, MAXSIZE>::MaxSize() const
	{
		return myMaxSize;
	}

	template<typename T, int MAXSIZE>
	inline const T& CircularArray<T, MAXSIZE>::operator[](int aIndex) const
	{
		assert(aIndex >= 0);
		assert(aIndex < myMaxSize);

		return myArray[(myFirstIndex + aIndex) % myMaxSize];
	}

	template<typename T, int MAXSIZE>
	inline T& CircularArray<T, MAXSIZE>::operator[](int aIndex)
	{
		assert(aIndex >= 0);
		assert(aIndex < myMaxSize);

		return myArray[(myFirstIndex + aIndex) % myMaxSize];
	}
}