#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * Function to split a string into tokens based on delimiter
 *
 * @param str: The string to be split
 * @param delimiter: The character used as a delimiter
 * @return: A vector of tokens extracted from the string
 */
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

/**
 * Function to execute the command
 *
 * @param command: The command to be executed
 * @param args: The arguments passed to the command
 * @param runInBackground: Flag indicating if the command should run in the background
 */
void executeCommand(const std::string& command, const std::vector<std::string>& args, bool runInBackground) {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Failed to create child process." << std::endl;
        return;
    } else if (pid == 0) {
        // Child process
        char** argv = new char*[args.size() + 2];
        argv[0] = const_cast<char*>(command.c_str());
        for (size_t i = 0; i < args.size(); ++i) {
            argv[i + 1] = const_cast<char*>(args[i].c_str());
        }
        argv[args.size() + 1] = nullptr;

        // Execute the command
        execvp(command.c_str(), argv);
        std::cerr << "Failed to execute command: " << command << std::endl;
        delete[] argv;
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        if (!runInBackground) {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

/**
 * Function to check if a command is running in the background
 *
 * @param args: The arguments passed to the command
 * @return: True if the command is a background command, False otherwise
 */
bool isBackgroundCommand(const std::vector<std::string>& args) {
    return !args.empty() && args.back() == "&";
}

/**
 * Function to remove the background '&' character from the command
 *
 * @param args: The arguments passed to the command
 */
void removeBackgroundSymbol(std::vector<std::string>& args) {
    if (!args.empty() && args.back() == "&") {
        args.pop_back();
    }
}

/**
 * Function to print details about background jobs
 *
 * @param backgroundProcesses: The list of background process IDs
 */
void printBackgroundJobs(const std::vector<pid_t>& backgroundProcesses) {
    for (const pid_t& pid : backgroundProcesses) {
        std::string cmdLine;
        std::ifstream cmdLineFile("/proc/" + std::to_string(pid) + "/cmdline");
        std::getline(cmdLineFile, cmdLine);
        cmdLineFile.close();
        std::cout << "PID: " << pid << "  Command: " << cmdLine << std::endl;
    }
}

/**
 * The main function
 */
int main() {
    std::string commandLine;
    std::vector<pid_t> backgroundProcesses;

    while (true) {
        std::cout << "shell> ";
        std::getline(std::cin, commandLine);

        // Split the command line into command and arguments
        std::vector<std::string> tokens = split(commandLine, ' ');
        if (tokens.empty()) {
            continue;
        }

        // Extract the command and arguments
        std::string command = tokens[0];
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());

        // Check if the command is running in the background
        bool runInBackground = isBackgroundCommand(args);
        if (runInBackground) {
            removeBackgroundSymbol(args);
        }

        // Handle built-in commands
        if (command == "myjobs") {
            printBackgroundJobs(backgroundProcesses);
            continue;
        }

        // Execute the command
        executeCommand(command, args, runInBackground);

        // Add the background process to the list
        if (runInBackground) {
            pid_t pid = fork();
            if (pid < 0) {
                std::cerr << "Failed to create child process." << std::endl;
            } else if (pid > 0) {
                backgroundProcesses.push_back(pid);
            } else {
                // Child process
                break;
            }
        }
    }

    return EXIT_SUCCESS;
}
