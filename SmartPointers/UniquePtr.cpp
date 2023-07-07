#include<iostream>
#include<cstdlib>
#include<string>
#include<cstdio>
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
	bool operator!=(std::nullptr_t) const {
		return ptr_ == nullptr;
	}

	bool operator==(std::nullptr_t) const {
		return ptr_ == nullptr;
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

	bool operator!=(std::nullptr_t) const {
		return ptr_ == nullptr;
	}

	bool operator==(std::nullptr_t) const {
		return ptr_ == nullptr;
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
template <class T, size_t N>
UniquePtr<T[]> MakeUnique() {
	return UniquePtr<T[]>(new T[N]);
}

template <class T, class... Args>
UniquePtr<T> MakeUnique(Args&&... args) {
	return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

int main()
{
	UniquePtr<int> a = { MakeUnique<int>(124) };
	int* pointer = a.get();
	std::cout << *pointer<<std::endl;

	int var = 10;

	UniquePtr<int[]> b(new int[10]);

	for (int i = 0; i < 10; ++i) {
		b[i] = i + 10;
		std::cout << b[i] << '\n';
	}
	


}
