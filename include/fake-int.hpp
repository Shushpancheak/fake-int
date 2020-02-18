#ifndef FAKE_INTEGER
#define FAKE_INTEGER

#include <iostream>
#include <vector>
#include <string>

#ifdef _MSC_VER
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif

namespace details {

// The type which size is enough for storing +-2 * digit
// in the maximum base 10^n for this type (which will be calculated later).
#define DIGIT_TYPE     int32_t
// A type that is capable of holding digit * digit.
#define MUL_DIGIT_TYPE int64_t
// Since c++11 compiler doesn't support proper constexprs, i'm just hard coding
// the maximum base for the int32_t.
#define DIGIT_BASE     static_cast<int>(1e9)

enum AnalyzerConstants {
  OPERATIONS_MONITORED = 5,

  LESS_AND_GREATER       = 0,
  EQUAL                  = 1,
  CONSTRUCTORS_AND_OP_EQ = 2,
  ARITHMETIC             = 3,
  OTHER                  = 4
};

struct QuotientAndRemainder;

// Fill trailing zeros so that the total length will be len
void FillTrailingZeros(std::vector<DIGIT_TYPE>& vec, size_t len);
void FillTrailingZeros(std::string& str,             size_t len);
void FillFirstZeros(std::vector<DIGIT_TYPE>& vec, size_t len);
void FillFirstZeros(std::string& str,             size_t len);
void DeleteTrailingZeros(std::vector<DIGIT_TYPE>& vec);

}


class FakeInteger {
public:
  FakeInteger();
  FakeInteger(int integer);
  FakeInteger(const FakeInteger& bigint) = default;
  FakeInteger(FakeInteger&& bigint) noexcept;
  ~FakeInteger() = default;

  FakeInteger  operator- ();
  FakeInteger& operator= (const FakeInteger&  bigint) = default;
  FakeInteger& operator= (      FakeInteger&& bigint) noexcept;
  FakeInteger& operator+=(const FakeInteger&  bigint);
  FakeInteger& operator-=(const FakeInteger&  bigint);
  FakeInteger& operator/=(const FakeInteger&  bigint);
  FakeInteger& operator*=(const FakeInteger&  bigint);
  FakeInteger& operator%=(const FakeInteger&  bigint);
  FakeInteger& operator++();
  FakeInteger  operator++(int);
  FakeInteger& operator--();
  FakeInteger  operator--(int);

  // ReSharper disable once CppInconsistentNaming
  std::string toString() const;

  explicit operator int() const;
  explicit operator bool() const;

  friend FakeInteger operator+(const FakeInteger& val_1, const FakeInteger& val_2);
  friend FakeInteger operator-(const FakeInteger& val_1, const FakeInteger& val_2);
  friend FakeInteger operator/(const FakeInteger& val_1, const FakeInteger& val_2);
  friend FakeInteger operator*(const FakeInteger& val_1, const FakeInteger& val_2);
  friend FakeInteger operator%(const FakeInteger& val_1, const FakeInteger& val_2);

  friend bool operator==(const FakeInteger& val_1, const FakeInteger& val_2);
  friend bool operator!=(const FakeInteger& val_1, const FakeInteger& val_2);
  friend bool operator> (const FakeInteger& val_1, const FakeInteger& val_2);
  friend bool operator< (const FakeInteger& val_1, const FakeInteger& val_2);
  friend bool operator>=(const FakeInteger& val_1, const FakeInteger& val_2);
  friend bool operator<=(const FakeInteger& val_1, const FakeInteger& val_2);

  friend std::istream&  operator>>(std::istream&  stream,       FakeInteger& bigint);
  friend std::istream&& operator>>(std::istream&& stream,       FakeInteger& bigint);
  friend std::ostream&  operator<<(std::ostream&  stream, const FakeInteger& bigint);
  friend std::ostream&& operator<<(std::ostream&& stream, const FakeInteger& bigint);

  // Static stuff.
  static size_t current_number_of_operations[details::OPERATIONS_MONITORED];

private:
  static const DIGIT_TYPE BASE = DIGIT_BASE;

  // Used both in operator+= and -=, since we don't want to
  // copy-paste code and to copy object (with unary -) to pass
  // to operator+= in operator-=..
  void Add(const FakeInteger& bigint, bool is_bigint_negative);

  // Used in division and module to reduce copy-pasting...
  friend details::QuotientAndRemainder Divide(const FakeInteger& dividend,
                                              const FakeInteger& divisor);
  // Finds a digit such that dividend - digit * divisor >= 0, but dividend -
  // - (digit + 1) * divisor < 0 via bin search.
  friend DIGIT_TYPE FindMinimalDigitForDivision(const FakeInteger& dividend,
                                                const FakeInteger& divisor);

  // Split number in two
  std::pair<FakeInteger, FakeInteger> SplitNumber(const FakeInteger& bigint,
                                                  size_t digit_ind);
  // Does not affect the sign.
  FakeInteger KaratsubaMultiply(const FakeInteger& bigint_1, const FakeInteger& bigint_2);

  bool is_negative_;
  // Starting from least significant digit.
  std::vector<DIGIT_TYPE> digits_;
};

#endif
