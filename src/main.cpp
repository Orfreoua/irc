
#include "headers.hpp"


int port;
int is_running;

void printCenteredText(const std::string &text) {
    // Get terminal dimensions
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int columns = w.ws_col;

    // Calculate the starting column for centered text
    int text_length = text.length();
    int start_col = (columns - text_length) / 2;

    // Clear the screen and position the cursor
    std::cout << "\x1b[2J\x1b[H";

    // Move the cursor to row 1, middle column
    std::cout << "\x1b[1;" << start_col << "H";

    // Print the formatted text
    std::cout << "\x1b[1;5;31m" << text << "\x1b[0m" << std::endl;
}

int main(int argc, char *argv[]) {
  //-- gestion de la sortie via signal
  IRCServer& server = IRCServer::getInstance();
  signal_catching();

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <port> <pass>" << std::endl;
    exit(EXIT_FAILURE);
  }
  int port = std::atoi(argv[1]);
  std::string pass = argv[2];

  try {
    // Use the singleton instance
    server.setPort(port);
    server.setPass(pass);

    if (!server.initSocket()) {
      throw std::runtime_error("Failed to init socket.");
    }

    if (!server.bindSocket()) {
      throw std::runtime_error("Failed to bind socket.");
    }
    printCenteredText("Welcome to 42IRC !");

    if (sodium_init() == -1) {
      std::cout << "Failed to init libsodium" << std::endl;
      return EXIT_FAILURE;
    }

    if (!server.isListening()) {
      throw std::runtime_error("Failed to listen connections.");
    }

    server.acceptClients();
  } catch (const std::runtime_error &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
