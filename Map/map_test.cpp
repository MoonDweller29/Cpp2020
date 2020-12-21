#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>


class Clock {
    std::chrono::high_resolution_clock::time_point m_start;
public:
    Clock();

    double restart();
    double getTime() const;

    static double seconds(long long microseconds);
};

Clock::Clock() : m_start() {
    restart();
}

double Clock::restart() {
    auto now = std::chrono::high_resolution_clock::now();
    auto before = m_start;
    m_start = now;
    auto dur = now - before;
    auto mcs = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();

    return seconds(mcs);
}

double Clock::getTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto dur = now - m_start;
    auto mcs = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();

    return seconds(mcs);
}

double Clock::seconds(long long microseconds) {
    return static_cast<double>(microseconds) * 1e-6;
}


class FakeDatabase {
public:
    FakeDatabase(int name_len):
        name_len(name_len),
        size(pow(CHAR_COUNT, name_len))
    {
        names.reserve(size);
        std::string s;
        for (int i = 0; i < name_len; ++i) {
            s = s + "A";
        }

        for (int id = 0; id < size; ++id) {
            names.push_back(s);
            inc_str(s);
        }
    }
    
    const std::string &getName(int id) const {
        return names.at(id);
    }

    const int len() const { return size; }
    void print(int lines) const {
        std::cout << "=============================\n";
        std::cout << "Dataset len = " << size << std::endl;
        for (int i = 0; i < lines; ++i) {
            std::cout << names[i] << std::endl;
        }

        std::cout << "..."<< std::endl;
        for (int i = 0; i < lines; ++i) {
            std::cout << names[size - lines + i] << std::endl;
        }       
        std::cout << "=============================\n";
    }
private:
    const int CHAR_COUNT = 'Z' + 1 - 'A';
    int name_len;
    int size;
    std::vector<std::string> names;

    void inc_str(std::string &s) {
        for (int i = name_len - 1; i >= 0; --i) {
            s[i] = (s[i] - 'A' + 1)%(CHAR_COUNT) + 'A';
            if (s[i] != 'A') {
                return;
            }
        }
    }
};

std::map<std::string, int> create_map(const FakeDatabase &database) {
    std::map<std::string, int> names_id;
    int len = database.len();
    for (int i = 0; i < len; ++i)
    {
        names_id.insert({database.getName(i), i});
    }

    return names_id;
}

std::map<std::string, int> create_map_FAST(const FakeDatabase &database) {
    std::map<std::string, int> names_id;
    
    int len = database.len();
    auto iter = names_id.begin();
    for (int i = 0; i < len; ++i)
    {
        iter = names_id.insert(iter, {database.getName(i), i});
    }

    return names_id;
}

std::map<std::string, int> create_map_from_reverse(const FakeDatabase &database) {
    std::map<std::string, int> names_id;
    
    int len = database.len();
    auto iter = names_id.begin();
    for (int i = len-1; i >= 0; --i)
    {
        iter = names_id.insert(iter, {database.getName(i), i});
    }

    return names_id;
}

double check(
    const FakeDatabase &database,
    std::map<std::string, int> dict
) {
    int len = database.len();
    Clock clock;
    clock.restart();

    for (int i = 0; i < len; ++i) {
        if (dict[database.getName(i)] != i) {
            std::cout << "ERROR\n";
        }
    }

    return clock.getTime();
}

int main(int argc, char **argv) {
    FakeDatabase database(3);
    database.print(3);
    
    Clock clock;

    std::map<std::string, int> dict1, dict2, dict3;

    clock.restart();
    for (int i = 0; i < 1000; ++i) {
        dict1 = create_map(database);
    }
    double time1 = clock.getTime();
    std::cout << "Insert time basic = " << time1 << std::endl;

    clock.restart();
    for (int i = 0; i < 1000; ++i) {
        dict2 = create_map_FAST(database);
    }
    double time2 = clock.getTime();
    std::cout << "Insert time improved = " << time2 << std::endl;

    clock.restart();
    for (int i = 0; i < 1000; ++i) {
        dict3 = create_map_from_reverse(database);
    }
    double time3 = clock.getTime();
    std::cout << "Insert time from reverced list = " << time3 << std::endl;

    std::cout << "Сheck basic map, time = " << check(database, dict1) << std::endl;
    std::cout << "Check improved map, time = " << check(database, dict2) << std::endl;
    std::cout << "Check improved map from reverced, time = " << check(database, dict3) << std::endl;


    return 0;
}