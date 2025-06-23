#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <cctype>
#include <algorithm>

void Usage() {
    std::cout << "Usage: \n" << "program_name " <<
                "filter filename.\n" <<
                "Possible filters:\n" <<
                "    even - even numbers\n" <<
                "    odd  - odd numbers\n" <<
                "    gt<n> - numbers greater then n\n";
}

struct INumFilter {
    virtual ~INumFilter() = default;
    virtual bool filter(const int num_to_filter) = 0;
};

struct EvenNumFilter : public INumFilter {
    bool filter(const int num_to_filter) override {
        return (num_to_filter % 2 == 0);
    }
};

struct OddNumFilter : public INumFilter {
    bool filter(const int num_to_filter) override {
        return (num_to_filter % 2 != 0);
    }
};

class GreatThanNumFilter : public INumFilter {
private:
    int lower_limit{0};

public:
    GreatThanNumFilter(int l_limit) : lower_limit(l_limit) {}
    bool filter(const int num_to_filter) override {
        return (num_to_filter > lower_limit);
    }
};

template<typename T> // With template implementation of base class it is ease to add registrations for different classes
struct IFactoryRegister {
    virtual std::unique_ptr<T> create(const std::string& filter_type) = 0;
    virtual ~IFactoryRegister() = default;
};

class FilterFactoryRegister : public IFactoryRegister<INumFilter> { // We also may create just one template for inherited classes
private:
    using Factory = std::function<std::unique_ptr<INumFilter>()>;
    std::map<std::string, Factory> filter_creator;

    FilterFactoryRegister() = default;
    FilterFactoryRegister(const FilterFactoryRegister& other) = delete;
    FilterFactoryRegister& operator=(const FilterFactoryRegister& other) = delete;

public:
    static FilterFactoryRegister& instance() {
        static FilterFactoryRegister reg;
        return reg;
    }
    
    void register_factory(const std::string& filter_type, Factory f) {
        this->filter_creator[filter_type] = f;
    }
    
    std::unique_ptr<INumFilter> create(const std::string& filter_type) override {
        const auto it = this->filter_creator.find(filter_type);
        if (it == filter_creator.cend()) {
            return nullptr;
        }
        return it->second();
    }
};

/* Тут я намагався створити парсер для даних будь-якого типу(щоб повернути вектор типу T) - у мене проблема що я часто намагаюся зробити щось зайве
template<typename T>
class IDataReader {
public:
    IDataReader(const std::string& file_to_read) : data_file(file_to_read) {
        std::ifstream ifs(file_to_read);
        if (!ifs.is_open()) {
                std::cerr << "ERROR! Couldn't open file " << file_to_read << " for reading." <<
                "\nPlease check file name or if file exist" << "\n";
                return;
            }
        T data;
        while (ifs >> data) {
            processed_data_vec.emplace_back(data);
        }
    }
    
    const std::vector<std::unique_ptr<T>>& get_user_data() const {
        return processed_data_vec;
    }
    
    void reload_data() {
        std::ifstream ifs(data_file);
        if (!ifs.is_open()) {
                std::cerr << "ERROR! Couldn't open file " << data_file << " for reading." <<
                "\nData wasn't relaod from file" << "\n";
                return;
            }
        std::vector<std::unique_ptr<T>>{}.swap(processed_data_vec);
        T data;
        while (ifs >> data) {
            processed_data_vec.emplace_back(data);
        }
    }
    
private:
    std::string data_file{};
    std::vector<std::unique_ptr<T>> processed_data_vec;

};
*/

struct INumReader {
    std::vector<int> parse(const std::string& file_to_read) {
        std::vector<int> data_vector;

        std::ifstream ifs(file_to_read);
        if(!ifs.is_open()) {
            Usage();
            throw std::runtime_error("Failed to open file: " + file_to_read);
        }
        int data;
        while(ifs >> data) {
            data_vector.emplace_back(data);
        }
        return data_vector;
    }
};

struct INumberObserver {
    virtual ~INumberObserver() = default;
    virtual void on_number(int number) = 0;
    virtual void on_finished() = 0;
};

struct PrintObserver : public INumberObserver {
    void on_number(int number) override {
        std::cout << number << "\n";
    }
    
    void on_finished() override {
        std::cout << "Filtering of numbers from file finished !" << "\n";
    }
};

class CountObserver : public INumberObserver {
private:
    int count = 0;
    
public:
    void on_number(int number) override {
        count++;
    }
    
    void on_finished() override {
        std::cout << "Total numbers processed: " << count << "\n";
    }
};

class NumberProcessor {
private:
    INumReader& reader;
    INumFilter& filter;
    std::vector<INumberObserver*> observers;
    
public:
    NumberProcessor(INumReader& reader, 
                   INumFilter& filter, 
                   const std::vector<INumberObserver*>& observers)
        : reader(reader), filter(filter), observers(observers) {}
    
    void run(const std::string& filename) {

        std::vector<int> numbers = reader.parse(filename);
        
        if (numbers.empty()) {
            std::cout << "No numbers found in file: " << filename << std::endl;
            notify_finished();
            return;
        }
        
        for (int number : numbers) {
            if (filter.filter(number)) {
                notify_number(number);
            }
        }
        
        notify_finished();
    }
    
private:
    void notify_number(int number) {
        for (auto* observer : observers) {
            observer->on_number(number);
        }
    }
    
    void notify_finished() {
        for (auto* observer : observers) {
            observer->on_finished();
        }
    }
};

int main(int argc, char* argv[]) {
    if(argc != 3) {
        Usage();
        return -1;
    }
    
    FilterFactoryRegister& filter_registry = FilterFactoryRegister::instance();
    
    filter_registry.register_factory("EVEN", []() {
        return std::make_unique<EvenNumFilter>();
    });
    filter_registry.register_factory("ODD", []() {
        return std::make_unique<OddNumFilter>();
    });
    
    std::string filter_name = argv[1];
    std::string filename = argv[2];
    
    for (auto& c : filter_name) {
        c = std::toupper(c);
    }
    
    INumReader reader;
    
    PrintObserver print_observer;
    CountObserver count_observer;
    
    std::vector<INumberObserver*> observers = {
        &print_observer,
        &count_observer
    };
    
    std::unique_ptr<INumFilter> filter_ptr;
    
    if (filter_name.substr(0, 2) == "GT") {
        int threshold = 0;
        try {
            threshold = std::stoi(filter_name.substr(2));            
        } catch (const std::exception& e) {
            Usage();
            throw std::runtime_error("Error parsing GT filter: " + std::string(e.what()));
        }
        filter_name = "GT";
        filter_registry.register_factory(filter_name, [threshold]() {
            return std::make_unique<GreatThanNumFilter>(threshold); 
        });
    } 
    filter_ptr = filter_registry.create(filter_name);

    
    if (!filter_ptr) {
        std::cerr << "Unknown filter: " << filter_name << std::endl;
        Usage();
        return -1;
    }
    
    NumberProcessor processor(reader, *filter_ptr, observers);
    processor.run(filename);
    
    return 0;
}
