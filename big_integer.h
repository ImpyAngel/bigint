#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <iosfwd>
#include <stdint.h>
#include <vector>
#include <limits>
struct big_integer
{
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
    explicit big_integer(std::string const& str);
    ~big_integer();

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);
private:
    bool sign;
    std::vector <uint64_t> data;
    inline friend big_integer mul_long_short(big_integer const & first, uint64_t x);
    const static uint8_t DEGREEBASE = 30;
    const static uint64_t BASE = static_cast<uint64_t>(1 << DEGREEBASE);
    big_integer& flip();
    inline big_integer& unsigned_add(big_integer const& other);
    inline big_integer& unsigned_sub(big_integer const& rhs, bool reverse = false);
    big_integer& binaryOp(big_integer const & other, uint64_t (* f)(uint64_t first, uint64_t second));
    inline void remove_zeroes();
    inline big_integer& add_long_short(uint64_t);
    int compare_to(big_integer const& other) const;
    int compare_by_abs(big_integer const &other) const;
    bool is_zero() const;
    uint64_t div_long_short(uint64_t x);
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);

#endif // BIG_INTEGER_H