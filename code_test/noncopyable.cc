#include <boost/noncopyable.hpp>
#include <iostream>

class Option : boost::noncopyable {
public:
    Option();
    ~Option();

    bool Init(int argc, char** argv);

    bool kill() const { return kill_; }
    bool reload() const { return reload_; }
    bool foreground() const { return foreground_; }
    const std::string& log_dir() const { return log_dir_; }
    const std::string& pid_file() const { return pid_file_; }
    const std::string& cfg_file() const { return cfg_file_; }

    static Option& instance() { return instance_; }
private:
    // Options only init with cmdline.
    bool kill_;
    bool reload_;
    bool foreground_;
    std::string log_dir_;
    std::string pid_file_;
    std::string cfg_file_;

    static Option instance_;
};

//.cc

Option Option::instance_;

Option::Option()
    :kill_(false), reload_(false), foreground_(false)
{
}

Option::~Option() {}

bool Option::Init(int argc, char** argv) 
{
    int opt;
    while( (opt = getopt(argc,argv,"c:l:p:krfhv")) != -1 ) 
    {            
        switch(opt)
        {
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

int main(int argc, char*argv[]) 
{
    if (!Option::instance().Init(argc, argv)) {
        std::cerr << "Option::instance failed";
        return 1;
    }

    std::cout << "log_dir="  << Option::instance().log_dir()  << std::endl;
    std::cout << "cfg_file=" << Option::instance().cfg_file() << std::endl;
    std::cout << "pid_file=" << Option::instance().pid_file() << std::endl;

    if (Option::instance().kill()) {
        std::cout << "kill" << std::endl;
    }

    if (Option::instance().reload()) {
        std::cout << "reload" << std::endl;
    }


    if (Option::instance().foreground()) {
        std::cout << "foreground" << std::endl;
    }

    return 0;
}


