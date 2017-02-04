#include <iostream>
#include <logger.hpp>


int main()
{

    string_logger strlog(1000);
    strlog.insert("Hello1");
    strlog.insert("Hello2");
    strlog.insert("Hello3");
    strlog.insert("Hello4");
    strlog.insert("Hello5");


    std::vector<shared_ptr<char>> a = strlog.get_last_n(2);

    std::cout << "Contents:\n";
    for(std::shared_ptr<char> str: a){
        std::cout << str << "\n";
    }

    strlog.flush_to_file();

    return 0;
}
