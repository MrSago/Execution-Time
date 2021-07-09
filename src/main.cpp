
#include <iostream>
#include <memory>
#include <chrono>
#include <Windows.h>


struct Args {
    std::unique_ptr<char> options;
    unsigned long time;
};

bool parseArg(int argc, char** argv, Args* out) {
    bool err = false;
    bool r_flag = false;
    bool f_flag = false;
    bool w_flag = false;
    char* r_opt = NULL;
    char* f_opt = NULL;

    for (int i = 1; !err && i < argc && argv[i][0] == '-'; ++i) {
        switch (argv[i][1]) {
            case 'r':
                if (r_flag || ++i >= argc) {
                    err = true;
                    break;
                }
                r_opt = argv[i];
                r_flag = true;
            break;

            case 'f':
                if (f_flag || ++i >= argc) {
                    err = true;
                    break;
                }
                f_opt = argv[i];
                f_flag = true;
            break;

            case 'w':
                if (w_flag || ++i >= argc) {
                    err = true;
                    break;
                }
                out->time = strtoul(argv[i], NULL, 10);
                w_flag = true;
            break;

            default:
                err = true;
            break;
        }
    }

    if (err || !(f_flag && w_flag)) {
        out->options = NULL;
        out->time = 0;
        return false;
    } 

    if (r_flag) {
        std::string make_opt = static_cast<std::string>(r_opt) + ' ' + static_cast<std::string>(f_opt);
        out->options = std::make_unique<char>(sizeof(char) * (make_opt.length() + 1));
        strcpy(out->options.get(), make_opt.c_str());
    } else {
        out->options = std::make_unique<char>(sizeof(char) * (strlen(f_opt) + 1));
        strcpy(out->options.get(), f_opt);
    }

    return true;
}

HANDLE Run(char* options) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(STARTUPINFO));
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));

    if (!CreateProcess(
            NULL, options,
                NULL, NULL, FALSE, FALSE, NULL, NULL, &si, &pi)) {
        return NULL;
    }

    return pi.hProcess;
}

long long GetTimeProcess(HANDLE hp, DWORD time) {
    auto start = std::chrono::high_resolution_clock::now();
    DWORD ret_code = WaitForSingleObject(hp, time);
    auto stop = std::chrono::high_resolution_clock::now();

    switch (ret_code) {
        case WAIT_ABANDONED:
            std::cerr << "Process not signaled! Error code: " << GetLastError() << '\n';
        break;

        case WAIT_OBJECT_0:
            //std::cerr << "Success!\n";
        break;

        case WAIT_TIMEOUT:
            std::cerr << "Time limit exceeded more than " << time << " ms\n";
            TerminateProcess(hp, 0);
        break;

        case WAIT_FAILED:
            std::cerr << "Process failed. Error code: " << GetLastError() << '\n';
        break;

        default:
            std::cerr << "Unknown error. Error code: " << GetLastError() << '\n';
        break;
    }

    CloseHandle(hp);

    return (stop - start).count();
}


int main(int argc, char** argv, char** envp) {
    Args args;
    if (!parseArg(argc, argv, &args)) {
        std::cerr << "Usage: " << argv[0] << " -r [path to run interpreter] (optional) -f [path to file] -w [waiting time in ms]\n";
        return 1;
    }

    HANDLE hProcess = Run(args.options.get());
    if (!hProcess) {
        std::cerr << "Can't run new process! Error code: " << GetLastError() << '\n';
        return 2;
    }

    double elpsed_time = GetTimeProcess(hProcess, args.time) * 1e-6;
    std::cerr << "Elapsed time: " << elpsed_time << " ms\n";

    return 0;
}

