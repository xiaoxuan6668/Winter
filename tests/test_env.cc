#include "../winter/env.h"
#include <unistd.h>
#include <iostream>
#include <fstream>

struct A {
    A() {
        std::ifstream ifs("/proc/" + std::to_string(getpid()) + "/cmdline", std::ios::binary);
        std::string content;
        content.resize(4096);

        ifs.read(&content[0], content.size());
        content.resize(ifs.gcount());

        for(size_t i = 0; i < content.size(); ++i) {
            std::cout << i << " - " << content[i] << " - " << (int)content[i] << std::endl;
        }
    }
};

A a;

int main(int argc, char** argv) {
    std::cout << "argc=" << argc << std::endl;
    winter::EnvMgr::GetInstance()->addHelp("s", "start with the terminal");
    winter::EnvMgr::GetInstance()->addHelp("d", "run as daemon");
    winter::EnvMgr::GetInstance()->addHelp("p", "print help");
    if(!winter::EnvMgr::GetInstance()->init(argc, argv)) {
        winter::EnvMgr::GetInstance()->printHelp();
        return 0;
    }

    std::cout << "exe=" << winter::EnvMgr::GetInstance()->getExe() << std::endl;
    std::cout << "cwd=" << winter::EnvMgr::GetInstance()->getCwd() << std::endl;

    std::cout << "path=" << winter::EnvMgr::GetInstance()->getEnv("PATH", "xxx") << std::endl;
    std::cout << "test=" << winter::EnvMgr::GetInstance()->getEnv("TEST", "") << std::endl;
    std::cout << "set env " << winter::EnvMgr::GetInstance()->setEnv("TEST", "yy") << std::endl;
    std::cout << "test=" << winter::EnvMgr::GetInstance()->getEnv("TEST", "") << std::endl;
    if(winter::EnvMgr::GetInstance()->has("p")) {
        winter::EnvMgr::GetInstance()->printHelp();
    }
    return 0;
}