#include <iostream>
#include <string>
#include <time.h>

#include <boost/any.hpp>

struct Context {
    int connect_time;
    std::string third_svr_addr;
    std::string qihoo_cli_addr;
}; 

class TcpClient {
public:
    void set_ctx(const boost::any& ctx){
        ctx_ = ctx;
    }

    boost::any& ctx() {
        return ctx_;
    }

    const boost::any& ctx() const {
        return ctx_;
    }

    boost::any* mutable_ctx() {
        return &ctx_;
    }
private:
    boost::any ctx_;
};

int main(int argc, char* argv[])
{
    TcpClient vpn_client;

    Context ctx;
    ctx.connect_time   = time(NULL);
    ctx.third_svr_addr = std::string("8.8.8.8:53");
    ctx.qihoo_cli_addr = std::string("106.120.162.174:46006");

    vpn_client.set_ctx(ctx);

    sleep(2);

    Context* mutable_ctx = boost::any_cast<Context>(vpn_client.mutable_ctx());
    std::cout << "connect_time:"   << mutable_ctx->connect_time   << "\t now:" << time(NULL) << std::endl;
    std::cout << "third_svr_addr:" << mutable_ctx->third_svr_addr << std::endl;
    std::cout << "qihoo_cli_addr:" << mutable_ctx->qihoo_cli_addr << std::endl;

    return 0;
}
