
#include <iostream>

template<typename T>
class WeakPtr;

template<typename T>
class SharedPtr {
public:
	using element_type = T;

	SharedPtr() : ptr_(nullptr), ref_count(nullptr) {}

	explicit SharedPtr(T* p) : ptr_(p), ref_count(new int(1)) {}

	SharedPtr(const SharedPtr<T>& other) : ptr_(other.ptr_), ref_count(other.ref_count) {
		if (ref_count) {
			++(*ref_count);
		}
	}

	SharedPtr(SharedPtr<T>&& other) : ptr_(nullptr), ref_count(nullptr) {
		swap(other);
	}

	SharedPtr& operator=(const SharedPtr& sp) noexcept {
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

	SharedPtr& operator=(SharedPtr&& sp) noexcept {
		if (ptr_ != sp.ptr_) {
			release();
			std::swap(ptr_, sp.ptr_);
			std::swap(ref_count, sp.ref_count);
		}
		return *this;
	}

	template <class Other>
	SharedPtr& operator=(const SharedPtr<Other>& sp) noexcept {
		release();
		ptr_ = sp.ptr_;
		ref_count = sp.ref_count;
		if (ref_count) {
			++(*ref_count);
		}
		return *this;
	}

	template <class Other>
	SharedPtr& operator=(SharedPtr<Other>&& sp) noexcept {
		release();
		std::swap(ptr_, sp.ptr_);
		std::swap(ref_count, sp.ref_count);
		return *this;
	}

	~SharedPtr() {
		release();
	}

	T* get() const noexcept {
		return ptr_;
	}

	bool owner_before(const SharedPtr<T>& other) const {
		return std::less<T*>()(ptr_, other.ptr_);
	}

	void reset(T* p = nullptr) {
		release();
		ptr_ = p;
		ref_count = new int(1);
	}

	void swap(SharedPtr<T>& other) {
		std::swap(ptr_, other.ptr_);
		std::swap(ref_count, other.ref_count);
	}

	bool unique() const {
		return use_count() == 1;
	}

	long use_count() const {
		return ref_count ? *ref_count : 0;
	}

	explicit operator bool() const noexcept {
		return ptr_ != nullptr;
	}

	T& operator*() const noexcept {
		return *ptr_;
	}

	T* operator->() const {
		return ptr_;
	}

	bool operator==(std::nullptr_t) const noexcept {
		return ptr_ == nullptr;
	}

	friend std::ostream& operator<<(std::ostream& os, const SharedPtr<T>& p) {
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

	template<typename U>
	explicit SharedPtr(const WeakPtr<U>& wp) : ptr_(wp.lock().get()), ref_count(wp.ref_count) {
		if (ref_count) {
			++(*ref_count);
		}
	}

	friend class WeakPtr<T>;

private:
	T* ptr_;
	int* ref_count;
};

template<typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
	return SharedPtr<T>(new T(std::forward<Args>(args)...));
}


template<typename T>
class WeakPtr {
public:
	WeakPtr() noexcept : ptr_(nullptr), ref_count(nullptr) {}

	explicit WeakPtr(const SharedPtr<T>& sp) noexcept : ptr_(sp.get()), ref_count(sp.ref_count) {}

	WeakPtr(const WeakPtr<T>& other) noexcept : ptr_(other.ptr_), ref_count(other.ref_count) {}

	WeakPtr(WeakPtr<T>&& other) noexcept : ptr_(nullptr), ref_count(nullptr) {
		swap(other);
	}

	WeakPtr& operator=(const SharedPtr<T>& sp) noexcept {
		ptr_ = sp.get();
		ref_count = sp.ref_count;
		return *this;
	}

	WeakPtr& operator=(const WeakPtr<T>& other) noexcept {
		ptr_ = other.ptr_;
		ref_count = other.ref_count;
		return *this;
	}

	WeakPtr& operator=(WeakPtr<T>&& other) noexcept {
		swap(other);
		return *this;
	}

	~WeakPtr() {}

		SharedPtr<T> lock() const noexcept {
		return SharedPtr<T>(*this);
	}

	void reset() noexcept {
		ptr_ = nullptr;
		ref_count = nullptr;
	}

	void swap(WeakPtr<T>& other) noexcept {
		std::swap(ptr_, other.ptr_);
		std::swap(ref_count, other.ref_count);
	}

	long use_count() const {
		return ref_count ? *ref_count : 0;
	}

	bool expired() const noexcept {
		return use_count() == 0;
	}

	bool owner_before(const WeakPtr<T>& other) const {
		return std::less<T*>()(ptr_, other.ptr_);
	}

	explicit operator bool() const noexcept {
		return ptr_ != nullptr;
	}

	T* get_ptr() {
		return ptr_;
	}

	friend class SharedPtr<T>;

private:
	T* ptr_;
	int* ref_count;
};
int main()
{

}