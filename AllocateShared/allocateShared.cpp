#include <iostream>
#include <memory>
#include <functional>

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
	std::function<void *(size_t size)> op_new;
	std::function<void (void *p, size_t size)> op_del;

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
			op_new = nullptr;
			op_del = nullptr;
		}
	}

	template <typename U>
	Allocator (const Allocator<U>& other) noexcept {
		op_new = other.op_new;
		op_del = other.op_del;
	}

	T* allocate (size_t n) {
		// std::cout << "allocate " << n  << " Tsize " << sizeof(T) << std::endl;
		if (op_new) {
			return static_cast<T*>(op_new(n*sizeof(T)));
		}
		else {
			std::cout << "default new" << std::endl;
			return static_cast<T*>(::operator new(n*sizeof(T)));
		}
	}
	void deallocate (T* p, size_t n) {
		if (op_del) {
			op_del(p, n*sizeof(T));
		}
		else {
			std::cout << "default delete" << std::endl;
			::operator delete(p, n*sizeof(T));
		}
	}
};

template <class T, class U>
constexpr bool operator==(const Allocator<T>&, const Allocator<U>&) noexcept
{return true;}

int main(int argc, char **argv)
{
	Allocator<A> alloc;
	auto p_sh = std::allocate_shared<A>(alloc);
	p_sh.reset();

	auto int_p_sh = std::allocate_shared<int>(Allocator<int>());
	int_p_sh.reset();

	return 0;
}