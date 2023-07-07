#include<iostream>
#include<cstdlib>
#include<string>
#include<cstdio>
#include <type_traits>
#include <iostream>
#include <exception>
#include <algorithm>
#include <functional>
#include <chrono>
#include <vector>
#include <algorithm>



class Node {
public:
	std::string data;
	int key;
	Node* next;
	Node* prev;
	Node(int key, std::string data) : key(key), data(data), next(nullptr) {}
};

class ArrayStack {
private:
	Node* top;
public:
	ArrayStack() { top = nullptr; }
	void push(int key, std::string value) {
		Node* newNode = new Node(key, value);
		newNode->next = NULL;
		newNode->prev = NULL;
		if (top == NULL) {
			top = newNode;
		}
		else {
			Node* temp = top;
			while (temp->next != NULL)
				temp = temp->next;
			temp->next = newNode;
			newNode->prev = temp;
		}
	}

	std::string search_key(int key)
	{
		if (top == nullptr) return "Not Found";
		Node* node = top;

		while (node->key != key)
		{
			if (node == nullptr) return "0";
		}
		return std::to_string(node->key);

	}

	std::string search_value(int key)
	{
		if (top == nullptr) return "Not Found";
		Node* node = top;

		while (node->key != key)
		{
			if (node == nullptr) return "0";
		}
		return node->data;

	}

	void remove(int key) {
		if (top != NULL) {

			if (top->next == NULL) {
				top = NULL;
			}
			else {

				Node* temp = top;
				while (temp->next->next != NULL)
					temp = temp->next;
				Node* lastNode = temp->next;
				temp->next = NULL;

				free(lastNode);
			}
		}
	}

};

class HashMap : public ArrayStack {
private:
	int capacity;
	int size;
	ArrayStack* arr = new ArrayStack[capacity];
public:

	HashMap(int capacity) :size(0), capacity(capacity) {}

	~HashMap()
	{
		delete[] arr;
	}

	int HashFunc(int key)
	{
		return abs(key % capacity);
	}

	void insert(int key, std::string value)
	{
		int index = HashFunc(key);
		++size;
		return  arr[index].push(key, value);
	}

	std::string get_value(int key)
	{
		int index = HashFunc(key);
		return arr[index].search_value(key);
	}


	void erase(int key)
	{
		int index = HashFunc(key);
		arr[index].remove(key);
		--size;
	}

	std::string get_key(int key)
	{
		int index = HashFunc(key);
		return arr[index].search_key(key);
	}

};

int main() {

}