#include "../inc/huffman.h"
#include <iostream>


using namespace std;
int main() {
    string message = "alabar_a_la_alabarda";
    auto code = encode(message);
    auto coded_message = get<0>(code);
    //auto tree = get<1>(code);
    std::cout << coded_message << std::endl;
    std::cout << "Longitud del mensaje: " << coded_message.size() << std::endl;
    return 0;
}
