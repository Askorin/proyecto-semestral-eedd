#ifndef BITFIELD_H
#define BITFIELD_H
#include <cstddef>
#include <memory>
#include <ostream>
#include <vector>

class BitField {
  private:
    size_t n_bits_;
    std::vector<unsigned char> uint8_vec_;

  public:
    BitField(size_t);
    /* Retorna 1 si el bit fue "flipeado" con éxito, en caso contrario, 0.*/
    void set_bit(size_t);
    /* Retorna el bit en la posición pedida si es que es válido. Retorna negativo si es que no. */
    bool get_bit(size_t) const ;

    size_t n_bits() const;
    void n_bits(size_t);

};


std::ostream& operator<<(std::ostream&, const BitField&);

#endif
