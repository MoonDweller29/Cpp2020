#include <iostream>
#include <memory>
#include <exception>
#include <string>

class IValue {
public:
    virtual IValue *copy() const = 0;
    virtual ~IValue(){}
};


template <typename T>
class TValue : public IValue {
public:
    TValue(const T& v): value(v) {}
    
    T &get() { return value; }
    
    virtual IValue *copy() const override {
        return new TValue(*this);
    }

private:
    T value;
};


class Any {
public:
    template<typename T>
    Any(T&& val) : val(new TValue<std::decay_t<T>>(val)) {}
    Any(const Any &other) : val(other.val->copy()) {}

    Any &operator=(const Any &other) {
        val.reset(other.val->copy());
        return *this;
    }
    
    template <typename T>
    std::decay_t<T> &get() {
        using RawType = std::decay_t<T>;
        // try {
        TValue<RawType> &tRef = dynamic_cast<TValue<RawType>&>(*val);

        return tRef.get();
    }

private:
    std::unique_ptr<IValue> val;
};

int main(int argc, char const *argv[])
{
    const char* str = "static";
    Any obj1(15);
    Any obj2("anime");
    Any obj3(str);

    std::cout << obj1.get<int>() << std::endl;
    try {
        obj1.get<long>();
    }
    catch(...) {
        std::cout << "get long failed\n";
    }

    obj1 = obj2;
    std::cout << obj1.get<const char*>() << std::endl;
    try {
        std::cout << obj1.get<char*>() << std::endl;
    }
    catch(...) {
        std::cout << "get char* failed\n";
    }

    obj1 = obj3;
    std::cout << obj1.get<const char*>() << std::endl;

    
    return 0;
}