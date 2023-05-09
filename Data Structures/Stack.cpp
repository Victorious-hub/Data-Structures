#include<iostream>

template<class T>
struct Node {
	T data;
	Node* next;
	Node(T n) :data(n), next(nullptr) {}
};
template<class T>
class Stack {
	Node<T>* top;
public:
	Stack() { top = nullptr; }

	void push(T data)
	{
		Node<T>* temp = new Node<T>(data);

		temp->next = top;

		top = temp;
	}


	bool isEmpty()
	{
		return top == nullptr;
	}


	int peek()
	{
		if (!isEmpty())
			return top->data;
		else
			exit(1);
	}



	void pop()
	{
		Node<T>*  temp = top;
		if (!temp) {
			return;
		}
		top = top->next;
		delete temp;
	}

	void display()
	{
		Node<T>* temp = top;
		if (!temp) {
			return;
		}
		while (temp) {
			std::cout << temp->data << "\n";
			temp = temp->next;
		}
		
	}
};


int main()
{

	Stack<int> s;


	s.push(11);
	s.push(22);
	s.push(33);
	s.push(44);


	s.display();
	std::cout << "lox" << "\n";
	s.pop();
	s.pop();

	
	s.display();


	return 0;
}