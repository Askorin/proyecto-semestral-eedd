#include "../inc/huffman.h"
#include <iostream>


using namespace std;
int main() {
    string message = "hola_fabian";
    std::cout << "Encoding: " << message << std::endl;
    auto code = encode(message);
    auto coded_message = get<0>(code);
    //auto tree = get<1>(code);
    std::cout << "Coded message: " << coded_message << std::endl;
    std::cout << "Message bits: " << coded_message.size() << std::endl;
    return 0;
}
