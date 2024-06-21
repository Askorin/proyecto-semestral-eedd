#include "../inc/huffman.h"
#include <array>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <ostream>
#include <queue>
#include <stack>

bool CompareHuffmanNodes::operator()(Node* node_l, Node* node_r) {
    return node_l->freq > node_r->freq;
}

bool CompareCodes::operator()(
    std::pair<unsigned char, unsigned char> symbol_l,
    std::pair<unsigned char, unsigned char> symbol_r) {
    /* Si la longitud del código es la misma, comparamos por orden de su código
     * ascii. */
    if (symbol_l.second == symbol_r.second)
        return symbol_l.first > symbol_r.first;
    /* Pero en verdad la prioridad es la longitud del código */
    else
        return symbol_l.second > symbol_r.second;
}

Node::Node(unsigned char symbol, size_t freq, Node* left, Node* right,
           Node* parent)
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

Code::Code(std::bitset<MAX_CODE_SIZE> code, unsigned char length)
    : code(code), length(length) {}

Code::Code() : code(std::bitset<MAX_CODE_SIZE>()), length(0) {}

Code Code::get_reversed() {
    std::bitset<MAX_CODE_SIZE> reversed;
    std::bitset<MAX_CODE_SIZE> to_reverse = code;
    for (size_t i = 0; i < length; ++i) {
        reversed[length - 1 - i] = to_reverse[0];
        to_reverse >>= 1;
    }
    code = reversed;
    return Code(reversed, length);
}

void encode_file(std::string& file_name_input, std::string& file_name_output) {
    size_t message_len = 0;
    auto frequencies =
        calculate_frequencies_from_file(file_name_input, message_len);
    // std::cout << "Message length: " << message_len << '\n';
    // std::cout << message_len << ";";
    Node* root = generate_huffman_tree(frequencies);

    /* Mapa que guardará las longitudes de los códigos de cada símbolo, esto es
     * en verdad lo único que importa. Una vez tengamos estos, podremos
     * construir los códigos canónicos.
     */
    std::array<unsigned char, CHAR_NUM> code_length_map{0};

    /*
     * Mapa que guardará la frecuencia de una longitud de código dada, utilizado
     * para Huffman canónico
     */
    std::array<unsigned char, CHAR_NUM> length_frequency_map{0};

    /* Longitud total del mensaje codificado */
    size_t total_len = 0;
    /* Máxima longitud de los códigos */
    traverse_huffman_tree(root, code_length_map, total_len,
                          length_frequency_map);

    /* Este vector servirá para guardar los símbolos en orden el de las
     * longitudes de sus códigos */
    std::vector<unsigned char> ordered_symbols;

    /*
     * Generamos los códigos canónicos
     */
    std::array<Code, CHAR_NUM> canonical_codes =
        get_canonical_codes(code_length_map, ordered_symbols);

    size_t max_code_length = canonical_codes[ordered_symbols.back()].length;
    /* Codificamos */
    std::ifstream fin(file_name_input, std::fstream::in | std::fstream::binary);

    std::ofstream fout(file_name_output, std::ofstream::out |
                                             std::ofstream::trunc |
                                             std::ofstream::binary);

    // std::cout << "Unique symbols: " << ordered_symbols.size() << '\n';
    /*
     * Para el primer tipo de header se guardan las longitudes de los códigos
     * para TODOS los símbolos del alfabeto.
     * */
    size_t header_type_1_size = CHAR_NUM;
    /*
     * Para el segundo tipo se guardan las frecuencias de cada longitud de
     * código y luego los símbolos ordenados con respecto a longitud de código.
     * Se deben guardar las frecuencias de longitudes para cada longitud menor o
     * igual que la máxima, eso incurre 1 * max_length bytes. Luego,
     * evidentemente, para guardar los símbolos se necesita el tamaño del vector
     * de ellos en bytes. Además se incluye un separador de un byte entre las
     * dos.
     */
    size_t header_type_2_size = max_code_length + ordered_symbols.size() + 1;

    int header_type = (header_type_1_size <= header_type_2_size) ? 1 : 2;

    // std::cout << "Usando tipo: " << header_type << '\n';

    save_header(fout, canonical_codes, length_frequency_map, ordered_symbols,
                message_len, header_type);
    save_code(fout, fin, canonical_codes);

    fin.close();
    fout.close();
}

std::array<Code, CHAR_NUM>
get_canonical_codes(std::array<unsigned char, CHAR_NUM>& code_length_map,
                    std::vector<unsigned char>& ordered_symbols) {

    /* Primero tenemos que ordenar los códigos, usamos una priority queue. */
    std::priority_queue<std::pair<unsigned char, unsigned char>,
                        std::vector<std::pair<unsigned char, unsigned char>>,
                        CompareCodes>
        prio_queue;

    for (size_t c = 0; c < CHAR_NUM; ++c) {
        if (code_length_map[c]) {
            prio_queue.push(std::pair((unsigned char)c, code_length_map[c]));
        }
    }

    std::array<Code, CHAR_NUM> canonical_codes{Code()};

    std::bitset<MAX_CODE_SIZE> code;
    /*
     * Al primer símbolo se le asigna un código de la misma longitud, compuesto
     * únicamente por ceros.
     */

    /*
     * La razón por la que damos vuelta el código es que los número se
     * escriben desde bit más significativo a menos significativo, pero un
     * prefijo se chequea desde más significativo a menos significativo. A
     * lo que quiero llegar es que si ingresaramos los códigos en el estado
     * en que los conseguimos, a la hora de decodificar, los recorreríamos
     * de bit MENOS SIGNIFICATIVO A MÁS SIGNIFICATIVO ¿Qué significa esto?
     * Que no puedo saber si algo es prefijo de algo con esa información
     * (aunque se podría sufijo) A modo de ejemplo, ver los códigos:
     *
     * 00, 01, 100
     *
     * Definitivamente son prefix-free, pero a la hora de escribirlos por el
     * bitstream, quedaría algo: 00 01 100
     *
     * Ok, ahora para leerlo en caso de decodificación, vamos de bit menos
     * significativo (derecha) a más significativo (izquierda)
     * Chequeamos 0, Chequeamos 0, Calza con 00!
     * Esto está mal, porque en efecto, 00 no es prefijo de 100, pero sí es
     * sufijo. Perdón si es un comentario muy largo pero esta es mi pesadilla
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
                 std::array<Code, CHAR_NUM>& canonical_codes,
                 std::array<unsigned char, CHAR_NUM>& length_frequency_map,
                 std::vector<unsigned char>& ordered_symbols,
                 size_t message_len, int header_type) {

    // std::cout << "### Guardando header ###\n";
    // std::cout << "Message len: " << message_len << '\n';
    //
    /* Indicador del tpo de header utilizado. */
    fout.write(reinterpret_cast<char*>(&header_type), 1);
    /* Siempre escribimos la longitud del mensaje en el header. */
    fout.write(reinterpret_cast<char*>(&message_len), 4);

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

    if (header_type == 1) {
        /* Longitudes de códigos de cada símbolo */
        for (size_t c = 0; c < CHAR_NUM; ++c) {
            unsigned char bit_length = 0;
            /* Si el carácter tiene un código de longitud no zero */
            if (canonical_codes[(unsigned char)c].length) {
                bit_length = canonical_codes[(unsigned char)c].length;
            }

            fout.write(reinterpret_cast<char*>(&bit_length),
                       sizeof(bit_length));
        }
    }

    /*
     * Método 2, útil cuando no se está utilizando todo el alfabeto. Se
     * guarda primero la frecuencias de longitudes de códigos, hasta la más
     * alta. Por ejemplo, de tener los códigos 110, 1110, 1110 se guarda
     *
     * (0, 0, 1, 2)
     *
     * Adicionalmente, se guardan los carácteres en orden de sus longitudes de
     * código, y en segunda preferencia, orden lexicográfico. (como estamos
     * trabajando con binario, simplemente chequeamos valor). Por ejemplo, si
     * los códigos anteriores les correspondían a (D, B, A), respectivamente, se
     * guarda: (D, A, B).
     * De esta manera, el header final quedaría (0, 0, 1, 2) SEP (D, A, B).
     * Se puede reconstruir con facilidad un mapa de símbolo->longitud de
     * código.
     */

    if (header_type == 2) {
        /*
         * La longitud de código máxima claramente le corresponde al último
         * elemento de ordered_symbols
         */
        unsigned char max_code_length =
            canonical_codes[ordered_symbols.back()].length;
        /* Longitudes de códigos de cada símbolo */

        for (size_t c = 1; c <= max_code_length; ++c) {
            unsigned char length_frequency = length_frequency_map[c];
            fout.write(reinterpret_cast<char*>(&length_frequency),
                       sizeof(length_frequency));
        }

        /*
         * Te podrías preguntar, ¿Puede que exista una longitud de código con
         * frecuencia 255? La respuesta es que para que eso ocurriera, todos
         * los símbolos del alfabeto deberían tener la misma frecuencia, de ser
         * el caso, este método no estaría siendo utilizado, ya que valdría
         * menos espacio guardar la longitud de cada código como en el método 1.
         * En el primer caso. Por esto es que es seguro utilizar un separador
         * 0xFF = 255.
         */
        unsigned char sep = 0xFF;
        fout.write(reinterpret_cast<char*>(&sep), sizeof(sep));

        for (unsigned char c : ordered_symbols) {
            fout.write(reinterpret_cast<char*>(&c), sizeof(c));
        }
    }
}

void save_code(std::ofstream& fout, std::ifstream& fin,
               std::array<Code, CHAR_NUM>& canonical_codes) {

    // std::cout << "### Guardando código ###\n";

    std::bitset<MAX_CODE_SIZE> ulong_bitset = 0xFFFFFFFFFFFFFFFF;
    unsigned char buffer = 0;
    unsigned char buffer_size = 0;
    unsigned char c;

    while (fin.read(reinterpret_cast<char*>(&c), sizeof(c))) {
        auto code = canonical_codes[c].code;
        auto bits = canonical_codes[c].length;

        while (bits) {
            buffer |= (code & ulong_bitset).to_ulong() << buffer_size;
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

void decode_file(std::string& file_name_encoded,
                 std::string& file_name_output) {

    std::ifstream fin(file_name_encoded,
                      std::ifstream::in | std::ifstream::binary);

    // std::cout << "### Empezando a decodificar ###\n";

    /*
     * Primero se debe extraer el tipo de haader, y luego la longitud del
     * mensaje.
     */

    /* Igualar a cero es importante. */
    int header_type = 0;
    size_t message_len = 0;
    fin.read(reinterpret_cast<char*>(&header_type), 1);
    fin.read(reinterpret_cast<char*>(&message_len), 4);
    // std::cout << "### Longitud del mensaje: " << message_len << "###\n";

    /* Ahora reconstruiremos el mapa de longitudes de código para cada
     * carácter
     */
    std::array<unsigned char, CHAR_NUM> code_lengths{0};

    if (header_type == 1) {
        for (size_t c = 0; c < CHAR_NUM; ++c) {
            unsigned char bit_length = 0;
            fin.read(reinterpret_cast<char*>(&bit_length), 1);
            code_lengths[(unsigned char)c] = bit_length;
        }
    }
    if (header_type == 2) {
        std::vector<std::pair<unsigned char, unsigned char>> length_frequencies;
        unsigned char read_length_frequency = 0;
        unsigned char length = 1;
        fin.read(reinterpret_cast<char*>(&read_length_frequency), 1);
        while (read_length_frequency != 0xFF) {
            /* Guardamos la frecuencia asociada a la longitud de código leida */
            length_frequencies.push_back(
                std::pair<unsigned char, unsigned char>(length++,
                                                        read_length_frequency));
            fin.read(reinterpret_cast<char*>(&read_length_frequency), 1);
        }

        for (auto length_frequency : length_frequencies) {
            for (size_t i = 0; i < length_frequency.second; ++i) {
                unsigned char symbol = 0;
                fin.read(reinterpret_cast<char*>(&symbol), 1);
                code_lengths[symbol] = length_frequency.first;
            }
        }
    }

    /* sanity check */
    // for (size_t c = 0; c < CHAR_NUM; ++c) {
    //     if (code_lengths[c]) {
    //         std::cout << (unsigned char)c << ": " << int(code_lengths[c])
    //                   << '\n';
    //     }
    // }

    // std::cout << "### Generando códigos canónicos para decodificación ###\n";

    /* Ahora tenemos que reconstruir los códigos canónicos. */

    /*
     * Un vector que guarda los símbolos ordenados en base a sus longitudes de
     * código y luego por su orden lexicográfico
     */
    std::vector<unsigned char> ordered_symbols;
    std::array<Code, CHAR_NUM> code_map =
        get_canonical_codes(code_lengths, ordered_symbols);

    // std::cout << "### Recuperando arbol de Huffman ###\n";
    /* Regeneramos al arbol */
    Node* root = generate_huffman_tree(code_map);

    // std::cout << "### Decodificando y guardando resultados ###\n";
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

        if (bit)
            current = current->right;
        else
            current = current->left;

        if (!current) {
            std::cout << "oops\n";
        }
        current->is_leaf();
        if (current->is_leaf()) {
            ++decoded_symbols;
            fout << current->symbol;
            current = root;
            // std::cout << current->symbol;
        }
    }

    fout.close();
}

void traverse_huffman_tree(
    Node* root, std::array<unsigned char, CHAR_NUM>& code_length_map,
    size_t& total_bits,
    std::array<unsigned char, CHAR_NUM>& length_frequency_map) {

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
            code_length_map[node->symbol] = bit;
            ++length_frequency_map[bit];
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
    for (unsigned char c : coded_message) {
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

std::unordered_map<unsigned char, size_t>
calculate_frequencies_from_file(std::string& file_name, size_t& message_len) {
    std::unordered_map<unsigned char, size_t> frequencies;
    std::fstream fin(file_name, std::fstream::in | std::fstream::binary);
    unsigned char c;
    while (fin.read(reinterpret_cast<char*>(&c), sizeof(c))) {
        frequencies[c]++;
        ++message_len;
    }
    // size_t i = 0;
    // for (auto entry : frequencies) {
    //     std::cout << int(i++) << "- " << int(entry.first)
    //               << ", freq: " << int(entry.second) << '\n';
    //     ;
    // }
    return frequencies;
}

/* TODO: Manejar edge case de un carácter */
Node* generate_huffman_tree(
    std::unordered_map<unsigned char, size_t>& frequencies) {

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

    Node* root;
    if (prio_queue.size()) {
        root = prio_queue.top();
    } else {
        root = nullptr;
    }
    return root;
}

Node* generate_huffman_tree(std::array<Code, CHAR_NUM>& code_map) {

    Node* root = new Node(0, 0, nullptr, nullptr, nullptr);
    for (size_t symbol = 0; symbol < CHAR_NUM; ++symbol) {
        if (code_map[(unsigned char)symbol].length) {
            auto code = code_map[(unsigned char)symbol];
            size_t total_bits = 0;
            _generate_huffman_tree(symbol, code, root, code.code[0],
                                   total_bits);
        }
    }
    return root;
}

void _generate_huffman_tree(unsigned char symbol, Code& code, Node* node,
                            size_t bit, size_t& total_bits) {

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

void increment(std::bitset<MAX_CODE_SIZE>& bitset) {
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
