#ifndef __OPTION_H_
#define __OPTION_H_

#include <iostream>

namespace util {

class Option {
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
    Option(Option& opt) {}
    Option& operator=(Option& opt) {}

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

} // end of namespace util



