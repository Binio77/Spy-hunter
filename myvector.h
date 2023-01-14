#pragma once
extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}
#include<stdio.h>
template<typename T> class Myvector
{
public:
	Myvector() = default;
	Myvector(const Myvector<T>& another_vector);
	Myvector<T>& operator=(const Myvector<T>&);
	Myvector(size_t capacity, T initial = T{});
	void Emplace_back(const T& element);
	T Pop();
	T Get(size_t);
	size_t Size();
	~Myvector() { delete[] myvector; };
	
private:
	T* myvector = nullptr;
	size_t capacity = 0;
	size_t curr_idx = 0;
	void Reserve(const size_t capacity);


};

template<class T>
Myvector<T>::Myvector(const Myvector<T>& another_vector)
{
	
	delete[] myvector;

	curr_idx = another_vector.size();
	capacity = another_vector.capacity();
	myvector = new T[capacity];
	for (size_t i = 0; i < capacity; ++i)
		myvector[i] = another_vector[i];
	
}

template<class T>
Myvector<T>& Myvector<T>::operator=(const Myvector<T>& another_vector)
{
	delete[] myvector;
	curr_idx = another_vector.size();
	capacity = another_vector.capacity();
	myvector = new T[capacity];
	for (size_t i = 0; i < capacity; ++i)
		myvector[i] = another_vector[i];

	return *this;
}

template<class T>
Myvector<T>::Myvector(size_t _capacity, T initial) : capacity{ _capacity },
curr_idx{ _capacity },
myvector{ new T[_capacity]{} }
{
	for (size_t i = 0; i < _capacity; ++i)
		myvector[i] = initial;
}

template<class T>
void Myvector<T>::Emplace_back(const T& element)
{
	if (curr_idx == capacity)
	{
		if (capacity == 0)
			Reserve(8);
		else
			Reserve(capacity * 2);
	}

	myvector[curr_idx] = element;
	curr_idx++;
}

template<class T>
T Myvector<T>::Pop()
{
	if (curr_idx > 0)
	{
		T to_return = myvector[curr_idx - 1];     
		curr_idx--;

		return to_return;
	}
	else
		printf("Nothing to pop");
}

template<class T>
inline void Myvector<T>::Reserve(const size_t _capacity)
{
	if (_capacity > curr_idx)
	{
		T* temp = new T[_capacity];

		for (size_t i = 0; i < capacity; ++i)
			temp[i] = myvector[i];

		delete[] myvector;
		capacity = _capacity;
		myvector = temp;
	}
}

template<class T>
T Myvector<T>::Get(size_t index)
{
	for (int i = 0; i <= curr_idx; i++)
	{
		if (index == i)
			return myvector[i];
	}
	printf("Element of this index doesnt exist");
}

template<class T>
size_t Myvector<T>::Size()
{
	return curr_idx;
}
