#include <filesystem>
#include <string>
#include <vector>
#include <unistd.h>

int main(int argc, char** argv)
{
    namespace fs = std::filesystem;

    fs::path executable = fs::canonical("/proc/self/exe");
    fs::path baseDir = executable.parent_path();
    fs::path launcher = baseDir / "XylarJavaLauncher";

    std::vector<std::string> argStorage;
    std::vector<char*> args;
    argStorage.emplace_back(launcher.string());
    args.push_back(argStorage.back().data());

    for (int i = 1; i < argc; ++i)
    {
        argStorage.emplace_back(argv[i]);
        args.push_back(argStorage.back().data());
    }

    args.push_back(nullptr);
    execv(launcher.c_str(), args.data());
    return 1;
}
