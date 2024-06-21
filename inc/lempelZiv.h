#ifndef LEMPEL_ZIV_H
#define LEMPEL_ZIV_H

#include <string>
#include <array>

// Typedefs para optimizar la cantidad de bytes que ocupa cada variable.
typedef int position_type;
typedef short length_type;

void guardar_par(position_type, length_type, std::string);
void comprimir(std::string, int, std::string);
std::string descomprimir(std::string);

#endif
