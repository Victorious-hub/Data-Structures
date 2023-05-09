#include <iostream>

template<class T>
struct Node
{
	Node<T>* next;
	Node<T>* prev;
	T data;
	Node()
	{
		next = prev = nullptr;
	}
	Node(T value):next(nullptr),prev(nullptr),data(value){}
};
template<class T>
class  DoublyLinkedList
{
	int size;
	Node<T>* head;
	Node<T>* tail;
public:
	DoublyLinkedList()
	{
		head = tail = nullptr;
		size = 0;
	}
	void push_back(T value)
	{
		Node<T>* newNode = new Node<T>(value);
		if (!tail)
		{
			tail = newNode;
			head = tail;
		}
		else
		{		
			tail->next = newNode;
			newNode->prev = tail;
			tail = newNode;
		}
		++size;
	}

	void push_front(T value)
	{
		Node<T>* newNode = new Node<T>(value);
		if (!head)
		{
			head = newNode;
			tail = head;
		}
		else
		{
			head->prev = newNode;
			newNode->next = head;
			head = newNode;
		}
		++size;
	}

	void display()
	{
		Node<T>* newNode = head;
		while (newNode)
		{
			std::cout << newNode->data<<"\n";
			newNode = newNode->next;
		}
	}

	void pop_front()
	{
		if (!head)return;
		Node<T>* newNode = head;

		head = newNode->next;
		head->prev = nullptr;
		delete newNode;
		--size;
	}

	void pop_back()
	{
		if (!tail)return;
		Node<T>* newNode = tail;

		tail = newNode->prev;
		tail->next = nullptr;
		delete newNode;
		--size;
	}

	Node<T>* at(int pos)
	{

			Node<T>* newNode = head;
			int counter = 0;
			while (newNode)
			{
				if (counter == pos) return newNode;
				newNode = newNode->next;
				++counter;
			}
			newNode == nullptr;
			return newNode;

	}

	void insert(int pos, T value)
	{
		if (!pos || !head) push_front(value);
		else if (pos == size)push_back(value);
		else
		{
			Node<T>* newNode = new Node<T>(value);

			Node<T>* leftNode = at(pos - 1);
			Node<T>* rightNode = at(pos);

			if (!leftNode || !rightNode) return;

			newNode->next = rightNode;
			newNode->prev = leftNode;

			rightNode->prev = newNode;
			leftNode->next = newNode;
		}
		
	}

	void erase(int pos)
	{
		if (!head) return;
		if (!pos)pop_front();
		if (pos == size-1)pop_back();
		else
		{
			Node<T>* leftNode = at(pos-1);

			Node<T>* rightNode = at(pos+1);
			
			Node<T>* newNode = at(pos);
		
			if (!leftNode || !rightNode || !newNode) return;

			leftNode->next = rightNode;
			rightNode->prev = leftNode;
			delete newNode;
		}

	}

	Node<T>* front()
	{
		if (!head) return nullptr;
		return head->data;
	}
	Node<T>* back()
	{
		if (!tail) return nullptr;
		return tail->data;
	}
};

int main()
{
	DoublyLinkedList<int> lox;
	lox.push_back(10);
	lox.push_back(20);
	lox.push_back(30);

	lox.push_front(40);

	lox.push_front(50);

	lox.push_back(60);


	lox.display();


	lox.erase(3);

	lox.insert(-30, 525225);

	lox.display();
}
