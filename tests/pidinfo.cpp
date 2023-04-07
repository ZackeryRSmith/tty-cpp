/**
 * pidinfo - a command-line tool for getting information on a process with a given PID or name.
 *
 * Usage:
 *     pidinfo <pid_or_name> [--nocolor] [--notable] [--noheader] [--nounicode] [--showpath]
 *
 * Supported platforms: Windows, macOS, Linux.
 *
 * This tool displays information about the specified process, including its name, process state, 
 * memory usage, CPU usage, memory usage, and parent process. The information is obtained using
 * platform-specific system calls and APIs.
 */

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <optional>
#include "table.hpp"

// default values for optional args
#define _NOCOLOR   false
#define _NOTABLE   false
#define _NOHEADER  false
#define _NOUNICODE false
#define _SHOWPATH  false

enum class ProcessState {
    Running,
    Stopped,
    NotRunning,
    Suspended
};

#ifdef _WIN32
/*
                            .oodMMMM
                   .oodMMMMMMMMMMMMM
       ..oodMMM  MMMMMMMMMMMMMMMMMMM
 oodMMMMMMMMMMM  MMMMMMMMMMMMMMMMMMM
 MMMMMMMMMMMMMM  MMMMMMMMMMMMMMMMMMM
 MMMMMMMMMMMMMM  MMMMMMMMMMMMMMMMMMM
 MMMMMMMMMMMMMM  MMMMMMMMMMMMMMMMMMM
 MMMMMMMMMMMMMM  MMMMMMMMMMMMMMMMMMM
 MMMMMMMMMMMMMM  MMMMMMMMMMMMMMMMMMM

 MMMMMMMMMMMMMM  MMMMMMMMMMMMMMMMMMM
 MMMMMMMMMMMMMM  MMMMMMMMMMMMMMMMMMM
 MMMMMMMMMMMMMM  MMMMMMMMMMMMMMMMMMM
 MMMMMMMMMMMMMM  MMMMMMMMMMMMMMMMMMM
 MMMMMMMMMMMMMM  MMMMMMMMMMMMMMMMMMM
 `^^^^^^MMMMMMM  MMMMMMMMMMMMMMMMMMM
       ````^^^^  ^^MMMMMMMMMMMMMMMMM
                      ````^^^^^^MMMM
 */
    #define OS_NAME "Windows"
    #include <Windows.h>
    #include <Psapi.h>
    #include <tlhelp32.h>

    std::vector<DWORD> get_process_list() {
        DWORD processes[1024];
        DWORD bytes_used;
        if (!EnumProcesses(processes, sizeof(processes), &bytes_used)) {
            std::cerr << "Failed to get process list: " << GetLastError() << std::endl;
            return {};
        }
        const int count = bytes_used / sizeof(DWORD);
        return std::vector<DWORD>(processes, processes + count);
    }
    
    std::optional<DWORD> find_pid(const std::string& name) {
        const auto pids = get_process_list();
        for (const auto& pid : pids) {
            HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
            if (handle) {
                TCHAR buf[MAX_PATH];
                DWORD size = sizeof(buf) / sizeof(TCHAR);
                if (GetModuleBaseName(handle, NULL, buf, size) && std::string(buf) == name) {
                    return pid;
                }
                CloseHandle(handle);
            }
        }
        return std::nullopt;
    }

    std::string get_process_path(DWORD pid) {
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (processHandle != NULL) {
            char path[MAX_PATH];
            DWORD pathSize = GetModuleFileNameExA(processHandle, NULL, path, MAX_PATH);
            if (pathSize != 0) {
                return std::string(path, pathSize);
            }
            CloseHandle(processHandle);
        }
        return "";
    }

    ProcessState get_process_state(DWORD pid) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProcess == NULL) {
            return ProcessState::NotRunning;
        }
        DWORD exitCode = STILL_ACTIVE;
        if (!GetExitCodeProcess(hProcess, &exitCode)) {
            CloseHandle(hProcess);
            return ProcessState::NotRunning;
        }
        if (exitCode == STILL_ACTIVE) {
            PROCESS_MEMORY_COUNTERS pmc;
            if (!GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                CloseHandle(hProcess);
                return ProcessState::NotRunning;
            }
            if (pmc.WorkingSetSize == 0) {
                CloseHandle(hProcess);
                return ProcessState::Suspended;
            }
            CloseHandle(hProcess);
            return ProcessState::Running;
        }
        CloseHandle(hProcess);
        return ProcessState::NotRunning;
    }

    double get_cpu_usage(DWORD pid) {
        FILETIME creationTime, exitTime, kernelTime, userTime;
        if (GetProcessTimes(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid),
                            &creationTime, &exitTime, &kernelTime, &userTime)) {
            SYSTEMTIME systemTime, currentTime;
            GetSystemTime(&systemTime);
            FileTimeToSystemTime(&userTime, &currentTime);
            double elapsedTime = (systemTime.wHour - currentTime.wHour) * 3600.0
                + (systemTime.wMinute - currentTime.wMinute) * 60.0
                + (systemTime.wSecond - currentTime.wSecond)
                + (systemTime.wMilliseconds - currentTime.wMilliseconds) / 1000.0;
            double cpuTime = (kernelTime.dwLowDateTime + userTime.dwLowDateTime) / 10000000.0
                + (kernelTime.dwHighDateTime + userTime.dwHighDateTime) * 429.4967296;
            return cpuTime / elapsedTime * 100.0;
        }
        return 0.0;
    }

    int get_memory_usage(DWORD pid) {
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (processHandle != NULL) {
            PROCESS_MEMORY_COUNTERS_EX pmc;
            if (GetProcessMemoryInfo(processHandle, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                return static_cast<int>(pmc.PrivateUsage / 1024); // convert bytes to KB
            }
            CloseHandle(processHandle);
        }
        return 0;
    }

    DWORD get_parent_pid(DWORD pid) {
        DWORD parentPid = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 processEntry;
            processEntry.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(snapshot, &processEntry)) {
                do {
                    if (processEntry.th32ProcessID == pid) {
                        parentPid = processEntry.th32ParentProcessID;
                        break;
                    }
                } while (Process32Next(snapshot, &processEntry));
            }
            CloseHandle(snapshot);
        }
        return parentPid;
    }
#elif __APPLE__
/*
                        .8 
                      .888
                    .8888'
                   .8888'
                   888'
                   8'
      .88888888888. .88888888888.
   .8888888888888888888888888888888.
 .8888888888888888888888888888888888.
.&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&'
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&'
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&'
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%.
`%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%.
 `00000000000000000000000000000000000'
  `000000000000000000000000000000000'
   `0000000000000000000000000000000'
     `###########################'
       `#######################'
         `#########''########'
           `""""""'  `"""""'
 */
    #define OS_NAME "macOS"
    #include <sys/sysctl.h>
    #include <mach/task_info.h>
    #include <libproc.h>
    #include <mach/mach.h>

    std::vector<pid_t> find_pids(const std::string& name) {
        std::vector<pid_t> pids;
        int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
        size_t len;
        if (sysctl(mib, 4, NULL, &len, NULL, 0) != 0) {
            return pids;
        }
        struct kinfo_proc* proc_list = (struct kinfo_proc*)malloc(len);
        if (sysctl(mib, 4, proc_list, &len, NULL, 0) != 0) {
            free(proc_list);
            return pids;
        }
        int n_proc = len / sizeof(struct kinfo_proc);
        for (int i = 0; i < n_proc; i++) {
            if (strcmp(proc_list[i].kp_proc.p_comm, name.c_str()) == 0) {
                pids.push_back(proc_list[i].kp_proc.p_pid);
            }
        }
        free(proc_list);
        return pids;
    }

    std::string get_process_path(pid_t pid) {
        char name[PROC_PIDPATHINFO_MAXSIZE];
        if (proc_pidpath(pid, name, sizeof(name)) <= 0) {
            std::cerr << "Error getting process name\n";
            return "";
        }
        return std::string(name);
    }

    ProcessState get_process_state(pid_t pid) {
        struct kinfo_proc info;
        memset(&info, 0, sizeof(info));
        size_t size = sizeof(info);
        int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, pid };
        if (sysctl(mib, 4, &info, &size, NULL, 0) != 0) {
            return ProcessState::NotRunning;
        }
        if (info.kp_proc.p_stat == SSTOP) {
            return ProcessState::Suspended;
        } else if (info.kp_proc.p_stat == SRUN) {
            return ProcessState::Running;
        } else {
            return ProcessState::NotRunning;
        }
    }

    double get_cpu_usage(pid_t pid) {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        double cpu_usage = ((double)usage.ru_utime.tv_sec + (double)usage.ru_stime.tv_sec) +
                           ((double)usage.ru_utime.tv_usec + (double)usage.ru_stime.tv_usec) / 1e6;
        return cpu_usage;
    }

    int get_memory_usage(pid_t pid) {
        struct proc_taskinfo pti;
        int size = sizeof(pti);
        if (proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &pti, size) <= 0) {
            return -1;
        }
        return pti.pti_resident_size / 1024;
    }

    pid_t get_parent_pid(pid_t pid) {
        struct proc_bsdinfo pbi;
        int size = sizeof(pbi);
        if (proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &pbi, size) <= 0) {
            return -1;
        }
        return pbi.pbi_ppid;
    }
#elif __linux__
/*
                 .88888888:.
                88888888.88888.
              .8888888888888888.
              888888888888888888
              88' _`88'_  `88888
              88 88 88 88  88888
              88_88_::_88_:88888
              88:::,::,:::::8888
              88`:::::::::'`8888
             .88  `::::'    8:88.
            8888            `8:888.
          .8888'             `888888.
         .8888:..  .::.  ...:'8888888:.
        .8888.'     :'     `'::`88:88888
       .8888        '         `.888:8888.
      888:8         .           888:88888
    .888:88        .:           888:88888:
    8888888.       ::           88:888888
    `.::.888.      ::          .88888888
   .::::::.888.    ::         :::`8888'.:.
  ::::::::::.888   '         .::::::::::::
  ::::::::::::.8    '      .:8::::::::::::.
 .::::::::::::::.        .:888:::::::::::::
 :::::::::::::::88:.__..:88888:::::::::::'
  `'.:::::::::::88888888888.88:::::::::'
        `':::_:' -- '' -'-' `':_::::'`
*/
    #define OS_NAME "Linux"
    #include <dirent.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <sstream>
    #include <filesystem>
    #include <cstdlib>

    std::vector<pid_t> get_process_list() {
        std::vector<pid_t> pids;
        DIR* dir = opendir("/proc");
        if (!dir) {
            return pids;
        }
        struct dirent* entry;
        while ((entry = readdir(dir))) {
            if (std::all_of(entry->d_name, entry->d_name + strlen(entry->d_name), isdigit)) {
                pids.push_back(atoi(entry->d_name));
            }
        }
        closedir(dir);
        return pids;
    }

    std::vector<pid_t> find_pid(const std::string& name) {
        std::vector<pid_t> pids;
        DIR* dir = opendir("/proc");
        if (dir == NULL) {
            return pids;
        }
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type != DT_DIR) {
                continue;
            }
            const char* name_str = entry->d_name;
            for (const char* p = name_str; *p != '\0'; p++) {
                if (!isdigit(*p)) {
                    goto skip;
                }
            }
            pid_t pid = atoi(name_str);
            char buf[1024];
            snprintf(buf, sizeof(buf), "/proc/%d/cmdline", pid);
            FILE* fp = fopen(buf, "r");
            if (fp != NULL) {
                if (fgets(buf, sizeof(buf), fp) != NULL) {
                    std::string cmd(buf);
                    size_t pos = cmd.find('\0');
                    if (pos != std::string::npos) {
                        cmd.erase(pos);
                    }
                    pos = cmd.rfind('/');
                    if (pos != std::string::npos) {
                        cmd.erase(0, pos+1);
                    }
                    if (cmd == name) {
                        pids.push_back(pid);
                    }
                }
                fclose(fp);
            }
            skip:;
        }
        closedir(dir);
        return pids;
    }

    std::string get_process_path(int pid) {
        std::string path;
        std::ifstream ifs("/proc/" + std::to_string(pid) + "/cmdline");
        if (ifs) {
            getline(ifs, path, '\0');
            ifs.close();
        }
        return path;
    }

    ProcessState get_process_state(pid_t pid) {
        std::ostringstream statFilePath;
        statFilePath << "/proc/" << pid << "/stat";
        std::ifstream statFile(statFilePath.str());
        if (!statFile.is_open()) {
            return ProcessState::NotRunning;
        }
        std::string line;
        std::getline(statFile, line);
        std::istringstream iss(line);
        char state;
        iss >> std::skipws >> state; // first character is process state
        statFile.close();
        if (state == 'R') {
            return ProcessState::Running;
        } else if (state == 'T' || state == 't' || state == 'W' || state == 'X' || state == 'Z') {
            return ProcessState::Stopped;
        } else {
            return ProcessState::NotRunning;
        }
    }

    double get_cpu_usage(pid_t pid) {
        double cpuUsage = 0.0;
        std::ifstream ifs("/proc/" + std::to_string(pid) + "/stat");
        if (ifs) {
            std::string line;
            getline(ifs, line);
            std::istringstream iss(line);
            int i = 1;
            while (i <= 13) {
                iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
                i++;
            }
            unsigned long long utime, stime;
            iss >> utime >> stime;
            ifs.close();
            unsigned long long total_time = utime + stime;
            unsigned long long uptime = sysconf(_SC_CLK_TCK);
            cpuUsage = static_cast<double>(total_time) / static_cast<double>(uptime);
        }
        return cpuUsage;
    }

    int get_memory_usage(pid_t pid) {
        int memoryUsage = 0;
        std::ifstream ifs("/proc/" + std::to_string(pid) + "/status");
        if (ifs) {
            std::string line;
            while (getline(ifs, line)) {
                if (line.compare(0, 6, "VmSize") == 0) {
                    std::istringstream iss(line);
                    iss.ignore(std::numeric_limits<std::streamsize>::max(), ':');
                    iss >> memoryUsage;
                    ifs.close();
                    break;
                }
            }
        }
        return memoryUsage;
    }

    int get_parent_pid(int pid) {
        int ppid = 0;
        std::ifstream ifs("/proc/" + std::to_string(pid) + "/stat");
        if (ifs) {
            std::string line;
            getline(ifs, line);
            std::istringstream iss(line);
            int i = 1;
            while (i <= 3) {
                iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
                i++;
            }
            iss >> ppid;
            ifs.close();
        }
        return ppid;
    }
#else
    #define OS_NAME "Unknown"
#endif

std::string basename(const std::string& path) {
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        return path;
    }
    return path.substr(pos + 1);
}

bool isnum(const std::string& s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

int main(int argc, char **argv) {
    vector<string> args(argv + 1, argv + argc);

    bool NOCOLOR = _NOCOLOR;
    bool NOTABLE = _NOTABLE;
    bool NOHEADER = _NOHEADER;
    bool NOUNICODE = _NOUNICODE;
    bool SHOWPATH = _SHOWPATH;

    // loop over command-line args
    for (auto i = args.begin(); i != args.end(); ++i) {
        if (*i == "-h" || *i == "--help") {
            cout << "Syntax: pidinfo <pid_or_name> [--nocolor] [--notable] [--noheader] [--nounicode] [--showpath]" << endl;
            return 0;
        } else if (*i == "--nocolor") {
            NOCOLOR = true;
        } else if (*i == "--notable") {
            NOTABLE = true;
        } else if (*i == "--noheader") {
            NOHEADER = true;
        } else if (*i == "--nounicode") {
            NOUNICODE = true;
        } else if (*i == "--showpath") {
            SHOWPATH = true;
        }
    }

    if (argc < 2) {
        std::cout << "Please provide a process name or PID!" << std::endl;
        return 1;
    }

    const auto PIDS = find_pids(isnum(args[0]) ? basename(get_process_path(std::stoi(args[0]))) : args[0]);
    if (PIDS.size() == 0) {
        std::cout << "Unable to find process!" << std::endl;
        return 1;
    }


    TableStyle style("│", "─", "─", "├", "┤", "╭", "╮", "╰", "╯", ' ');
    if (NOTABLE) {
        style = TableStyle("", "", "", "", "", "", "", "", "", ' ');
    } else if (NOUNICODE) {
        style = TableStyle("|", "-", "-", "+", "+", "+", "+", "+", "+", ' ');
    }
    
    Table table({}, style);
        
    if (!NOHEADER) {
        table.set_headers({"Process Name", "PID", "Process State", "CPU Usage (secs)", "Memory Usage (KB)", "Parent Process"}, true);
    }
    
    std::vector<std::string> row;

    for (pid_t pid : PIDS) {
        std::string pidpath = get_process_path(pid);
        row.push_back(SHOWPATH ? PIDPATH : basename(pidpath));
        row.push_back(std::to_string(pid));
    
        switch (get_process_state(pid)) {
            case ProcessState::Running: row.push_back("Running"); break;
            case ProcessState::Stopped: row.push_back("Stopped"); break;
            case ProcessState::NotRunning: row.push_back("Not Running"); break;
            case ProcessState::Suspended: row.push_back("Suspended"); break;
        }

        row.push_back(std::to_string(get_cpu_usage(pid)));
        row.push_back(std::to_string(get_memory_usage(pid)));
        row.push_back(std::to_string(get_parent_pid(pid)));

        table.add_row(row);
        row.clear();
    }
    table.print(); // need to create a color system :)

    return 0;
}
