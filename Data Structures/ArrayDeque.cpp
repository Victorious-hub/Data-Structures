
#include <memory>
#include <type_traits>
#include <iostream>
#include <string>
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


struct free_deleter {
	void operator()(void* p) const { free(p); }
};








template<class T>
class iterator
{
private:
	T* ptr;
	T* ptr_arr;
	int pos, j;
public:
	iterator() {}
	iterator(T* pt) : ptr(pt) {}

	iterator& operator++()
	{
		ptr++;
		return *this;
	}
	iterator operator++(int)
	{
		iterator tmp = *this;
		++(*this);
		return tmp;
	}

	iterator& operator--()
	{
		ptr--;
		return *this;
	}
	iterator operator--(int)
	{
		iterator tmp = *this;
		--(*this);
		return tmp;
	}

	iterator operator+(int n)
	{
		return iterator(ptr + n);
	}

	iterator operator-(int n)
	{
		return iterator(ptr - n);
	}
	iterator operator+=(int n)
	{
		return iterator(ptr += n);
	}

	iterator operator-=(int n)
	{
		return iterator(ptr -= n);
	}
	T& operator*()
	{
		return *ptr;
	}
	iterator begin();
	iterator end();
};



template<class T>
class Deque_A : public iterator<T>
{
public:
	//private:

	UniquePtr<UniquePtr<T[]>[]> arr;

	UniquePtr<T[]> head;
	UniquePtr<T[]> tail;
	UniquePtr<T[]>ptr_get;

	int front_pos, rear_pos;
	int size;
	int capacity = 3;
	int blocks;
	int K = capacity;
	int cols;
	int consider_block;
	int front_cols;
	int get_from_front;
	bool flag = false;

	Deque_A()
	{
		get_from_front = 1;
		consider_block = 0;
		cols = 0;

		front_cols = K - 1;

		blocks = capacity / 3;

		front_pos = -1;

		rear_pos = -1;

		size = 0;

		head = MakeUnique<T[]>(1);

		tail = MakeUnique<T[]>(1);

		arr = MakeUnique<UniquePtr<T[]>[]>(blocks);

		UniquePtr<T[]> A;


		for (int i = 0; i < blocks; ++i)
		{
			A = MakeUnique<T[]>(capacity);
			arr[i] = std::move(A);
		}
	}
public:
	int capacity_();
	int size_();
	bool empty();
	void push_back(const T& data);
	T get_front();
	T get_back();
	T& operator[](int index)
	{
		return ptr_get[index];
	}
	void push_front(const T& data);

	void allocate()
	{
		std::cout << "pass" << '\n';
	}

	void pop_back();

	void pop_front();


	iterator<T> begin()
	{
		output();
		return iterator<T>(&ptr_get[0]);
	}

	iterator<T> end()
	{
		output();
		return iterator<T>(&ptr_get[size - 1]);
	}

	void output()
	{
		UniquePtr<T[]>ptr = MakeUnique<T[]>(capacity);
		int counter = 0;
		for (int i = 0; i < blocks; ++i)
		{
			for (int g = 0; g < K; ++g)
			{
				ptr[counter] = arr[i][g];

				++counter;

			}
		}

		ptr_get = MakeUnique<T[]>(size);
		counter = 0;
		std::string str;
		for (int g = 0; g < capacity; ++g)
		{
			if (ptr[g] == head[0])
			{
				for (int k = g; k < capacity; ++k)
				{
					if (counter == size) break;
					ptr_get[counter] = ptr[k];
					str += ptr_get[counter] + " ";
					++counter;
				}
				break;
			}
		}

	}

	void resize(int value);

};

template <class T>
void Deque_A<T>::resize(int value)
{
	if (value > size)
	{
		while (true) {
			if (value % K != 0)
			{
				value++;
			}
			else break;
		}

		int new_blocks = (value - (blocks*K)) / K;




		int get_old_block = blocks;
		blocks += new_blocks;
		capacity = blocks * K;

		int counter = 0;
		UniquePtr<T[]> get_elems = MakeUnique<T[]>(get_old_block*capacity);
		for (int i = 0; i < get_old_block; ++i)
		{
			for (int g = 0; g < K; ++g)
			{
				get_elems[counter] = arr[i][g];

				++counter;
			}
		}
		arr.reset();
		counter = 0;

		arr = MakeUnique<UniquePtr<T[]>[]>(blocks);

		UniquePtr<T[]> A;

		for (int i = 0; i < blocks; ++i)
		{
			A = MakeUnique < T[] >(K);

			arr[i] = std::move(A);
		}



		for (int i = 0; i < blocks; ++i)
		{
			for (int g = 0; g < K; ++g)
			{
				arr[i][g] = get_elems[counter];
				++counter;
			}
		}
		get_elems.reset();

	}
	else if (value < size)
	{

		while (true) {
			if (value % K != 0)
			{
				value--;
			}
			else break;
		}


		blocks = (blocks*K - (capacity - value)) / K;
		int counter = 0;
		capacity = blocks * K;
		UniquePtr<T[]> get_elems = MakeUnique<T[]>(blocks*capacity);
		for (int i = 0; i < blocks; ++i)
		{
			for (int g = 0; g < K; ++g)
			{
				get_elems[counter] = arr[i][g];

				++counter;
			}
		}
		arr.reset();
		counter = 0;

		arr = MakeUnique<UniquePtr<T[]>[]>(blocks);

		UniquePtr<T[]> A;

		for (int i = 0; i < blocks; ++i)
		{
			A = MakeUnique < T[] >(K);

			arr[i] = std::move(A);
		}



		for (int i = 0; i < blocks; ++i)
		{
			for (int g = 0; g < K; ++g)
			{
				arr[i][g] = get_elems[counter];
				++counter;
			}
		}
		get_elems.reset();


		UniquePtr<T[]>ptr = MakeUnique<T[]>(capacity);
		counter = 0;
		for (int i = 0; i < blocks; ++i)
		{
			for (int g = 0; g < K; ++g)
			{
				ptr[counter] = arr[i][g];

				++counter;

			}
		}

		ptr_get = MakeUnique<T[]>(size);
		counter = 0;

		for (int g = 0; g < capacity; ++g)
		{
			if (ptr[g] == head[0])
			{

				size = capacity - g;
				break;
			}
		}
		
	}


}




template<class T>
void Deque_A<T>::pop_back()
{


	if (size == 0) {
		std::cout << "Deque undekhkhrflow" << std::endl;
		return;
	}

	if (front_pos == rear_pos && blocks == 1) {
		front_pos = rear_pos = -1;
		size--;
		return;
	}


	rear_pos = (rear_pos - 1 + capacity) % capacity;
	if (cols == 1)
	{
		cols = K;
		size--;
		return;
	}
	cols--;
	size--;
	return;

}

template<class T>
int Deque_A<T>::capacity_()
{
	return capacity;
}

template<class T>
int Deque_A<T>::size_()
{
	return size;
}

template <class X>
bool Deque_A<X>::empty()
{
	if (front_pos == -1 && rear_pos == -1)
		return true;
	else
		return false;
}
template <class T>
void Deque_A<T>::push_back(const T& data)
{


	if (size == 0)
	{

		arr[0][cols++] = data;

		head[0] = arr[0][0];

		tail[0] = arr[0][0];
		front_pos = rear_pos = 0;
		front_cols = -1;
		size++;
	}
	else
	{
		if (size == capacity || rear_pos / K == blocks - 1 && cols == 3)
		{

			cols = 0;
			int counter = 0;
			UniquePtr<T[]> get_elems = MakeUnique<T[]>(blocks*capacity);
			for (int i = 0; i < blocks; ++i)
			{
				for (int g = 0; g < K; ++g)
				{
					get_elems[counter] = arr[i][g];

					++counter;
				}
			}
			arr.reset();
			counter = 0;


			capacity += 3;
			blocks += 1;

			arr = MakeUnique<UniquePtr<T[]>[]>(blocks);

			UniquePtr<T[]> A;

			for (int i = 0; i < blocks; ++i)
			{
				A = MakeUnique < T[] >(K);
				arr[i] = std::move(A);
			}

			for (int i = 0; i < blocks; ++i)
			{
				for (int g = 0; g < K; ++g)
				{
					arr[i][g] = get_elems[counter];

					++counter;
				}
			}


			get_elems.reset();
			arr[blocks - 1][cols++] = data;
			tail[0] = arr[blocks - 1][cols - 1];
			rear_pos++;

			size++;

		}
		else {
			if (cols == K)
			{

				cols = 0;
				rear_pos++;
				arr[rear_pos / K][0] = data;
				tail[0] = arr[rear_pos / K][0];
				size++;
				cols++;
				return;
			}

			arr[rear_pos / K][cols++] = data;
			tail[0] = arr[rear_pos / K][cols - 1];


			rear_pos++;
			size++;



		}
	}

	for (int i = 0; i < blocks; ++i)
	{
		for (int g = 0; g < K; ++g)
		{
			std::cout << arr[i][g] << " ";
		}
	}
	std::cout << std::endl;

}

template <class T>
void Deque_A<T>::push_front(const T& data)
{
	if (size == 0)
	{
		arr[0][0] = data;

		head[0] = arr[0][0];

		tail[0] = arr[0][0];
		front_pos = rear_pos = get_from_front = 0;

		size++;
	}
	else
	{
		if (front_pos == 0)
		{

			int counter = 0;
			UniquePtr<T[]> get_elems = MakeUnique<T[]>(blocks*capacity);
			for (int i = 0; i < blocks; ++i)
			{
				for (int g = 0; g < K; ++g)
				{
					get_elems[counter] = arr[i][g];

					++counter;
				}
			}

			arr.reset();
			counter = 0;



			capacity += (K * 2);

			int get_prev_blocks = blocks;
			blocks += 2;

			arr = MakeUnique<UniquePtr<T[]>[]>(blocks);

			UniquePtr<T[]> A;

			for (int i = 0; i < blocks; ++i)
			{
				A = MakeUnique < T[] >(K);
				arr[i] = std::move(A);
			}


			for (int i = 1; i < get_prev_blocks + 1; ++i)
			{
				for (int g = 0; g < K; ++g)
				{
					arr[i][g] = get_elems[counter];
					++counter;
				}
			}


			rear_pos = (size + K - 1);

			front_pos = K - 1;
			front_cols = K - 1;

			get_from_front = K - 1;

			get_elems.reset();
			arr[0][front_cols--] = data;
			head[0] = arr[0][front_cols + 1];
			size++;

		}
		else
		{
			if (front_cols == -1)
			{
				front_cols = K - 1;
				front_pos--;
				arr[front_pos / K][front_cols--] = data;
				head[0] = arr[front_pos / K][front_cols + 1];

				size++;
				return;
			}

			arr[front_pos / K][front_cols--] = data;
			head[0] = arr[front_pos / K][front_cols + 1];
			front_pos--;
			get_from_front--;
			size++;
		}
	}



}

template <class T>
T Deque_A<T>::get_front()
{
	if (size == 0) {
		std::cout << "Deque underflow" << std::endl;
		abort();
	}



	return arr[front_pos / K][front_cols + 1];
}

template <class T>
T Deque_A<T>::get_back()
{
	if (size == 0) {
		std::cout << "Deque underflow" << std::endl;
		abort();
	}
	if (flag) rear_pos--;

	return arr[rear_pos / K][cols - 1];
}


template <class T>
void Deque_A<T>::pop_front()
{



	if (size == 0) {
		std::cout << "Deque undbbbberflow" << std::endl;
		abort();
	}


	if (front_pos == rear_pos && blocks == 1) {
		front_pos = rear_pos = -1;
		size--;
		return;
	}


	front_pos = (front_pos + 1) % capacity;
	if (front_pos%K == 0)
	{
		front_cols = -1;
		head[0] = arr[front_pos / K][front_cols + 1];
		size--;
		return;
	}
	front_cols++;
	size--;
	head[0] = arr[front_pos / K][front_cols + 1];
	return;


}

int main()
{

}