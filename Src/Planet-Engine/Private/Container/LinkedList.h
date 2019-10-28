#pragma once

#include <vector>
#include <algorithm>

template<typename T>
class LinkedList
{
	struct Node
	{
		T data;
		Node* next;
	};

	Node* list = nullptr;
	Node* back = nullptr;
	int size = 0;

public:
	inline int Length() const { return size; }

	T* Add(const T& item);

	bool Remove(const T& item);
	bool Remove(const T* item);

	template<typename ComparisonFunc>
	void SortToArray(ComparisonFunc function, std::vector<T>& outArray)
	{
		outArray.reserve(Length());

		Node* node = list;
		while (node != nullptr)
		{
			outArray.push_back(node->data);
			node = node->next;
		}

		std::sort(outArray.begin(), outArray.end(), function);
	}

private:

	Node* CreateNode(const T& data);
	void DeleteNode(Node* node);
};

template<typename T>
T* LinkedList<T>::Add(const T& item)
{
	Node* newNode = CreateNode(item);

	if (back)
	{
		back->next = newNode;
	}
	else
	{
		list = newNode;
	}

	back = newNode;
	++size;

	return &newNode->data;
}

template<typename T>
bool LinkedList<T>::Remove(const T& item)
{
	return false;
}

template<typename T>
bool LinkedList<T>::Remove(const T* item)
{
	return false;
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::CreateNode(const T& data)
{
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->next = nullptr;
	// construct
	T* dataptr = &newNode->data;
	new (dataptr) T(data);

	return newNode;
}

template<typename T>
void LinkedList<T>::DeleteNode(Node* node)
{

}
