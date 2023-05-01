#include <iostream>
#include <string>
template<class T>
struct Node
{
	T data;
	Node<T>* next = nullptr;
	Node(){}
	Node(T data):data(data){}
};
template<class T>
class SinglyLinkedList
{
	Node<T>*  head;
	Node<T>*  tail;
public:
	SinglyLinkedList()
	{
		head = tail = nullptr;
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
			
			head->next = newNode;
			head = newNode;
		}

	}
	void pop()
	{
		if (!head)return;
		Node<T>* temp = head;

        head = temp->next;

	    delete temp;
		
	}
	T peek()
	{
		if (head) {
			return head->data;
		}
		return "List is empty";
	}

	void display()
	{
		while (tail) {
			std::cout << tail->data << "\n";
			tail = tail->next;
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
	std::cout << lst.empty();
}