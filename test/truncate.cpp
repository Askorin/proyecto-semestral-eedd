#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

//Función que permite "cortar" un archivo a un tamaño máximo de bytes
void truncate(std::string& filename, std::string& truncatedFilename, std::streamsize maxSize) {

    // Se abre el archivo en modo binario para lectura
    std::fstream fin(filename, std::fstream::in | std::fstream::binary);
    if (!fin) {
        std::cerr << "No se pudo abrir el archivo original para lectura: " << filename << std::endl;
        return;
    }

    // Se lee el contenido hasta maxSize bytes
    std::string content;
    content.resize(maxSize);
    fin.read(&content[0], maxSize);
    std::streamsize bytesRead = fin.gcount();
    fin.close();

    // Se abre el archivo nuevo en modo binario para escritura
    std::ofstream fout(truncatedFilename, std::ios::out | std::ios::binary);
    if (!fout) {
        std::cerr << "No se pudo abrir el archivo nuevo para escritura: " << truncatedFilename << std::endl;
        return;
    }

    // Se escribe el contenido truncado en el archivo nuevo
    fout.write(content.c_str(), bytesRead);
    fout.close();

    std::cout << "Se ha creado una copia truncada del archivo " << filename << " a " << maxSize << " bytes" << std::endl;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <nombre_archivo>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    filename = "../data/"  + filename;

    if (!std::filesystem::exists(filename)) {
        std::cerr << "No se encontró el archivo: " << filename << std::endl;
        return 1;
    }
    
    std::vector<int> sizes = {
        1024,       // 1kB
        10240,      // 10kB
        25600 ,     // 25kB
        51200,      // 50kB
        76800,      // 75kB
        102400,     // 100kB
        256000,     // 250kB
        512000,     // 500kB
        768000,     // 750kB
        1048576,    // 1MB
        5242880,    // 5MB
        10485760,   // 10MB
    };

    for (auto size : sizes) {
        std::string truncatedFilename;
        if (size >= 1024 && size < 1048576) {
            truncatedFilename = filename + "_" + std::to_string(size / 1024) + "kB.txt";
        } else if (size >= 1048576) {
            truncatedFilename = filename + "_" + std::to_string(size / 1048576) + "MB.txt";
        }
        truncate(filename, truncatedFilename, size);
    }
    
    return 0;
}
