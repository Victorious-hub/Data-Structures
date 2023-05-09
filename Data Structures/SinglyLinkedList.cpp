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
class SinglyLinkedList
{
	int size;
	Node<T>*  head;
	Node<T>*  tail;
public:
	SinglyLinkedList()
	{
		head = tail = nullptr;
		size = 0;
	}
	void push(T data)
	{
		if (!head)
		{
			head = new Node<T>(data);
			tail = head;
		}
		else {
			Node<T>*  newNode = new Node<T>(data);

			tail->next = newNode;
			tail = newNode;
		}
		++size;

	}
	void pop()
	{
		if (!head)return;
		Node<T>* temp = tail;

		tail = temp->next;

		delete temp;
		--size;

	}
	T  peek()
	{
		Node<T>* newNode = head;
		int counter = 1;
		while (newNode) {

			if (counter == size) return newNode->data;			
			++counter;
			newNode = newNode->next;
		}
	}

	void display()
	{
		Node<T>* newNode = head;
		while (newNode) {
			std::cout << newNode->data << "\n";
			newNode = newNode->next;
		}
	}

	bool empty()
	{
		return head == nullptr ? 1 : 0;
	}
};


int main()
{

	SinglyLinkedList<std::string> lst;
	lst.push("qwrqr");

	lst.push("q424");

	lst.push("q14144");

	lst.push("q14144");

	lst.pop();

	lst.display();

	lst.display();

	std::cout << (lst.peek());
}

