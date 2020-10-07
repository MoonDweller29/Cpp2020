#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <tuple>


class Printer
{
    std::stringstream sstream;

    template <typename T>
    static auto is_range_based_container(const T &object)
        -> decltype(object.begin(), object.end(), std::true_type());

    template <typename ...>
    static auto is_range_based_container(...)
        -> std::false_type;

    template <typename T>
    static char open_bracket(const std::vector<T> &v) { return '['; }

    template <typename T>
    static char close_bracket(const std::vector<T> &v) { return ']'; }

    template <typename T>
    static char open_bracket(const std::set<T> &s) { return '{'; }

    template <typename T>
    static char close_bracket(const std::set<T> &s) { return '}'; }

public:
    std::string str() const { return sstream.str(); }

    template<size_t I = 0, typename... Args>
    Printer &format(const std::tuple<Args...>& t) {
        if constexpr (I == 0)
            sstream << "( ";
        format(std::get<I>(t));
        if constexpr(I+1 != sizeof...(Args))
        {
            sstream << ", ";
            format<I+1>(t);
        }
        else
        {
            sstream << " )";
        }

        return *this;
    }

    template<typename T1, typename T2>
    Printer &format(const std::pair<T1, T2>& pair) {
        sstream << "( ";
        format(std::get<0>(pair));
        sstream << ", ";
        format(std::get<1>(pair));
        sstream << " )";

        return *this;
    }

    Printer &format(const std::string &s)
    {
        sstream << s;
        return *this;
    }

    template<typename T>
    Printer &format(const T& object)
    {
        if constexpr (decltype(is_range_based_container(object))::value)
        {
            sstream << open_bracket(object) << " ";
            auto i = object.begin();
            format(*i);
            ++i;
            for (; i != object.end(); ++i)
            {
                sstream << ", ";
                format(*i);
            }
            sstream << " " << close_bracket(object);
        }
        else
        {
            sstream << object;
        }
        
        return *this;
    }
};

template<typename T>
std::string format(const T& t) {
    return Printer().format(t).str();
}

int main(int argc, char **argv)
{
    // basic examples
    std::cout << "\nbasic examples \n";
    std::tuple<std::string, int, int> t = {"xyz", 1, 2};
    std::vector<std::pair<int, int> > v = {{1, 4}, {5, 6}};

    std::cout << Printer().format(" vector: ").format(v).str() << std::endl;
    std::cout << Printer().format(t).format(" ! ").format(0).str() << std::endl;

    // extra examples
    std::cout << "\nexamples \n";
    std::pair<int, std::string> k(1,"second");
    std::cout << Printer().format("pair: ").format(k).str() << std::endl;

    std::set<std::tuple<int, std::string, int>> s {
        {1, "2", 3},
        {1, "second", 3}
    };

    std::cout << Printer().format("set: ").format(s).str() << std::endl;

    std::tuple<int, int, std::vector<char>> tuple{1, 2, {'v', 'e', 'c'}};
    std::cout << Printer().format("tuple: ").format(tuple).str() << std::endl;


    return 0;
}