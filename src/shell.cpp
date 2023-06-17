#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

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
 * @param inputRedirect: File path for input redirection (STDIN)
 * @param outputRedirect: File path for output redirection (STDOUT)
 */
void executeCommand(const std::string& command, const std::vector<std::string>& args, bool runInBackground,
    const std::string& inputRedirect, const std::string& outputRedirect) {
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to create child process." << std::endl;
        return;
    } else if (pid == 0) {
        // Child process

        // Input redirection
        if (!inputRedirect.empty()) {
            int inputFile = open(inputRedirect.c_str(), O_RDONLY);
            if (inputFile == -1) {
                std::cerr << "Failed to open input file: " << inputRedirect << std::endl;
                exit(EXIT_FAILURE);
            }
            dup2(inputFile, STDIN_FILENO);
            close(inputFile);
        }

        // Output redirection
        if (!outputRedirect.empty()) {
            int outputFile = open(outputRedirect.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (outputFile == -1) {
                std::cerr << "Failed to open output file: " << outputRedirect << std::endl;
                exit(EXIT_FAILURE);
            }
            dup2(outputFile, STDOUT_FILENO);
            close(outputFile);
        }

        // Convert vector of strings to array of C-style strings
        std::vector<char*> cArgs;
        for (const std::string& arg : args) {
            cArgs.push_back(const_cast<char*>(arg.c_str()));
        }
        cArgs.push_back(nullptr); // Null-terminate the array

        execvp(command.c_str(), cArgs.data());
        std::cerr << "Failed to execute command: " << command << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        if (!runInBackground) {
            waitpid(pid, nullptr, 0);
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
void printBackgroundJobs(const std::vector<int>& backgroundProcesses) {
    for (int pid : backgroundProcesses) {
        std::cout << "PID: " << pid << std::endl;
    }
}

/**
 * The main function
 */
int main() {
    std::string commandLine;
    std::vector<int> backgroundProcesses;

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

        // Input/output redirection
        std::string inputRedirect;
        std::string outputRedirect;
        for (size_t i = 0; i < args.size(); i++) {
            if (args[i] == "<" && i + 1 < args.size()) {
                inputRedirect = args[i + 1];
                args.erase(args.begin() + i, args.begin() + i + 2);
                i--;
            } else if (args[i] == ">" && i + 1 < args.size()) {
                outputRedirect = args[i + 1];
                args.erase(args.begin() + i, args.begin() + i + 2);
                i--;
            }
        }

        // Execute the command
        executeCommand(command, args, runInBackground, inputRedirect, outputRedirect);

        // Add the background process to the list
        if (runInBackground) {
            backgroundProcesses.push_back(0); // Change 0 to the actual process ID
        }
    }

    return 0;
}
