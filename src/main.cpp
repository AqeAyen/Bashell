#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

// Splits command into two parts "like ls -l" it splits the command
// into "ls" and "-l"
std::vector<std::string> splitInput(const std::string &input) {
  std::vector<std::string> tokens;
  std::stringstream ss(input);
  std::string token;

  while (ss >> token) {
    tokens.push_back(token);
  }

  return tokens;
}

// Convert vector of strings into array of C-strings like char[n] = "stuff"

char **convertToArr(const std::vector<std::string> &args) {
  char **cArgs = new char *[args.size() + 1];

  for (size_t i = 0; i < args.size(); i++) {
    cArgs[i] = const_cast<char *>(args[i].c_str());
  }
  cArgs[args.size()] = nullptr;

  return cArgs;
}
int main(int argc, char **argv) {
  std::string input;

  while (true) {
    std::cout << "bashell  $ ";
    std::getline(std::cin, input);

    if (input == "exit") {
      break;
    }
    std::vector<std::string> args = splitInput(input);
    if (args[0] == "cd") {
      if (args.size() < 2) {
        std::cerr << "cd missing args\n";
      } else {
        if (chdir(args[1].c_str()) != 0) {
          perror("cd");
        }
      }
      continue;
    }
    if (args.empty()) {
      continue;
    }
    // creates a new proccess to run the commands
    pid_t pid = fork();

    if (pid == 0) {
      char **cArgs = convertToArr(args);
      execvp(cArgs[0], cArgs);
      perror("Error Executing command");

      delete[] cArgs;
      exit(EXIT_FAILURE);
    } else if (pid > 0) {
      // parent proccess
      int status;
      waitpid(pid, &status, 0);
    } else {
      perror("Fork failed");
    }
  }
  return 0;
}
