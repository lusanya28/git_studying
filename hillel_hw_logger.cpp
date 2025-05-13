#include <iostream>
#include <fstream>
#include <memory>

constexpr std::string_view c_log_file_name("app.log");

struct LogSink {
    virtual void write(const std::string& log)= 0;
    virtual ~LogSink() = default;
};

class ConsoleLogger : public LogSink {
public:
    void write(const std::string& log) override {
        std::cout << log << std::endl;
    }
};

class FileLogger : public LogSink {
public:
    void write(const std::string& log) override {
        std::ofstream log_file;
	log_file.open(c_log_file_name.data(), std::ios::app);
	log_file << log << "\n";
	log_file.close();
    }

};

class NoneLogger : public LogSink {
    void write(const std::string& log) {};
};

enum class LogType {
e_console,
e_file,
e_none
};

class Logger {
public:
    static Logger& instance() {
	static Logger instance;
        return instance;
    }
    void set_sink(LogType type) {
	switch (type) {
        case LogType::e_console:
            log_sink_ptr = std::make_unique<ConsoleLogger>();
	    break;
        case LogType::e_file:
	    log_sink_ptr = std::make_unique<FileLogger>();
	    break;
	case LogType::e_none:
	    log_sink_ptr = std::make_unique<NoneLogger>();
	    break;
	default:
	    break;
	}
    }
    void log(std::string msg) {
        log_sink_ptr->write(msg);
    }
private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    std::unique_ptr<LogSink> log_sink_ptr;

};

void DefaultLog() {
    std::cout << "Log type is missing or incorrect - log will be set to default 'console' type\n" <<
            "To set log type please run program with one of the parameter:\n" <<
            "'console' - will log to console,\n" <<
            "'file' - will log to " << c_log_file_name.data() << " file\n" <<
            "'none' - to run program without log\n";
}

LogType ParseLogType(int argc, char* argv[]) {
    if(argc < 2) {
        DefaultLog();
	return LogType::e_console;
    }

    std::string string_reg = argv[1];
    
    for (auto& c : string_reg) {
        c = std::tolower(c);
    }

    if(string_reg == "console") {
        return LogType::e_console;
    }

    if(string_reg == "file") {
        return LogType::e_file;
    }

    if(string_reg == "none") {
        return LogType::e_none;
    }
    
    DefaultLog();
    return LogType::e_console;
}

int main(int argc, char* argv[]) {

    LogType log_type = ParseLogType(argc, argv);

    auto& logger = Logger::instance();
    logger.set_sink(log_type);
    logger.log("Test 1");
    return 0;
}
