#include "big_integer.h"

#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <stdint.h>

big_integer::big_integer() {
    sign = true;
    data.resize(1);
}

big_integer::big_integer(big_integer const &other) {
    this->data = other.data;
    this->sign = other.sign;
}

big_integer::big_integer(int a) {
    int64_t r = a;
    sign = true;
    if (r < 0) {
        sign = false;
        r = -r;
    }
    if (static_cast<uint64_t>(r) < BASE) {
        data.resize(1);
        data[0] = static_cast<uint64_t>(r);
    } else {
        data.resize(2);
        data[1] = static_cast<uint64_t>(r >> DEGREEBASE);
        data[0] = r - (data[1] << DEGREEBASE);
    }
}

big_integer::big_integer(std::string const &str) : big_integer() {
    size_t beginIndex = 0;
    if (str[0] == '-') {
        this->sign = false;
        beginIndex = 1;
    }

    for (size_t i = beginIndex; i < str.size(); i++) {
        int digit = str[i] - (char) '0';
        *this = mul_long_short(*this, 10);
        this->add_long_short(digit);
    }
    remove_zeroes();
}

big_integer::~big_integer() {
    this->data.clear();
}

big_integer &big_integer::operator=(big_integer const &other) {
    this->data = other.data;
    this->sign = other.sign;

    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    if (this->sign == rhs.sign) return this->unsigned_add(rhs);
    if (this->compare_by_abs(rhs) >= 0) {
        return this->unsigned_sub(rhs, false);
    }
    else {
        this->sign = !this->sign;
        return this->unsigned_sub(rhs, true);
    }
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    if (this->sign != rhs.sign) return this->unsigned_add(rhs);
    if (this->compare_by_abs(rhs) >= 0) {
        return this->unsigned_sub(rhs, false);
    }
    else {
        this->sign = !this->sign;
        return this->unsigned_sub(rhs, true);
    }
}

big_integer &big_integer::unsigned_sub(big_integer const &rhs, bool reverse) {
    uint64_t carry = 0;
    size_t lim = reverse ? this->data.size() : rhs.data.size();
    for (size_t i = 0; i < lim || carry; i++) {
        int64_t a;
        if (!reverse)
            a = (int64_t)(this->data[i]) - carry -
                static_cast<int64_t>(i < rhs.data.size() ? rhs.data[i] : 0);
        else
            a = static_cast<int64_t>( rhs.data[i]) - carry -
                static_cast<int64_t>(i < this->data.size() ? this->data[i] : 0);
        if (a < 0) {
            carry = 1;
            a += BASE;
        }
        else {
            carry = 0;
        }
        this->data[i] = static_cast<uint64_t>(a);
    }

    remove_zeroes();
    return *this;
}

big_integer &big_integer::unsigned_add(big_integer const &other) {
    uint64_t carry = 0;
    size_t const ma = std::max(this->data.size(), other.data.size());
    for (size_t i = 0; i < ma || carry; i++) {
        if (i == this->data.size()) this->data.push_back(0);
        uint64_t a = (this->data[i]) +
                     (i < other.data.size() ? other.data[i] : 0) + carry;
        carry = a >> DEGREEBASE;
        this->data[i] = a - (carry << DEGREEBASE);
    }
    while (this->data.size() > 1 && this->data.back() == 0) {
        this->data.pop_back();
    }
    return *this;

}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    this->sign = this->sign == rhs.sign;

    uint64_t t, mulcarry = 0, addcarry = 0;

    for (ptrdiff_t i = this->data.size() - 1; i >= 0; i--) {
        mulcarry = 0;
        uint64_t mul = this->data[i];
        for (size_t j = 0; j < rhs.data.size(); j++) {
            t = mul * static_cast<int64_t>(rhs.data[j]) + mulcarry;
            mulcarry = t >> DEGREEBASE;
            t = t - (mulcarry << DEGREEBASE);

            if (j == 0) this->data[i] = 0;
            size_t k = i + j;
            if (k == this->data.size()) this->data.push_back(0);
            uint64_t tmp2 = static_cast<uint64_t>(this->data[k] + t);
            addcarry = tmp2 >> DEGREEBASE;
            tmp2 = tmp2 - (addcarry << DEGREEBASE);

            this->data[k] = (tmp2);

            while (addcarry != 0) {
                k++;
                if (k == this->data.size()) this->data.push_back(0);
                tmp2 = static_cast<uint64_t> (this->data[k] + addcarry);
                addcarry = tmp2 >> DEGREEBASE;
                this->data[k] = tmp2 - (addcarry << DEGREEBASE);
            }
        }

        size_t k = i + rhs.data.size();
        while (mulcarry != 0) {
            if (k == this->data.size()) this->data.push_back(0);
            uint64_t tmp2 = static_cast<uint64_t> (this->data[k] + mulcarry);
            mulcarry = tmp2 >> DEGREEBASE;
            this->data[k] = tmp2 - (mulcarry << DEGREEBASE);
            k++;
        }
    }
    remove_zeroes();

    return *this;
}
big_integer zero = big_integer(0);
big_integer one = big_integer(1);
big_integer &big_integer::operator/=(big_integer const &rhs) {
    if (this->compare_by_abs(rhs) < 0) {
        return *this = zero;
    }
    if (rhs.data.size() == 1) {
        this->sign = this->sign == rhs.sign;
        uint64_t temp = rhs.data[0];
        this->div_long_short(temp);
        return *this;
    }
    big_integer del = big_integer(rhs);
    bool sign = this->sign == rhs.sign;
    std::vector<uint64_t> ans;
    uint64_t m = rhs.data.size();
    big_integer temp = big_integer();
    this->sign = true;
    del.sign = true;
    temp.data.resize(m);
    for (size_t i = 0; i < m; i++) {
        temp.data[i] = this->data[i + this->data.size() - m];
    }
    int64_t j = this->data.size() - m;
    while (true) {
        if (temp < rhs) {
            ans.push_back(0);
        } else {
            uint64_t l = 0;
            uint64_t r = BASE;
            while (r - 1 > l) {
                uint64_t mm = (r + l) / 2;
                if (mul_long_short(del, mm) <= temp) {
                    l = mm;
                } else {
                    r = mm;
                }
            }
            ans.push_back(l);
            temp -= mul_long_short(del, l);

        }
        temp <<= DEGREEBASE;

        if (j > 0) {
            temp.add_long_short(this->data[--j]);
        } else break;
    }
    std::reverse(ans.begin(), ans.end());
    this->data = ans;
    this->sign = sign;
    remove_zeroes();
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return *this -= ((*this / rhs) * rhs);
}

uint64_t my_and(uint64_t first, uint64_t second) {
    return first & second;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    *this = this->binaryOp(rhs, (&my_and));
    this->sign = this->sign | rhs.sign;
    return *this;
}

uint64_t my_or(uint64_t first, uint64_t second) {
    return first | second;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    *this = this->binaryOp(rhs, (&my_or));
    this->sign = this->sign & rhs.sign;
    if (!this->sign) this->add_long_short(1);
    return *this;
}

uint64_t my_xor(uint64_t first, uint64_t second) {
    return first ^ second;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    *this = this->binaryOp(rhs, (&my_xor));
    if (rhs.sign ^ this->sign) this->add_long_short(1);
    this->sign = this->sign == rhs.sign;
    return *this;
}

big_integer &big_integer::operator<<=(int rhs) {
    uint64_t first = static_cast<uint64_t>(rhs / DEGREEBASE);
    uint64_t second = static_cast<uint64_t>(rhs) - (first * DEGREEBASE);
    if (*this == zero) return *this;
    if (!this->sign) {
        *this -= one;
    }
    big_integer before(*this);
    this->data.resize(before.data.size() + first);
    uint64_t carry = 0;
    for (size_t i = 0; i < this->data.size(); i++) {
        if (i < first) {
            this->data[i] = 0;
        } else {
            uint64_t temp = before.data[i - first] << second;
            uint64_t tempCarry = temp >> DEGREEBASE;
            uint64_t tempReal = temp - (tempCarry << DEGREEBASE);
            this->data[i] = tempReal + carry;
            carry = tempCarry;
        }
    }
    if (carry != 0) {
        this->data.push_back(carry);
    }
    if (!this->sign) {
        *this += one;
    }
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    uint64_t first = static_cast<uint64_t>(rhs / DEGREEBASE);
    uint64_t second = static_cast<uint64_t>(rhs) - (first * DEGREEBASE);
    if (!this->sign) {
        *this += one;
    }
    big_integer before(*this);
    for (size_t i = 0; i < first && !this->data.empty(); i++) this->data.pop_back();
    if (this->data.empty()) {
        *this = zero;
    } else {
        uint64_t carry = 0;
        for (int i = ((this->data.size()) - 1); i >= 0; i--) {
            uint64_t temp = before.data[i + first];
            this->data[i] = (temp >> second) + (carry << (DEGREEBASE - second));
            carry = temp - ((temp >> second) << second);
        }
    }
    if (!this->sign) {
        *this -= one;
    }
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer r(*this);
    r.sign = !r.sign;
    return r;
}

big_integer big_integer::operator~() const {
    big_integer r(*this);
    r += 1;
    r.sign = !r.sign;
    return r;
}

big_integer &big_integer::operator++() {
    this->operator+=(1);
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &big_integer::operator--() {
    this->operator-=(1);
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return a.compare_to(b) == 0;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return a.compare_to(b) != 0;
}

bool operator<(big_integer const &a, big_integer const &b) {
    return a.compare_to(b) < 0;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return a.compare_to(b) > 0;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return a.compare_to(b) <= 0;
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return a.compare_to(b) >= 0;
}

std::string to_string(big_integer const &a) {
    if (a.is_zero()) return "0";
    big_integer x(a);
    std::string res = "";
    while (!x.is_zero()) {
        int digit = x.div_long_short(10);
        res.push_back(static_cast<char>(digit) + (char) '0');
    }
    while (res[res.size() - 1] == '0') {
        res.pop_back();
    }
    if (!x.sign) res.push_back('-');

    std::reverse(res.begin(), res.end());
    return res;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}

int big_integer::compare_to(big_integer const &other) const {
    if (this->is_zero() && other.is_zero()) return 0;
    if (this->sign && !other.sign) return 1;
    if (!this->sign && other.sign) return -1;
    if (this->sign && other.sign) {
        if (this->data.size() > other.data.size()) return 1;
        if (this->data.size() < other.data.size()) return -1;
        for (size_t i = this->data.size(); i > 0; i--) {
            if (this->data[i - 1] > other.data[i - 1]) return 1;
            if (this->data[i - 1] < other.data[i - 1]) return -1;
        }
    } else {
        if (this->data.size() > other.data.size()) return -1;
        if (this->data.size() < other.data.size()) return 1;
        for (size_t i = this->data.size(); i > 0; i--) {
            if (this->data[i - 1] > other.data[i - 1]) return -1;
            if (this->data[i - 1] < other.data[i - 1]) return 1;
        }
    }
    return 0;
}

int big_integer::compare_by_abs(big_integer const &other) const {
    if (this->data.size() > other.data.size()) return 1;
    if (this->data.size() < other.data.size()) return -1;
    for (size_t i = this->data.size(); i > 0; i--) {
        if (this->data[i - 1] > other.data[i - 1]) return 1;
        if (this->data[i - 1] < other.data[i - 1]) return -1;
    }
    return 0;
}

bool big_integer::is_zero() const {
    return (this->data.size() == 1 && this->data[0] == 0);
}

big_integer &big_integer::binaryOp(big_integer const &other, uint64_t (*f)(uint64_t first, uint64_t second)) {
    big_integer first = big_integer(*this);
    big_integer second = big_integer(other);
    if (second.data.size() > first.data.size()) first.data.resize(second.data.size());
    if (second.data.size() < first.data.size()) second.data.resize(first.data.size());
    if (!first.sign) {
        first.flip();
        first.add_long_short(1);
    }
    if (!second.sign) {
        second.flip();
        second.add_long_short(1);
    }
    for (size_t i = 0; i < (first.data.size()); i++) {
        first.data[i] = (*f)(first.data[i], second.data[i]);
    }
    *this = first;
    if ((*f)(static_cast<uint64_t>(!other.sign), static_cast<uint64_t>(!this->sign))) *this = first.flip();
    remove_zeroes();
    return *this;
}

big_integer &big_integer::flip() {
    for (size_t i = 0; i < this->data.size(); i++) {
        this->data[i] ^= (BASE - 1);
    }
    return *this;
}

big_integer &big_integer::add_long_short(uint64_t const x) {
    uint64_t tmp, carry = 0;
    tmp = this->data[0] +  x;
    carry = tmp / BASE;
    this->data[0] = (tmp % BASE);
    size_t k = 1;
    while (carry != 0) {
        if (k == this->data.size()) data.push_back(0);
        tmp = this->data[k] + carry;
        carry = tmp / BASE;
        data[k] = (tmp % BASE);
        k++;
    }
    return *this;
}

big_integer mul_long_short(big_integer const &first, uint64_t const x) {
    uint64_t tmp, carry = 0;
    big_integer ans = big_integer(first);
    for (size_t i = 0; i < first.data.size(); i++) {
        tmp = first.data[i] *  x + carry;
        carry = tmp >> big_integer::DEGREEBASE;
        ans.data[i] = (tmp - (carry << big_integer::DEGREEBASE));

    }
    size_t k = first.data.size();
    while (carry != 0) {
        if (k == ans.data.size()) ans.data.push_back(0);
        tmp =  ans.data[k] + carry;
        carry = tmp >> big_integer::DEGREEBASE;
        ans.data[k] = (tmp - (carry << big_integer::DEGREEBASE));
        k++;
    }
    return ans;
}

void big_integer::remove_zeroes() {
    while (this->data.size() > 1 && this->data.back() == 0) {
        this->data.pop_back();
    }
}

uint64_t big_integer::div_long_short(uint64_t x) {
    uint64_t carry = 0;
    for (size_t i = this->data.size(); i > 0; --i) {
        uint64_t temp = (this->data[i - 1]) + carry * BASE;
        this->data[i - 1] = (temp / x);
        carry = temp %  x;
    }
    remove_zeroes();
    return (carry);
}