#include <iostream>
#include <memory>

class A {
public:
	static void * operator new(size_t size) {
		std::cout << "operator new!" << std::endl;
		return ::operator new(size);
	}
	static void operator delete(void *p, size_t size) {
		std::cout << "operator delete!" << std::endl;
		return ::operator delete(p);
	}
};

template <typename T>
class Allocator {
	void *(*op_new)(size_t size);
	void (*op_del)(void *p, size_t size);

	template <typename C>
    static auto has_custom_new_op(const Allocator<C>&)
    	-> decltype(C::operator new, std::true_type());

    template <typename ...>
    static auto has_custom_new_op(...)
        -> std::false_type;

public:
	typedef T value_type;

	template<typename> friend class Allocator;

	Allocator() noexcept {
		if constexpr (decltype(has_custom_new_op(*this))::value) {
			op_new = T::operator new;
			op_del = T::operator delete;
		} else {
			op_new = ::operator new;
			op_del = ::operator delete;
		}
	}

	template <typename U>
	Allocator (const Allocator<U>& other) noexcept {
		op_new = other.op_new;
		op_del = other.op_del;
	}

	T* allocate (size_t n) {
		// std::cout << "allocate " << n  << " Tsize " << sizeof(T) << std::endl;
		return static_cast<T*>(op_new(n*sizeof(T)));
	}
	void deallocate (T* p, size_t n) {
		op_del(p, n*sizeof(T));
	}

	template <class T1, class T2>
	friend constexpr bool operator==(const Allocator<T1>&, const Allocator<T2>&) noexcept;
};

template <class T, class U>
constexpr bool operator==(const Allocator<T>& al1, const Allocator<U>& al2) noexcept
{ return al1.op_new == al2.op_new; }

int main(int argc, char **argv)
{
	Allocator<A> alloc;
	auto p_sh = std::allocate_shared<A>(alloc);
	p_sh.reset();

	auto int_p_sh = std::allocate_shared<int>(Allocator<int>());
	int_p_sh.reset();

	return 0;
}