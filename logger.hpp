#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <deque>
#include <mutex>
#include <fstream>
#include <stdarg.h>
#include <cstdio>
#include <memory>
#include <algorithm>
#include <vector>


struct log_message{
    uint32_t priority;
    std::shared_ptr<char> message;
};

class string_logger {
private:
    uint32_t max_stored;
    std::deque<log_message> str_log;
    std::mutex mtx;

public:
    string_logger(uint32_t size) : max_stored(size) {
    }

    void insert(uint32_t priority, const char* event, ...) {
        va_list temp, args;
        va_start(args, event);
        va_copy(temp,args);
        int ret = vsnprintf(nullptr, 0, event, args);
        va_end(temp);
        char *buffer = new char[ret+1];
        vsnprintf(buffer, ret+1 , event, args);
        va_end(args);

        log_message msg;
        msg.priority = priority;
        msg.message = std::shared_ptr<char>(buffer);

        std::lock_guard<std::mutex> lock(mtx);
        if(str_log.size() == max_stored) {
            str_log.pop_back();
        }
        str_log.push_front(msg);

    }

    std::vector<std::shared_ptr<char>> tail(uint32_t n_strings, uint32_t priority = 0) {
        std::vector<std::shared_ptr<char>> last_n_strings;
        if (n_strings < static_cast<int>(str_log.size())) {
            priority = (priority) ? priority : UINT32_MAX;
            std::lock_guard<std::mutex> lock(mtx);
            for(int i = n_strings - 1; i >= 0; --i) {
                if(str_log[i].priority < priority){
                    last_n_strings.push_back(str_log[i].message);
                }
            }
        }
        return last_n_strings;
    }

    void flush_to_file(std::string ofile) {
        std::lock_guard<std::mutex> lock(mtx);
        std::ofstream outfile;
        outfile.open(ofile, std::ios::ate | std::ios::out);
        while (!str_log.empty()) {
            outfile << str_log.back().message << "\n";
            str_log.pop_back();
        }
        outfile.close();
    }


};

#endif // LOGGER_HPP
