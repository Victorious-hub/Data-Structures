#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <memory>
using namespace std;

template<class T>
class Shared_ptr
{
	T* m_ptr;
	uint32_t *m_refCount;
public:
	Shared_ptr(T* ptr = nullptr) :m_ptr(ptr)
	{
		if (m_ptr)
			m_refCount = new uint32_t(1);
		else
			m_refCount = nullptr;
	}

	Shared_ptr(T* data, std::default_delete<T>& dl) : m_ptr(data) {}

	~Shared_ptr()
	{
		if (m_refCount != nullptr) {
			(*m_refCount)--;
			if ((*m_refCount) == 0) {
				delete m_ptr;
				delete m_refCount;
			}
		}
	}


	Shared_ptr(const Shared_ptr& a)
	{
		m_ptr = a.m_ptr;
		m_refCount = a.m_refCount;
		(*m_refCount)++;
	}
	Shared_ptr& operator=(const Shared_ptr& a)
	{
		m_ptr = a.m_ptr;
		m_refCount = a.m_refCount;
		(*m_refCount)++;
		return *this;
	}

	Shared_ptr(Shared_ptr&& a) : m_ptr(a.m_ptr), m_refCount(a.m_refCount)
	{
		a.m_ptr = nullptr;
		a.m_refCount = nullptr;
	}

	Shared_ptr& operator=(Shared_ptr&& a)
	{
		if (&a == this)
			return *this;

		delete m_ptr;
		delete m_refCount;

		m_ptr = a.m_ptr;
		a.m_ptr = nullptr;

		m_refCount = a.m_refCount;
		a.m_refCount = nullptr;

		return *this;
	}

	Shared_ptr(std::weak_ptr<T>& x)
	{
		m_ptr = x.m_ptr;
		m_refCount = x.m_refCount;
		(*m_refCount)++;
	};

	Shared_ptr& operator=(std::weak_ptr<T>& x)
	{
		m_ptr = x.m_ptr;
		m_refCount = x.m_refCount;
		(*m_refCount)++;
		return *this;
	}


	T& operator*() const { return *m_ptr; }
	T* operator->() const { return m_ptr; }


	void swap(Shared_ptr& src) noexcept
	{
		std::swap(m_ptr, src.m_ptr);
	}

	int unique()
	{
		if (use_count() > 1)
		{
			return 0;
		}
		return 1;
	}

	T* get()const { return m_ptr; }

	T* release() noexcept
	{
		T* result = nullptr;
		std::swap(result, m_ptr);
		return result;
	}

	void reset()
	{
		T* tmp = release();
		delete tmp;
	}


	bool operator==(std::nullptr_t) const {
		return m_ptr == nullptr;
	}

	bool operator!=(const Shared_ptr& src) const {
		return m_ptr != src.m_ptr;
	}

	bool operator==(const Shared_ptr& src) const {
		return m_ptr == src.m_ptr;
	}

	uint32_t use_count()
	{
		return *m_refCount;
	}
	friend std::ostream& operator<<(std::ostream& os, Shared_ptr<T>& sp)
	{
		os << "Address pointed : " << sp.get() << std::endl;
		return os;
	}

};


template<class T>
class Shared_ptr<T[]>
{
	T* m_ptr;
	uint32_t *m_refCount;
public:

	Shared_ptr() {}

	explicit Shared_ptr(T *m_ptr) {
		Shared_ptr<T[]>::m_ptr = m_ptr;
		m_refCount = new uint32_t(1);
	}

	Shared_ptr(const Shared_ptr<T[]>& a)
	{
		m_ptr = a.m_ptr;
		m_refCount = a.m_refCount;
		(*m_refCount)++;
	}


	Shared_ptr(Shared_ptr<T[]>&& a) : m_ptr(a.m_ptr), m_refCount(a.m_refCount)
	{
		a.m_ptr = nullptr;
		a.m_refCount = nullptr;
	}


	Shared_ptr<T[]>& operator=(const Shared_ptr<T[]>& a)
	{
		m_ptr = a.m_ptr;
		m_refCount = a.m_refCount;
		(*m_refCount)++;
		return *this;
	}


	Shared_ptr<T[]>& operator=(Shared_ptr<T[]>&& a)
	{
		if (&a == this)
			return *this;

		delete m_ptr;
		delete m_refCount;

		m_ptr = a.m_ptr;
		a.m_ptr = nullptr;

		m_refCount = a.m_refCount;
		a.m_refCount = nullptr;

		return *this;
	}





	~Shared_ptr()
	{
		if (m_refCount != nullptr) {
			(*m_refCount)--;
			if ((*m_refCount) == 0) {
				delete[] m_ptr;
				delete m_refCount;
			}
		}
	}





	T& operator*() const { return *m_ptr; }
	T* operator->() const { return m_ptr; }


	void swap(Shared_ptr<T[]>& src) noexcept
	{
		std::swap(m_ptr, src.m_ptr);
	}

	int unique()
	{
		if (use_count() > 1)
		{
			return 0;
		}
		return 1;
	}

	T* get()const { return m_ptr; }

	T* release() noexcept
	{
		T* result = nullptr;
		std::swap(result, m_ptr);
		return result;
	}

	void reset()
	{
		T* tmp = release();
		delete tmp;
	}
	T& operator[](int pos) const {
		return Shared_ptr<T[]>::m_ptr[pos];
	}
	uint32_t use_count()
	{
		return *m_refCount;
	}

};
template <class T, class ...Args>
std::enable_if_t<!std::is_array_v<T>, Shared_ptr<T>>
Make_shared(Args&& ...args)
{
	return Shared_ptr<T>(new T(std::forward<Args>(args)...));
};

template <class T>
std::enable_if_t<std::is_array_v<T>, Shared_ptr<T>>
Make_shared(int size)
{
	using type = std::remove_extent_t<T>;
	return Shared_ptr<T>(new type[size]);
};
int main()
{

}