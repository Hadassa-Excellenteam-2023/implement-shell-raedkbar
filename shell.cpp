#include <iostream>
#include <vector>
#include <string>
#include <sstream>

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
    std::ostringstream commandStream;
    commandStream << command;
    for (const std::string& arg : args) {
        commandStream << ' ' << arg;
    }
    std::string commandString = commandStream.str();

    int status = system(commandString.c_str());
    if (status == -1) {
        std::cerr << "Failed to execute command: " << commandString << std::endl;
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

        // Execute the command
        executeCommand(command, args, runInBackground);

        // Add the background process to the list
        if (runInBackground) {
            backgroundProcesses.push_back(0); // Change 0 to the actual process ID
        }
    }

    return 0;
}
