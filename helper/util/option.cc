#include "option.h"

namespace util {

Option Option::instance_;

Option::Option()
    :kill_(false), reload_(false), foreground_(false)
{
}

Option::~Option() {}

bool Option::Init(int argc, char** argv) 
{
    int opt;
    while((opt = getopt(argc, argv, "c:l:p:krfhv")) != -1) {            
        switch(opt) {
            case 'c':                                 
                cfg_file_ = std::string(optarg);
                break;
            case 'l':                                     
                log_dir_  = std::string(optarg);
                break;
            case 'p':                                     
                pid_file_ = std::string(optarg);
                break;
            case 'k':
                kill_ = true;
                break;
            case 'r':
                reload_ = true;
                break;
            case 'f':
                foreground_ = true;
                break; 
            case 'h':
                //显示使用该程序的帮助信息。
                std::cout << "./noncopyable -c \"/home/s/etc/cfg.ini\" -l \"/home/s/var/log\" -p \"/home/s/var/pid.pid\"" << std::endl;
                break;
            case 'v':
                //显示该程序的版本信息。           
                break;
            default:
                //不是参数列表中的参数的处理。
                break;
        }
    }
    return true;
}

} // end of namespace util

#ifdef _TEST_

int main(int argc, char*argv[]) 
{
    if (!util::Option::instance().Init(argc, argv)) {
        std::cerr << "Option::instance failed";
        return 1;
    }

    std::cout << "log_dir="  << util::Option::instance().log_dir()  << std::endl;
    std::cout << "cfg_file=" << util::Option::instance().cfg_file() << std::endl;
    std::cout << "pid_file=" << util::Option::instance().pid_file() << std::endl;

    if (util::Option::instance().kill()) {
        std::cout << "kill" << std::endl;
    }

    if (util::Option::instance().reload()) {
        std::cout << "reload" << std::endl;
    }


    if (util::Option::instance().foreground()) {
        std::cout << "foreground" << std::endl;
    }

    return 0;
}

#endif
