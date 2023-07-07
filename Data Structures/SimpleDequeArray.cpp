
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <type_traits>
#include <iostream>

template <class T, class Del = std::default_delete<T>>
class UniquePtr {
public:
	// Constructors
	UniquePtr() noexcept : ptr_(nullptr), deleter_(Del()) {}
	UniquePtr(nullptr_t) noexcept : ptr_(nullptr), deleter_(Del()) {}
	explicit UniquePtr(T* ptr) noexcept : ptr_(ptr), deleter_(Del()) {}
	UniquePtr(T* ptr, const Del& deleter) noexcept : ptr_(ptr), deleter_(deleter) {}
	UniquePtr(T* ptr, Del&& deleter) noexcept : ptr_(ptr), deleter_(std::move(deleter)) {}
	UniquePtr(UniquePtr&& other) noexcept : ptr_(other.release()), deleter_(std::move(other.deleter_)) {}

	// Move assignment operator
	UniquePtr& operator=(UniquePtr&& other) noexcept {
		reset(other.release());
		deleter_ = std::move(other.deleter_);
		return *this;
	}

	// Copy operations deleted
	UniquePtr(const UniquePtr&) = delete;
	UniquePtr& operator=(const UniquePtr&) = delete;

	// Destructor
	~UniquePtr() noexcept {
		reset();
	}

	// Release ownership
	T* release() noexcept {
		T* ptr = ptr_;
		ptr_ = nullptr;
		return ptr;
	}

	// Reset pointer and deleter
	void reset(T* ptr = nullptr) noexcept {
		if (ptr_ != ptr) {
			deleter_(ptr_);
			ptr_ = ptr;
		}
	}

	// Swap pointers and deleters
	void swap(UniquePtr& other) noexcept {
		std::swap(ptr_, other.ptr_);
		std::swap(deleter_, other.deleter_);
	}

	// Accessor
	T* get() const noexcept {
		return ptr_;
	}

	Del& get_deleter() {
		return deleter_;
	}

	const Del& get_deleter() const {
		return deleter_;
	}

	// Dereference operator
	T& operator*() const noexcept {
		return *ptr_;
	}

	T* operator->() const noexcept {
		return ptr_;
	}

	friend std::ostream& operator<<(std::ostream& os, const UniquePtr<T>& p) {
		os << p.ptr_;
		return os;
	}

	// Boolean conversion operator
	explicit operator bool() const noexcept {
		return ptr_ != nullptr;
	}

private:
	T* ptr_;
	Del deleter_;
};

template <class T, class D>
class UniquePtr<T[], D> {
public:
	using pointer = T * ;
	using element_type = T;
	using deleter_type = D;

	// Constructors
	constexpr UniquePtr() noexcept : ptr_(nullptr), deleter_(deleter_type()) {}

	template <class U>
	explicit UniquePtr(U p) noexcept : ptr_(p), deleter_(deleter_type()) {}

	template <class U>
	UniquePtr(U p, typename std::enable_if<std::is_convertible<U, pointer>::value, deleter_type>::type d) noexcept
		: ptr_(p), deleter_(d) {}

	template <class U>
	UniquePtr(U p, typename std::enable_if<std::is_convertible<U, pointer>::value, deleter_type&&>::type d) noexcept
		: ptr_(p), deleter_(std::move(d)) {}

	UniquePtr(UniquePtr&& u) noexcept : ptr_(u.ptr_), deleter_(std::move(u.deleter_)) {
		u.ptr_ = nullptr;
	}

	template <class U, class E>
	UniquePtr(UniquePtr<U, E>&& u) noexcept : ptr_(u.release()), deleter_(std::move(u.get_deleter())) {}

	// Destructor
	~UniquePtr() {
		reset();
	}

	// Move assignment operators
	UniquePtr& operator=(UniquePtr&& u) noexcept {
		reset(u.release());
		deleter_ = std::move(u.deleter_);
		return *this;
	}

	template <class U, class E>
	UniquePtr& operator=(UniquePtr<U, E>&& u) noexcept {
		reset(u.release());
		deleter_ = std::move(u.get_deleter());
		return *this;
	}

	UniquePtr& operator=(nullptr_t) noexcept {
		reset();
		return *this;
	}

	// Element access
	T& operator[](size_t i) const {
		return ptr_[i];
	}

	// Accessors
	pointer get() const noexcept {
		return ptr_;
	}

	deleter_type& get_deleter() noexcept {
		return deleter_;
	}

	const deleter_type& get_deleter() const noexcept {
		return deleter_;
	}

	explicit operator bool() const noexcept {
		return ptr_ != nullptr;
	}

	// Release ownership
	pointer release() noexcept {
		pointer p = ptr_;
		ptr_ = nullptr;
		return p;
	}

	// Reset pointer and deleter
	void reset(pointer p = pointer()) noexcept {
		if (ptr_ != p) {
			deleter_(ptr_);
			ptr_ = p;
		}
	}

	void reset(nullptr_t) noexcept {
		reset();
	}

	template <class U>
	void reset(U p) noexcept = delete;

	// Swap pointers and deleters
	void swap(UniquePtr& u) noexcept {
		std::swap(ptr_, u.ptr_);
		std::swap(deleter_, u.deleter_);
	}

private:
	pointer ptr_;
	deleter_type deleter_;
};

// Non-member swap function
template <class T, class Del>
void Swap(UniquePtr<T, Del>& lhs, UniquePtr<T, Del>& rhs) noexcept {
	lhs.swap(rhs);
}

// Make unique function
/*template <class T, size_t N>
UniquePtr<T[]> MakeUnique() {
	return UniquePtr<T[]>(new T[N]);
}

template <class T, class... Args>
UniquePtr<T> MakeUnique(Args&&... args) {
	return UniquePtr<T>(new T(std::forward<Args>(args)...));
}*/

template <class T, class ...Args>
std::enable_if_t<!std::is_array_v<T>, UniquePtr<T>>
MakeUnique(Args&& ...args)
{
	return UniquePtr<T>(new T(std::forward<Args>(args)...));
};

template <class T>
std::enable_if_t<std::is_array_v<T>, UniquePtr<T>>
MakeUnique(int size)
{
	using type = std::remove_extent_t<T>;
	return UniquePtr<T>(new type[size]);
};

template <class X>
class Deque {

private:
	// Stores the frontIndex
	int frontIndex;

	// Stores the backIndex
	int backIndex;

	// Stores the array
	UniquePtr<X[]> arr;

	// Stores the size of deque
	int sizeVar;

	// Stores the size of array
	int capacityVar = 4;

public:
	// Deque class constructor
	Deque()
	{
		arr = MakeUnique<X[]>(capacityVar);
		frontIndex = backIndex = -1;
		sizeVar = 0;
	}

	// Function methods
	bool empty();
	bool full();
	void push_back(X x);
	void push_front(X x);
	void pop_front();
	void pop_back();
	X front();
	X back();
	int capacity();
	int size();

	X& operator[](int index)
	{
		return arr[index];
	}

	void clear() { arr.reset(); }
};

// Function to find the capacity of the deque
template <class X>
int Deque<X>::capacity()
{
	return capacityVar;
}

// Function to find the number of elements
// present in deque
template <class X>
int Deque<X>::size() { return sizeVar; }

// Function to check if deque is empty or not
template <class X>
bool Deque<X>::empty()
{
	if (frontIndex == -1 && backIndex == -1)
		return true;
	else
		return false;
}

// Function to check if deque is full or not
template <class X>
bool Deque<X>::full()
{
	if (sizeVar == capacityVar)
		return true;
	else
		return false;
}

// Function to find the front element of the deque
template <class X>
X Deque<X>::front()
{
	// If deque is empty
	if (empty()) {
		std::cout << "Deque underflow" << std::endl;
		abort();
	}
	return arr[frontIndex];
}

// Function to find the last element of the deque
template <class X>
X Deque<X>::back()
{
	// If deque is empty
	if (empty()) {
		std::cout << "Deque underflow" << std::endl;
		abort();
	}
	return arr[backIndex];
}

// Function to insert the element
// to the back of the deque
template <class X>
void Deque<X>::push_back(X x)
{
	if (full()) {

		// If the deque is full, then
		// double the capacity
		capacityVar = capacityVar * 2;

		// Initialize new array of
		// double size
		UniquePtr<X[]> temp = MakeUnique<X[]>(capacityVar);

		// Copy the elements of the
		// previous array
		int i = frontIndex;
		int j = 0;
		while (i != backIndex) {
			temp[j] = arr[i];
			i = (i + 1) % sizeVar;
			j++;
		}
		temp[j] = arr[i];

		frontIndex = 0;
		backIndex = sizeVar - 1;

		// Deallocate the memory
		// of previous array
		arr.reset();
		arr = std::move(temp);
	}

	// If size is zero
	if (empty()) {
		frontIndex = backIndex = 0;
		arr[backIndex] = x;
		sizeVar++;
		return;
	}

	// Increment back index cyclically
	backIndex = (backIndex + 1) % capacityVar;
	arr[backIndex] = x;
	sizeVar++;
	return;
}

// Function to insert the element
// to the front of the deque
template <class X>
void Deque<X>::push_front(X x)
{
	if (full()) {

		// If the deque is full, then
		// double the capacity
		capacityVar = capacityVar * 2;

		// Initialize new array of
		// double size
		X* temp = new X[capacityVar];

		// Copy the elements of the
		// previous array
		int i = frontIndex;
		int j = 0;
		while (i != backIndex) {
			temp[j] = arr[i];
			i = (i + 1) % sizeVar;
			j++;
		}
		temp[j] = arr[i];

		frontIndex = 0;
		backIndex = sizeVar - 1;

		// Deallocate the memory
		// of previous array
		delete[] arr;
		arr = temp;
	}

	// If size is zero
	if (empty()) {
		frontIndex = backIndex = 0;
		arr[backIndex] = x;
		sizeVar++;
		return;
	}

	// Decrement front index cyclically
	frontIndex
		= (frontIndex - 1 + capacityVar) % capacityVar;
	arr[frontIndex] = x;
	sizeVar++;
	return;
}

// Function to delete the element
// from the front of the deque
template <class X>
void Deque<X>::pop_front()
{
	// If deque is empty
	if (empty()) {
		std::cout << "Deque underflow" << std::endl;
		abort();
	}

	// If there is only one character
	if (frontIndex == backIndex) {

		// Mark deque as empty
		// and decrement sizeVar
		frontIndex = backIndex = -1;
		sizeVar--;
		return;
	}

	// Increment frontIndex cyclically
	frontIndex = (frontIndex + 1) % capacityVar;
	sizeVar--;
	return;
}

// Function to delete the element
// from the back of the deque
template <class X>
void Deque<X>::pop_back()
{
	// If deque is empty
	if (empty()) {
		std::cout << "Deque underflow" << std::endl;
		abort();
	}

	// If there is only one character
	if (frontIndex == backIndex) {

		// Mark deque as empty
		// and decrement sizeVar
		frontIndex = backIndex = -1;
		sizeVar--;
		return;
	}

	// Decrement backIndex cyclically
	backIndex = (backIndex - 1 + capacityVar) % capacityVar;
	sizeVar--;
	return;
}





template<class T>
class Deque_taska : public Deque<T>
{

public:
	void add_elems(Deque<std::string>& deque, const std::string& data)
	{
		deque.push_back(data);
	}

	std::string findDublicates(Deque<std::string>& deque)
	{
		std::vector<std::string> arr;

		std::string g;

		int size = deque.size();

		int counter = 0;

		while (deque.size() != 0)
		{
			arr.push_back(deque[counter]);
			counter++;
		}

		deque.clear();

		std::sort(arr.begin(), arr.end());

		arr.erase(std::unique(arr.begin(), arr.end()), arr.end());
		arr.clear();
		for (int i = 0; i < arr.size(); ++i)
		{
			std::cout << arr[i] << '\n';

			g += arr[i] + " ";

			deque.push_back(arr[i]);
		}
		return g;
	}

	void show_deck(Deque<std::string>& deque)
	{
		std::cout << "Your deck" << "\n";
		for (int i = 0; i < deque.size(); ++i)
		{
			std::cout << deque[i] << "\n";
		}
	}



};
int main() {

}