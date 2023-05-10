#include <iostream>
#include <string>
template<class T>
struct Node
{
	T data;
	Node<T>* next = nullptr;
	Node() {}
	Node(T data) :data(data) {}
};

template<class T>
class Queue
{
	Node<T>* head;
	Node<T>* tail;
public:
	Queue()
	{
		head = tail = nullptr;

	}
	void push(T data)
	{
		Node<T>*  newNode = new Node<T>(data);
		if (!head)
		{
			head = tail = newNode;
			return;
		}
		
		tail->next = newNode;

		tail = newNode;
	}
	void pop()
	{
		if (!head)
			return;

		Node<T>* temp = head;
		head = head->next;

		if (!head)
			tail = nullptr;

		delete temp;
	}
    
	void display()
	{
		Node<T>* temp = head;

		while (temp)
		{
			std::cout << temp->data << "\n";
			temp = temp->next;
		}
	}

	T back()
	{
		if (!empty()) {
			return tail->data;
		}
		std::cout << "Queue is empty" << '\n';
		return T();
	}

	T front()
	{
		if (!empty()) {
			return head->data;
		}
		std::cout << "Queue is empty" << '\n';
		return T();
	}

	bool empty()
	{
		return head == nullptr;
	}
};

#include <queue>
int main()
{

	Queue<std::string> data;
	
	data.display();

}


