#include <iostream>
#include <string>
#include <memory>
#include <type_traits>
#include <algorithm>
#include <queue>
#include <vector>

template<typename T>
class Shared_ptr {
public:
	using element_type = T;

	Shared_ptr() : ptr_(nullptr), ref_count(nullptr) {}

	explicit Shared_ptr(T* p) : ptr_(p), ref_count(new int(1)) {}

	Shared_ptr(const Shared_ptr<T>& other) : ptr_(other.ptr_), ref_count(other.ref_count) {
		if (ref_count) {
			++(*ref_count);
		}
	}

	Shared_ptr(Shared_ptr<T>&& other) : ptr_(nullptr), ref_count(nullptr) {
		swap(other);
	}

	Shared_ptr& operator=(const Shared_ptr& sp) noexcept {
		if (ptr_ != sp.ptr_) {
			release();
			ptr_ = sp.ptr_;
			ref_count = sp.ref_count;
			if (ref_count) {
				++(*ref_count);
			}
		}
		return *this;
	}

	Shared_ptr& operator=(std::nullptr_t)
	{
		reset();
		return *this;
	}

	Shared_ptr& operator=(Shared_ptr&& sp) noexcept {
		if (ptr_ != sp.ptr_) {
			release();
			std::swap(ptr_, sp.ptr_);
			std::swap(ref_count, sp.ref_count);
		}
		return *this;
	}

	template <class Other>
	Shared_ptr& operator=(const Shared_ptr<Other>& sp) noexcept {
		release();
		ptr_ = sp.ptr_;
		ref_count = sp.ref_count;
		if (ref_count) {
			++(*ref_count);
		}
		return *this;
	}

	template <class Other>
	Shared_ptr& operator=(Shared_ptr<Other>&& sp) noexcept {
		release();
		std::swap(ptr_, sp.ptr_);
		std::swap(ref_count, sp.ref_count);
		return *this;
	}

	~Shared_ptr() {
		release();
	}

	T* get() const noexcept {
		return ptr_;
	}

	void reset(T* p = nullptr) {
		release();
		ptr_ = p;
		ref_count = new int(1);
	}

	void swap(Shared_ptr<T>& other) {
		std::swap(ptr_, other.ptr_);
		std::swap(ref_count, other.ref_count);
	}


	explicit operator bool() const noexcept {
		return ptr_ != nullptr;
	}



	T* operator->() const {
		return ptr_;
	}

	bool operator==(std::nullptr_t) const noexcept {
		return ptr_ == nullptr;
	}

	bool operator==(const Shared_ptr& ptr) const noexcept {
		return ptr_ == ptr.ptr_;
	}

	bool operator!=(const Shared_ptr& ptr) const noexcept {
		return ptr_ != ptr.ptr_;
	}

	friend std::ostream& operator<<(std::ostream& os, const Shared_ptr<T>& p) {
		os << p.ptr_;
		return os;
	}

	void release() {
		if (ref_count) {
			--(*ref_count);
			if (*ref_count == 0) {
				delete ptr_;
				delete ref_count;
			}
			ptr_ = nullptr;
			ref_count = nullptr;
		}
	}

private:
	T* ptr_;
	int* ref_count;
};

template<typename T, typename... Args>
Shared_ptr<T> MakeShared(Args&&... args) {
	return Shared_ptr<T>(new T(std::forward<Args>(args)...));
}


typedef bool color_type;
template<class KeyType>
struct Node {

	std::pair<KeyType, char> container;

	Shared_ptr<Node<KeyType>> parent;

	Shared_ptr<Node<KeyType>> left;

	Shared_ptr<Node<KeyType>> right;

	color_type color; // 1 -> Red, 0 -> Black
	Node(KeyType key)
	{
		container.first = key;
		left = nullptr;
		right = nullptr;
		parent = nullptr;
		color = true;
	}
	Node()
	{
		container.first = KeyType();
		left = nullptr;
		right = nullptr;
		parent = nullptr;
		color = true;
	}

};

template<class KeyType>
class Set {
private:
	Shared_ptr<Node<KeyType>> root;

	Shared_ptr<Node<KeyType>> nullptr_node;

	KeyType searchTreeHelper(Shared_ptr<Node<KeyType>> node, KeyType key) {
		if (node == nullptr_node || key == node->container.first) {
			return  node->container.first;
		}

		if (key < node->container.first) {
			return searchTreeHelper(node->left, key);
		}
		return searchTreeHelper(node->right, key);
	}


	void getRotationColorChange(Shared_ptr<Node<KeyType>> node) {
		Shared_ptr<Node<KeyType>> uncle_node;
		while (node->parent->color == true) {
			if (node->parent == node->parent->parent->right) {
				uncle_node = node->parent->parent->left; // uncle
				if (uncle_node->color == true) {
					// case 3.1
					uncle_node->color = false;
					node->parent->color = false;
					node->parent->parent->color = true;
					node = node->parent->parent;
				}
				else {
					if (node == node->parent->left) {
						// case 3.2.2
						node = node->parent;
						rightRotate(node);
					}
					// case 3.2.1
					node->parent->color = false;
					node->parent->parent->color = true;
					leftRotate(node->parent->parent);
				}
			}
			else {
				uncle_node = node->parent->parent->right; // uncle

				if (uncle_node->color == true) {
					// mirror case 3.1
					uncle_node->color = false;
					node->parent->color = false;
					node->parent->parent->color = true;
					node = node->parent->parent;
				}
				else {
					if (node == node->parent->right) {
						// mirror case 3.2.2
						node = node->parent;
						leftRotate(node);
					}
					// mirror case 3.2.1
					node->parent->color = false;
					node->parent->parent->color = true;
					rightRotate(node->parent->parent);
				}
			}
			if (node == root) {
				break;
			}
		}
		root->color = false;
	}

	void deleteFix(Shared_ptr<Node<KeyType>> x) {
		Shared_ptr<Node<KeyType>> s;
		while (x != root && x->color == 0) {
			if (x == x->parent->left) {
				s = x->parent->right;
				if (s->color == 1) {
					s->color = 0;
					x->parent->color = 1;
					leftRotate(x->parent);
					s = x->parent->right;
				}

				if (s->left->color == 0 && s->right->color == 0) {
					s->color = 1;
					x = x->parent;
				}
				else {
					if (s->right->color == 0) {
						s->left->color = 0;
						s->color = 1;
						rightRotate(s);
						s = x->parent->right;
					}

					s->color = x->parent->color;
					x->parent->color = 0;
					s->right->color = 0;
					leftRotate(x->parent);
					x = root;
				}
			}
			else {
				s = x->parent->left;
				if (s->color == 1) {
					s->color = 0;
					x->parent->color = 1;
					rightRotate(x->parent);
					s = x->parent->left;
				}

				if (s->right->color == 0 && s->right->color == 0) {
					s->color = 1;
					x = x->parent;
				}
				else {
					if (s->left->color == 0) {
						s->right->color = 0;
						s->color = 1;
						leftRotate(s);
						s = x->parent->left;
					}

					s->color = x->parent->color;
					x->parent->color = 0;
					s->left->color = 0;
					rightRotate(x->parent);
					x = root;
				}
			}
		}
		x->color = 0;
	}

	void rbTransplant(Shared_ptr<Node<KeyType>> u, Shared_ptr<Node<KeyType>> v) {
		if (u->parent == nullptr) {
			root = v;
		}
		else if (u == u->parent->left) {
			u->parent->left = v;
		}
		else {
			u->parent->right = v;
		}
		v->parent = u->parent;
	}

	void deleteNodeHelper(Shared_ptr<Node<KeyType>> node, KeyType key) {
		Shared_ptr<Node<KeyType>>  z = nullptr_node;
		Shared_ptr<Node<KeyType>>  x, y;
		while (node != nullptr_node) {
			if (node->container.first == key) {
				z = node;
			}

			if (node->container.first <= key) {
				node = node->right;
			}
			else {
				node = node->left;
			}
		}

		if (z == nullptr_node) {
			std::cout << "Key not found in the tree" << std::endl;
			return;
		}

		y = z;
		int y_original_color = y->color;
		if (z->left == nullptr_node) {
			x = z->right;
			rbTransplant(z, z->right);
		}
		else if (z->right == nullptr_node) {
			x = z->left;
			rbTransplant(z, z->left);
		}
		else {
			y = minimum(z->right);
			y_original_color = y->color;
			x = y->right;
			if (y->parent == z) {
				x->parent = y;
			}
			else {
				rbTransplant(y, y->right);
				y->right = z->right;
				y->right->parent = y;
			}

			rbTransplant(z, y);
			y->left = z->left;
			y->left->parent = y;
			y->color = z->color;
		}
		z.reset();
		if (y_original_color == 0) {
			deleteFix(x);
		}
	}

	std::vector<std::string>& inOrderHelper(Shared_ptr<Node<KeyType>> node) {
		if (node != nullptr_node) {
			inOrderHelper(node->left);
			vec.push_back(node->container.first);
			inOrderHelper(node->right);
		}
		return vec;
	}


public:
	std::vector<std::string> vec;
	Set() {
		nullptr_node = Shared_ptr<Node<KeyType>>(new Node<KeyType>());
		nullptr_node->color = false;
		nullptr_node->left = nullptr;
		nullptr_node->right = nullptr;
		root = nullptr_node;
	}

	std::vector<std::string>& inorder() {
		inOrderHelper(this->root);
		return vec;
	}

	Shared_ptr<Node<KeyType>> minimum(Shared_ptr<Node<KeyType>> node) {
		while (node->left != nullptr_node) {
			node = node->left;
		}
		return node;
	}

	Shared_ptr<Node<KeyType>> maximum(Shared_ptr<Node<KeyType>> node) {
		while (node->right != nullptr_node) {
			node = node->right;
		}
		return node;
	}

	Shared_ptr<Node<KeyType>> successor(Shared_ptr<Node<KeyType>> x) {
		if (x->right != nullptr_node) {
			return minimum(x->right);
		}

		Shared_ptr<Node<KeyType>> y = x->parent;
		while (y != nullptr_node && x == y->right) {
			x = y;
			y = y->parent;
		}
		return y;
	}

	Shared_ptr<Node<KeyType>> predecessor(Shared_ptr<Node<KeyType>> x) {
		if (x->left != nullptr_node) {
			return maximum(x->left);
		}

		Shared_ptr<Node<KeyType>> y = x->parent;
		while (y != nullptr_node && x == y->left) {
			x = y;
			y = y->parent;
		}

		return y;
	}


	void leftRotate(Shared_ptr<Node<KeyType>> x) {
		Shared_ptr<Node<KeyType>> y = x->right;
		x->right = y->left;
		if (y->left != nullptr_node) {
			y->left->parent = x;
		}
		y->parent = x->parent;
		if (x->parent == nullptr) {
			this->root = y;
		}
		else if (x == x->parent->left) {
			x->parent->left = y;
		}
		else {
			x->parent->right = y;
		}
		y->left = x;
		x->parent = y;
	}

	void rightRotate(Shared_ptr<Node<KeyType>> x) {
		Shared_ptr<Node<KeyType>> y = x->left;
		x->left = y->right;
		if (y->right != nullptr_node) {
			y->right->parent = x;
		}
		y->parent = x->parent;
		if (x->parent == nullptr) {
			this->root = y;
		}
		else if (x == x->parent->right) {
			x->parent->right = y;
		}
		else {
			x->parent->left = y;
		}
		y->right = x;
		x->parent = y;
	}

	void insert(KeyType key) {

		Shared_ptr<Node<KeyType>> node = Shared_ptr<Node<KeyType>>(new Node<KeyType>(key));
		node->container.first = key;

		node->left = nullptr_node;
		node->right = nullptr_node;

		Shared_ptr<Node<KeyType>> y;
		y = nullptr;
		Shared_ptr<Node<KeyType>> x = this->root;

		while (x != nullptr_node) {
			y = x;
			if (node->container.first < x->container.first) {
				x = x->left;
			}
			else {
				x = x->right;
			}
		}


		node->parent = y;
		if (y == nullptr) {
			root = node;
		}
		else if (node->container.first < y->container.first) {
			y->left = node;
		}
		else {
			y->right = node;
		}

		if (node->parent == nullptr) {
			node->color = false;
			return;
		}


		if (node->parent->parent == nullptr) {
			return;
		}

		getRotationColorChange(node);
	}


	void deleteNode(KeyType data) {
		deleteNodeHelper(this->root, data);
	}

	KeyType searchTree(KeyType k) {
		return searchTreeHelper(this->root, k);
	}

	KeyType operator[](const KeyType& key)
	{
		return searchTree(key);
	}

	Shared_ptr<Node<KeyType>> getRoot() {
		return this->root;
	}

	Shared_ptr<Node<KeyType>> next(Shared_ptr<Node<KeyType>> node)
	{
		Shared_ptr<Node<KeyType>> tmp = node->right;

		if (tmp) {
			while (tmp->left) tmp = tmp->left;
			return tmp;
		}
		tmp = node;
		while (tmp->parent && tmp.equals(tmp->parent->right))
			tmp = tmp->parent;

		return tmp->parent;
	}

	class iterator
	{
	private:
		Shared_ptr<Node<KeyType>> iter;
		Shared_ptr<Node<KeyType>> root_iter;
	public:
		std::pair<KeyType, char> pair;
		iterator() {}
		iterator(Shared_ptr<Node<KeyType>> root) :iter(root) {
			pair.first = root->container.first;
		}

		bool operator==(const iterator& wp)
		{
			return wp.iter == iter;
		}
		bool operator!=(const iterator& wp)
		{
			return wp.iter != iter;
		}
	};


	iterator begin()
	{
		Shared_ptr<Node<KeyType>> current = this->root;

		return iterator(minimum(current));
	}
	iterator end()
	{
		Shared_ptr<Node<KeyType>> current = this->root;
		return iterator(maximum(current));
	}
	//NOT FINISHED

};

int main()
{
	Set<int> f;
	f.insert(5);
	std::cout << f.searchTree(6);
}