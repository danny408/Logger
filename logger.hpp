#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <deque>
#include <mutex>
#include <fstream>
#include <stdarg.h>
#include <cstdio>
#include <memory>
#include <algorithm>

#define FLUSH_PATH "output_log.txt"

class string_logger {
private:
    uint32_t max_stored;
    std::deque<std::shared_ptr<char>> str_log;
    std::mutex mtx;

public:
    string_logger(uint32_t size) : max_stored(size) {
    }

    void insert(const char* event, ...) {
        va_list temp, args;
        va_start(args, event);
        va_copy(temp,args);
        int ret = vsnprintf(nullptr, 0, event, args);
        va_end(temp);
        char *buffer = new char[ret+1];
        vsnprintf(buffer, ret+1 , event, args);
        va_end(args);


        std::lock_guard<std::mutex> lock(mtx);
        if(str_log.size() == max_stored) {
            str_log.pop_back();
        }
        str_log.push_front(std::shared_ptr<char>(buffer));

    }

    std::vector<std::shared_ptr<char>> get_last_n(int n_strings) {
        std::vector<std::shared_ptr<char>> last_n_strings;
        if (n_strings < static_cast<int>(str_log.size())) {
            std::lock_guard<std::mutex> lock(mtx);
            for(int i = n_strings - 1; i >= 0; --i) {
                std::cout << "Pushing:" << str_log[i];
                last_n_strings.push_back(str_log[i]);
            }
        }
        return last_n_strings;
    }

    void flush_to_file() {
        std::lock_guard<std::mutex> lock(mtx);
        std::ofstream outfile;
        outfile.open(FLUSH_PATH, std::ios::ate | std::ios::out);
        while (!str_log.empty()) {
            outfile << str_log.back() << "\n";
            str_log.pop_back();
        }
        outfile.close();
    }


};

#endif // LOGGER_HPP
