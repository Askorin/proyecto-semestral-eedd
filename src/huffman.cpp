#include "../inc/huffman.h"
#include <boost/dynamic_bitset.hpp>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <ostream>
#include <queue>
#include <stack>

bool CompareHuffmanNodes::operator()(Node* node_l, Node* node_r) {
    return node_l->freq > node_r->freq;
}

bool CompareCodes::operator()(std::pair<char, unsigned char> symbol_l,
                              std::pair<char, unsigned char> symbol_r) {
    /* Si la longitud del código es la misma, comparamos por orden de su código
     * ascii. */
    if (symbol_l.second == symbol_r.second)
        return symbol_l.first > symbol_r.first;
    /* Pero en verdad la prioridad es la longitud del código */
    else
        return symbol_l.second > symbol_r.second;
}

Node::Node(char symbol, size_t freq, Node* left, Node* right, Node* parent)
    : symbol(symbol), freq(freq), left(left), right(right), parent(parent) {}

Node::Node(Node* node_l, Node* node_r)
    : symbol('\0'), freq(node_l->freq + node_r->freq), left(node_l),
      right(node_r), parent(nullptr) {

    node_l->parent = this;
    node_r->parent = this;
}

bool Node::is_leaf() { return !(left || right); }

std::ostream& operator<<(std::ostream& os, const Node& node) {
    return (os << node.symbol << " : " << node.freq);
}

Code::Code(std::bitset<255> code, unsigned char length)
    : code(code), length(length) {}

Code::Code() : code(std::bitset<255>()), length(0) {}

Code Code::get_reversed() {
    std::bitset<255> reversed;
    std::bitset<255> to_reverse = code;
    for (size_t i = 0; i < length; ++i) {
        reversed[length - 1 - i] = to_reverse[0];
        to_reverse >>= 1;
    }
    code = reversed;
    return Code(reversed, length);
}

void encode_file(std::string& file_name_input, std::string& file_name_output) {
    std::cout << "### Comenzando Codificación ###\n";
    size_t message_len = 0;
    auto frequencies =
        calculate_frequencies_from_file(file_name_input, message_len);
    std::cout << "Message length: " << message_len << '\n';
    Node* root = generate_huffman_tree(frequencies);

    /* Mapa que guardará las longitudes de los códigos de cada símbolo, esto es
     * en verdad lo único que importa. Una vez tengamos estos, podremos
     * construir los códigos canónicos.
     */
    std::unordered_map<char, unsigned char> code_map;

    /*
     * Mapa que guardará la frecuencia de una longitud de código dada, utilizado
     * para Huffman canónico
     */
    std::unordered_map<unsigned char, size_t> length_map;

    /* Longitud total del mensaje codificado */
    size_t total_len = 0;
    /* Máxima longitud de los códigos */
    size_t max_length = 0;
    traverse_huffman_tree(root, code_map, 0, total_len, length_map, max_length);

    /* Este vector servirá para guardar los símbolos en orden el de las
     * longitudes de sus códigos */
    std::vector<char> ordered_symbols;

    /*
     * Generamos los códigos canónicos
     */
    std::unordered_map<char, Code> canonical_codes =
        get_canonical_codes(code_map, ordered_symbols);

    // std::cout << "Códigos Canónicos:\n";
    // for (auto entry : canonical_codes) {
    //     char c = entry.first;
    //     boost::dynamic_bitset<> code = entry.second.code;
    //     code.resize(entry.second.length);
    //     std::cout << c << ", " << int(c) << ": " << code << std::endl;
    // }

    boost::dynamic_bitset<> coded_message(total_len);

    /* Codificamos */
    std::ifstream fin(file_name_input, std::fstream::in);

    std::ofstream fout(file_name_output, std::ofstream::out |
                                             std::ofstream::trunc |
                                             std::ofstream::binary);

    /*
     * Para el primer tipo de header se guardan las longitudes de los códigos
     * para TODOS los símbolos del alfabeto.
     * */
    size_t tamaño_header_tipo_1 = 255;
    /*
     * Para el segundo tipo se guardan las frecuencias de cada longitud de
     * código y luego los símbolos ordenados con respecto a longitud de código.
     * Se deben guardar las frecuencias de longitudes para cada longitud menor o
     * igual que la máxima, eso incurre 1 * max_length bytes. Luego,
     * evidentemente, para guardar los símbolos es el tamaño del vector de
     * ellos. Además se incluye un separador de un byte netre las dos.
     */
    size_t tamaño_header_tipo_2 = max_length + ordered_symbols.size() + 1;

    if (tamaño_header_tipo_1 <= tamaño_header_tipo_2) {
        std::cout << "Usando header tipo 1\n";
    } else
        std::cout << "Usando header tipo 2\n";

    save_header(fout, canonical_codes, length_map, ordered_symbols,
                message_len);
    save_code(fout, fin, canonical_codes, length_map, ordered_symbols,
              message_len);

    fin.close();
    fout.close();
    return;
}

std::unordered_map<char, Code>
get_canonical_codes(std::unordered_map<char, unsigned char>& code_map,
                    std::vector<char>& ordered_symbols) {

    /* Primero tenemos que ordenar los códigos, usamos una priority queue. */
    std::priority_queue<std::pair<char, unsigned char>,
                        std::vector<std::pair<char, unsigned char>>,
                        CompareCodes>
        prio_queue;

    for (auto entry : code_map) {
        prio_queue.push(entry);
    }

    std::unordered_map<char, Code> canonical_codes;

    /*
     * Observación: En el peor caso, para los 256 carácteres ASCII, podríamos
     * terminar con un código de 255 bits, asi que guardaremos los códigos en un
     * bitset de ese tamaño.
     */
    std::bitset<255> code;
    /*
     * Al primer símbolo se le asigna un código de la misma longitud, compuesto
     * únicamente por ceros.
     */

    while (!prio_queue.empty()) {
        auto current_symbol = prio_queue.top();
        ordered_symbols.push_back(current_symbol.first);
        canonical_codes[current_symbol.first] =
            Code(code, current_symbol.second).get_reversed();

        unsigned char current_bit_length = current_symbol.second;
        prio_queue.pop();
        unsigned char next_bit_length = prio_queue.top().second;

        increment(code);

        code = code << (next_bit_length - current_bit_length);
    }
    return canonical_codes;
}

void save_header(std::ofstream& fout,
                 std::unordered_map<char, Code>& canonical_codes,
                 std::unordered_map<unsigned char, size_t>& length_map,
                 std::vector<char>& ordered_symbols, size_t message_len) {

    std::cout << "### Guardando header ###\n";
    /*
     * Guardaremos el mensaje en un formato Huffman Compacto, de esta manera
     * no hay necesidad de guardar el arbol de Huffman entero en el archivo.
     */

    /*
     * Método 1, útil en caso de varios elementos de la tabla ascii
     * existentes en el código a comprimir. Se guardan las longitudes de los
     * códigos de cada carácter ascii. Si el carácter no se encontraba en el
     * texto, tiene longitud 0.
     *
     * Como header del archivo, guardamos:
     * - En los primeros 4 bytes se guardarán la cantidad de carácters
     * codificados, en teoria esto debería dejarnos comprimir hasta ~4 GiB.
     * - Desde ahí vienen las longitudes de los códigos para cada caracter
     * del alfabeto.
     */

    /* Longitud del mensaje */
    fout.write(reinterpret_cast<char*>(&message_len), 4);

    /* Longitudes de códigos de cada símbolo */
    for (size_t c = 0; c <= 255; ++c) {
        unsigned char bit_length = 0;
        /* Si el carácter está en el mapa su código tiene longitud no cero
         */
        if (canonical_codes.count(char(c))) {
            bit_length = canonical_codes[char(c)].length;
        }

        fout.write(reinterpret_cast<char*>(&bit_length), sizeof(bit_length));
    }

    /*
     * Método 2, útil cuando no se está utilizando todo el alfabeto. Se
     * guarda primero la cantidad de longitudes
     */
}

void save_code(std::ofstream& fout, std::ifstream& fin,
               std::unordered_map<char, Code>& canonical_codes,
               std::unordered_map<unsigned char, size_t>& length_map,
               std::vector<char>& ordered_symbols, size_t message_len) {

    std::cout << "### Guardando código ###\n";

    unsigned char buffer = 0;
    unsigned char buffer_size = 0;
    char c;
    while (!fin.eof()) {
        fin >> std::noskipws >> c;
        auto code = canonical_codes[c].code;
        auto bits = canonical_codes[c].length;

        while (bits) {
            buffer |= (code.to_ulong() << buffer_size);
            unsigned char inserted_bits =
                bits > 8 - buffer_size ? 8 - buffer_size : bits;
            buffer_size += inserted_bits;
            code >>= inserted_bits;
            bits -= inserted_bits;

            if (buffer_size == 8) {
                fout.write(reinterpret_cast<char*>(&buffer), sizeof(buffer));
                buffer = 0, buffer_size = 0;
            }
        }
    }

    if (buffer_size) {
        fout.write(reinterpret_cast<char*>(&buffer), sizeof(buffer));
        buffer = 0, buffer_size = 0;
    }
}

void decode_file(std::string& file_name_input, std::string& file_name_output) {
    std::ifstream fin(file_name_input,
                      std::ifstream::in | std::ifstream::binary);

    std::cout << "### Empezando a decodificar ###\n";
    /* Primero extraemos la longitud del mensaje, esta se encuentra en los
     * primeros 4 bytes. */
    size_t message_len;
    fin.read(reinterpret_cast<char*>(&message_len), 4);
    std::cout << "### Longitud del mensaje: " << message_len << "###\n";

    /* Ahora reconstruiremos el mapa de longitudes de código para cada
     * carácter
     */
    std::unordered_map<char, unsigned char> code_lengths;
    for (size_t c = 0; c <= 255; ++c) {
        unsigned char bit_length = 0;
        fin.read(reinterpret_cast<char*>(&bit_length), 1);
        if (bit_length)
            code_lengths[char(c)] = bit_length;
    }

    std::cout << "### Generando códigos canónicos para decodificación ###\n";

    /* Ahora tenemos que reconstruir los códigos canónicos. */
    std::vector<char> ordered_symbols;
    std::unordered_map<char, Code> code_map =
        get_canonical_codes(code_lengths, ordered_symbols);

    std::cout << "### Recuperando arbol de Huffman ###\n";
    /* Regeneramos al arbol */
    Node* root = generate_huffman_tree(code_map);

    std::cout << "### Decodificando y guardando resultados ###\n";
    std::ofstream fout(file_name_output,
                       std::ofstream::out | std::ofstream::trunc);

    size_t decoded_symbols = 0;

    unsigned char buffer = 0;
    unsigned char buffer_size = 0;

    Node* current = root;
    while (decoded_symbols < message_len) {
        if (!buffer_size) {
            fin.read(reinterpret_cast<char*>(&buffer), 1);
            buffer_size += 8;
        }
        int bit = buffer & 1;
        buffer >>= 1;
        --buffer_size;

        // TODO: Manejar caso root es hoja
        if (bit)
            current = current->right;
        else
            current = current->left;

        if (current->is_leaf()) {
            ++decoded_symbols;
            fout << current->symbol;
            current = root;
        }
    }

    fout.close();
}

void traverse_huffman_tree(
    Node* root, std::unordered_map<char, unsigned char>& code_map,
    size_t& total_bits, std::unordered_map<unsigned char, size_t>& length_map,
    size_t& max_length) {

    if (!root)
        return;

    std::stack<std::pair<Node*, unsigned char>>* stack =
        new std::stack<std::pair<Node*, unsigned char>>();
    stack->push(std::pair(root, 0));

    while (!stack->empty()) {
        auto current = stack->top();
        stack->pop();

        Node* node = current.first;
        unsigned char bit = current.second;

        if (node->is_leaf()) {
            total_bits += node->freq * bit;
            code_map.insert({node->symbol, bit});
            ++length_map[bit];
        }
        if (node->right) {
            stack->push(std::pair(node->right, bit + 1));
        }
        if (node->left) {
            stack->push({node->left, bit + 1});
        }
    }
    delete stack;
}

std::string decode(std::string& coded_message, Node& tree) {

    /* El mensaje es la repeticion de un mismo caracter */
    if (tree.is_leaf()) {
        return std::string(coded_message.size(), tree.symbol);
    }

    std::string decoded_message("");

    /* Decodificamos */
    Node* v = &tree;
    for (char c : coded_message) {
        /* Si es 0, vamos al hijo izquierdo, de lo contrario (es 1), vamos a
         * la derecha */
        if (c == '0') {
            v = v->left;
        } else {
            v = v->right;
        }
        /*
         * Si el nodo es una hoja, hemos encontrado el simbolo
         * correspondiente, la agregamos al mensaje decodificado y volvemos
         * a la raíz.
         */
        if (v->is_leaf()) {
            decoded_message += v->symbol;
            v = &tree;
        }
    }
    return decoded_message;
}

std::unordered_map<char, size_t>
calculate_frequencies_from_file(std::string& file_name, size_t& message_len) {
    std::unordered_map<char, size_t> frequencies;
    std::fstream fin(file_name, std::fstream::in);
    char c;
    while (fin >> std::noskipws >> c) {
        frequencies[c]++;
        ++message_len;
    }
    return frequencies;
}

std::unordered_map<char, size_t> calculate_frequencies(std::string& message) {
    std::unordered_map<char, size_t> frequencies;
    for (char c : message) {
        frequencies[c]++;
    }
    return frequencies;
}

/* TODO: Manejar edge case de un carácter */
Node* generate_huffman_tree(std::unordered_map<char, size_t>& frequencies) {

    std::priority_queue<Node*, std::vector<Node*>, CompareHuffmanNodes>
        prio_queue;
    /*
     * El orden en el que el iterador itere sobre las entries del mapa
     * influye en la naturaleza de la codificación, pero sigue siendo la
     * misma en verdad, no asustarse.
     */
    for (auto entry : frequencies) {
        prio_queue.push(
            new Node(entry.first, entry.second, nullptr, nullptr, nullptr));
    }

    while (prio_queue.size() > 1) {
        /* Extraemos los dos nodos de menor peso */
        Node* node_l = prio_queue.top();
        prio_queue.pop();
        Node* node_r = prio_queue.top();
        prio_queue.pop();

        /* Los combinamos */
        Node* node_combined = new Node(node_l, node_r);

        /* Lo reingresamos a la priority queue */
        prio_queue.push(node_combined);
    }

    Node* root = prio_queue.top();
    return root;
}

Node* generate_huffman_tree(std::unordered_map<char, Code>& code_map) {

    Node* root = new Node(0, 0, nullptr, nullptr, nullptr);
    for (auto entry : code_map) {
        auto symbol = entry.first;
        auto code = entry.second;
        size_t total_bits = 0;
        _generate_huffman_tree(symbol, code, root, code.code[0], total_bits);
    }
    return root;
}

void _generate_huffman_tree(char symbol, Code& code, Node* node, size_t bit,
                            size_t& total_bits) {

    Node* child;
    if (bit) {
        if (!node->right) {
            node->right = new Node(symbol, 0, nullptr, nullptr, nullptr);
        }
        child = node->right;
    } else {
        if (!node->left) {
            node->left = new Node(symbol, 0, nullptr, nullptr, nullptr);
        }
        child = node->left;
    }

    total_bits += 1;
    if (code.length != total_bits) {
        _generate_huffman_tree(symbol, code, child, code.code[total_bits],
                               total_bits);
    }
}

void increment(std::bitset<255>& bitset) {
    for (size_t i = 0; i < bitset.size(); ++i) {
        // std::cout << bitset[n_bits - 1 - i] << '\n';
        bitset[i] = bitset[i] ^ 1;
        if ((bitset[i]) == 1)
            return;
    }
    /*
     * Si es que llegamos a este punto, en verdad no tengo nada que ofrecer, ya
     * que es un bitset estático. En cualquier caso, no debería pasar.
     */
    // bitset.push_back(1);
}
