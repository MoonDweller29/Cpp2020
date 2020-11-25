#include <iostream>


template <typename ...>
struct are_same : public std::false_type {};

template <typename T>
struct are_same<T> : public std::true_type {};

template <typename T1, typename T2, typename ... Rest>
struct are_same<T1, T2, Rest...> {
    static constexpr bool value = std::is_same<std::decay_t<T1>, std::decay_t<T2>>::value
                               && are_same<T2, Rest...>::value;
};

template<typename ...T>
constexpr bool are_same_v = are_same<T...>::value;

int main(int argc, char const *argv[])
{
	bool value1 = are_same_v<int, const int32_t&, signed int, char>;
	std::cout << value1 << std::endl;

	bool value2 = are_same_v<int, const int32_t&, signed int>;
	std::cout << value2 << std::endl;

	static_assert(are_same_v<int, const int32_t&, signed int>, "MY compile assert");
	return 0;
}