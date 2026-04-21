/// @internal
/*****************************************************************************
**
**  ctl::json version 2.115
**
**  Copyright (c) 2019-2025, Nozomu Katoo. All rights reserved.
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions are
**  met:
**
**  1. Redistributions of source code must retain the above copyright notice,
**     this list of conditions and the following disclaimer.
**
**  2. Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in the
**     documentation and/or other materials provided with the distribution.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
**  IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
**  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
**  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
**  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
**  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
**  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
**  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
**  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
**  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
******************************************************************************
**/

#ifndef CTL_JSON_TEMPLATE_LIBRARY
#define CTL_JSON_TEMPLATE_LIBRARY

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <new>
#include <string>
#include <utility>
#include <vector>


#if !defined(NAMESPACE_CTLJSON)
#define NAMESPACE_CTLJSON ctl
#endif

namespace NAMESPACE_CTLJSON {
namespace string_internal_ {

template <typename T>
struct casetables {
  static const T lc[];
  static const T uc[];
};
template <typename T>
const T casetables<T>::lc[] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39,  //  "0123456789";
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a,  //  "abcdefghij"
    0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72,
    0x73, 0x74,                                      //  "klmnopqrst"
    0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x2e, 0x2b,  //  "uvwxyz.+"
    0x2d, 0x69, 0x6e, 0x66,                          //  "-inf"
    0x2d, 0x71, 0x6e, 0x61, 0x6e,                    //  "-qnan"
    0x2d, 0x73, 0x6e, 0x61, 0x6e                     //  "-snan"
};
template <typename T>
const T casetables<T>::uc[] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39,  //  "0123456789";
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a,  //  "ABCDEFGHIJ"
    0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52,
    0x53, 0x54,                                      //  "KLMNOPQRST"
    0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x2e, 0x2b,  //  "UVWXYZ.+"
    0x2d, 0x49, 0x4e, 0x46,                          //  "-INF"
    0x2d, 0x51, 0x4e, 0x41, 0x4e,                    //  "-QNAN"
    0x2d, 0x53, 0x4e, 0x41, 0x4e                     //  "-SNAN"
};

template <typename stringT>
stringT reverse_(stringT& s) {
  typedef typename stringT::size_type size_type;
  const size_type mid = s.size() / 2;

  for (size_type i = 0; i < mid; ++i) std::swap(s[i], s[s.size() - i - 1]);

  return s;
}

template <typename stringT>
int mul2_(stringT& s, const int radix) {
  typedef typename stringT::value_type char_type;
  typedef typename stringT::size_type size_type;
  char_type carried = 0;

  for (size_type i = 0; i < s.size(); ++i) {
    char_type& ch = s[i] *= 2;

    carried = ((ch = static_cast<char_type>(ch + carried)) >=
               static_cast<char_type>(radix))
                  ? (ch = static_cast<char_type>(ch - radix), 1)
                  : 0;
  }
  return carried ? (s.push_back(carried), 1) : 0;
}

template <typename stringT>
int div2_(stringT& s, const int radix) {
  typedef typename stringT::value_type char_type;
  typedef typename stringT::size_type size_type;
  int borrowed = 0;

  for (size_type i = s.size(); i;) {
    char_type& ch = s[--i];

    ch = static_cast<char_type>(ch + (borrowed ? radix : 0));

    borrowed = ch & 1;
    ch /= 2;
  }
  if (borrowed) s.insert(0, 1, static_cast<char_type>(radix / 2));

  if (s.size() && s[s.size() - 1] == 0) {
    s.resize(s.size() - 1);
    borrowed |= 2;
  }
  return borrowed;
}

template <typename stringT>
int plus_(stringT& s1, const stringT& s2, const int radix) {
  typedef typename stringT::value_type char_type;
  typedef typename stringT::size_type size_type;
  char_type carried = 0;

  if (s1.size() < s2.size()) s1.resize(s2.size(), 0);

  for (size_type i = 0; i < s1.size(); ++i) {
    char_type& ch = s1[i] += (i < s2.size() ? s2[i] : 0) + carried;
    carried = (ch >= static_cast<char_type>(radix))
                  ? (ch = static_cast<char_type>(ch - radix), 1)
                  : 0;
  }
  return carried ? (s1.push_back(carried), 1) : 0;
}

template <typename stringT>
int round_(stringT& s, typename stringT::size_type pos, const int radix) {
  typedef typename stringT::value_type char_type;

  if (pos <= s.size()) {
    char_type carried =
        (s[(pos = s.size() - pos)] >= static_cast<char_type>(radix / 2)) ? 1
                                                                         : 0;

    for (s.erase(0, pos + 1), pos = 0; carried && pos < s.size(); ++pos) {
      char_type& ch = s[pos];

      ch = static_cast<char_type>(ch + carried);
      carried = (ch >= static_cast<char_type>(radix))
                    ? (ch = static_cast<char_type>(ch - radix), 1)
                    : 0;
    }
    if (carried) return s.push_back(carried), 1;
  }
  return 0;
}

template <typename T>
static bool is_same_(const T l, const T r) {
  return std::memcmp(&l, &r, sizeof(T)) == 0;
}

}  //  namespace string_internal_

template <typename stringT, typename T>
inline stringT to_string(T value, int base = 10, const int precision = 1) {
  typedef typename stringT::value_type char_type;
  typedef typename stringT::size_type size_type;
  const char_type* const table =
      (base >= 0
           ? string_internal_::casetables<char_type>::lc
           : (base = 0 - base, string_internal_::casetables<char_type>::uc));
  stringT num;

  if (base >= 2 && base <= 36) {
    const bool minus = value < 0 ? (value = 0 - value, true) : false;

    for (; value; value /= base) {
      num.push_back(table[value % base]);
    }

    if (precision > 0 && num.size() < static_cast<size_type>(precision))
      num.append(static_cast<size_type>(precision) - num.size(), table[0]);

    if (minus) num.push_back(table[38]);
  }
  return string_internal_::reverse_(num);
}

template <typename stringT, typename T>
stringT to_string_fp(T value, int fmt, int precision) {
  typedef stringT string_type;
  typedef typename stringT::value_type char_type;
  typedef typename stringT::size_type size_type;

  const bool uc = fmt >= 'A' && fmt <= 'Z';

  const char_type* const table =
      uc ? (fmt += 'a' - 'A', string_internal_::casetables<char_type>::uc)
         : string_internal_::casetables<char_type>::lc;

  int dppos;

  if ((dppos = 38 + 1,
       string_internal_::is_same_(value, std::numeric_limits<T>::infinity())) ||
      (--dppos,
       string_internal_::is_same_(value, -std::numeric_limits<T>::infinity())))
    //		return &(uc ? "-INF" : "-inf")[dppos];
    return stringT(&table[dppos], ((dppos & 1) ? 3 : 4));

  if ((dppos = 42 + 1, string_internal_::is_same_(
                           value, std::numeric_limits<T>::quiet_NaN())) ||
      (--dppos,
       string_internal_::is_same_(value, -std::numeric_limits<T>::quiet_NaN())))
    //		return &(uc ? "-QNAN" : "-qnan")[dppos];
    return stringT(&table[dppos], ((dppos & 1) ? 4 : 5));

  if ((dppos = 47 + 1, string_internal_::is_same_(
                           value, std::numeric_limits<T>::signaling_NaN())) ||
      (--dppos, string_internal_::is_same_(
                    value, -std::numeric_limits<T>::signaling_NaN())))
    //		return &(uc ? "-SNAN" : "-snan")[dppos];
    return stringT(&table[dppos], ((dppos & 1) ? 5 : 4));

  const int base = fmt == 'a' ? (fmt = 'e', 16) : 10;
  const bool remove0s =
      (precision < 0) ? (precision = 0 - precision, true) : (fmt == 'g');
  const bool minus =
      (value < 0 || string_internal_::is_same_(value, static_cast<T>(-0.0)))
          ? (value = 0 - value, true)
          : false;
  string_type resstr;

  dppos = 0;
  if (value != 0.0) {
    int borrowed = 0;
    T mask = 1.0;
    string_type mstr;

    mstr.push_back(1);

    if (value >= mask)
      for (; (value - mask) >= mask; mask *= 2.0)
        dppos += string_internal_::mul2_(mstr, base);
    else
      for (; value <
             ((dppos -= (string_internal_::div2_(mstr, base) & 2) ? 1 : 0),
              mask /= 2.0););

    for (int count = 11; value > 0.0 && mask > 0.0 && count < 64; mask /= 2.0) {
      if (borrowed) resstr.insert(0, 1, 0);

      if (value >= mask) {
        value -= mask;
        dppos += string_internal_::plus_(resstr, mstr, base);
      }
      borrowed = string_internal_::div2_(mstr, base) & 1;
    }

    if (base == 16 && resstr.size()) {
      const char_type& lb = resstr[resstr.size() - 1];
      const int shift = (lb & 8) ? 3 : ((lb & 4) ? 2 : ((lb & 2) ? 1 : 0));

      dppos = dppos * 4 + shift;
      if (shift) {
        char_type borrowed2 = 0;
        for (size_type i = resstr.size(); i;) {
          char_type& ch = resstr[--i] |= borrowed2;
          borrowed2 = ch << 4;
          ch = static_cast<char_type>((ch >> shift) & 15);
        }
        if (borrowed2)
          resstr.insert(0, 1,
                        static_cast<char_type>((borrowed2 >> shift) & 15));
      }
    }
    //		dppos += round_(resstr, 18, base);
  }

  if (fmt == 'g') {
    dppos += string_internal_::round_(resstr, precision + 1, base);

    if (dppos >= -4 && dppos < precision) {
      precision = precision - 1 - dppos;
      dppos < 0 ? (resstr.append(static_cast<size_type>(-dppos),
                                 static_cast<char_type>(0)),
                   dppos = 1)
                : ++dppos;
      goto STYLE_F;
    }
    goto STYLE_E;
  }

  if (fmt == 'e') {
    ++precision;
    dppos += string_internal_::round_(resstr, precision + 1, base);

  STYLE_E:

    if (remove0s)
      for (size_type i = 0;
           precision > 1 && i < resstr.size() && resstr[i] == 0;
           --precision, ++i);

    if (resstr.size() > static_cast<size_type>(precision))
      resstr.erase(0, resstr.size() - precision);
    else
      resstr.insert(0, static_cast<size_type>(precision) - resstr.size(), 0);

    for (size_type i = 0; i < resstr.size(); ++i)
      resstr[i] = table[static_cast<unsigned int>(resstr[i])];

    if (resstr.size() > 1) resstr.insert(resstr.size() - 1, 1, table[36]);

    resstr.insert(0, 1, table[base == 10 ? 14 : 25]);  //  'E''e' 'P''p'
    resstr.insert(0, 1, ((dppos >= 0) ? table[37] : table[38]));

    if (dppos < 0) dppos = 0 - dppos;

    const size_type minwidth = base == 10 ? 2 : 1;
    for (size_type inspos = 0; dppos || inspos < minwidth;
         ++inspos, dppos /= 10)
      resstr.insert(inspos, 1, table[dppos % 10]);

    if (base == 16) {
      resstr.push_back(table[33]);  //  'X' 'x'
      resstr.push_back(table[0]);
    }
  } else {
    dppos < 0 ? (resstr.append(static_cast<size_type>(-dppos),
                               static_cast<char_type>(0)),
                 dppos = 1)
              : ++dppos;

    dppos += string_internal_::round_(resstr, precision + dppos + 1, base);

  STYLE_F:

    if (remove0s)
      for (size_type i = 0;
           precision > 1 && i < resstr.size() && resstr[i] == 0;
           --precision, ++i);

    precision += dppos;

    if (resstr.size() > static_cast<size_type>(precision))
      resstr.erase(0, resstr.size() - precision);
    else {
      const size_type wantedsize =
          static_cast<size_type>(remove0s ? dppos : precision);
      if (resstr.size() < wantedsize)
        resstr.insert(0, wantedsize - resstr.size(), 0);
    }

    for (size_type i = 0; i < resstr.size(); ++i)
      resstr[i] = table[static_cast<unsigned int>(resstr[i])];

    if (resstr.size() > static_cast<size_type>(dppos))
      resstr.insert(resstr.size() - dppos, 1, table[36]);
  }

  if (minus) resstr.push_back(table[38]);

  return string_internal_::reverse_(resstr);
}

template <typename stringT>
inline stringT to_string(double val, const int fmt = 'f',
                         const int precision = 6) {
  return to_string_fp<stringT>(val, fmt, precision);
}
template <typename stringT>
inline stringT to_string(float val, const int fmt = 'f',
                         const int precision = 6) {
  return to_string_fp<stringT>(val, fmt, precision);
}
template <typename stringT>
inline stringT to_string(long double val, const int fmt = 'f',
                         const int precision = 6) {
  return to_string_fp<stringT>(val, fmt, precision);
}

struct json_error {
  enum code_type {
    ec_no_error = 0,
    ec_ok = 0,
    ec_array = 2,
    ec_object = 3,
    ec_object_name = 4,
    ec_no_colon = 5,
    ec_object_value = 6,
    ec_string = 7,
    ec_literal = 8,
    ec_number = 9,
  };

  int code() const { return code_; }

  std::size_t position() const { return pos_; }

  // private:

  bool clear_() {
    pos_ = 0;
    code_ = 0;
    return false;
  }

  std::size_t pos_;
  int code_;
};

namespace json_internal_ {

template <typename T>
struct view {
  typedef T char_type;
  typedef std::size_t size_type;
  typedef const char_type* const_iterator;

  view(const char_type* const p)
      : data_(p), size_(std::char_traits<T>::length(p)) {}

  view(const char_type* const p, const size_type l) : data_(p), size_(l) {}

  template <typename StringLike>
  view(const StringLike& s) : data_(s.data()), size_(s.size()) {}

  char_type* data() { return data_; }
  const char_type* data() const { return data_; }

  size_type size() const { return size_; }

  template <typename StringLike>
  StringLike str() const {
    return StringLike(data_, size_);
  }

  template <typename StringLike>
  int compare(const StringLike& right) const {
    if (data_ == right.data()) return 0;

    const size_type count = size_ <= right.size() ? size_ : right.size();
    const int cmp = std::memcmp(data_, right.data(), count * sizeof(T));

    if (cmp != 0) return cmp;

    return size_ == right.size() ? 0 : (size_ < right.size() ? -1 : 1);
  }

  template <typename StringLike>
  bool operator==(const StringLike& right) const {
    return compare(right) == 0;
  }

  template <typename StringLike>
  bool operator!=(const StringLike& right) const {
    return !operator==(right);
  }

  const T operator[](const size_type n) const { return data_[n]; }

  const_iterator begin() const { return data_; }
  const_iterator end() const { return data_ + size_; }

 private:
  const T* data_;
  size_type size_;
};

template <typename ElemT>
class nestring {
 public:
  typedef ElemT value_type;
  typedef std::size_t size_type;
  typedef ElemT& reference;
  typedef const ElemT& const_reference;
  typedef ElemT* pointer;
  typedef const ElemT* const_pointer;
  typedef const_pointer const_iterator;
  typedef view<ElemT> view_type;

  static const size_type npos = static_cast<size_type>(-1);

 public:
  nestring() : size_(0), capacity_p1_(lbsize_p1_) {}

  nestring(const const_pointer p, const size_type len)
      : size_(0), capacity_p1_(lbsize_p1_) {
    resize(len);
    if (len) std::memcpy(buf_(), p, len * sizeof(ElemT));
  }

  nestring(const nestring& right, const size_type pos, size_type len = npos)
      : size_(0), capacity_p1_(lbsize_p1_) {
    {
      const size_type len2 = right.size_ - pos;
      if (len > len2) len = len2;
    }

    resize(len);
    if (len) std::memcpy(buf_(), right.buf_() + pos, len * sizeof(ElemT));
  }

  nestring(const const_pointer p) : size_(0), capacity_p1_(lbsize_p1_) {
    operator=(p);
  }

  nestring(const nestring& right) : size_(0), capacity_p1_(lbsize_p1_) {
    operator=(right);
  }

  nestring(const view_type v) : size_(0), capacity_p1_(lbsize_p1_) {
    assign(v.data(), v.size());
  }

  nestring& operator=(const nestring& right) {
    if (this != &right) {
      resize(right.size_);
      std::memcpy(buf_(), right.buf_(), right.size_ * sizeof(ElemT));
    }
    return *this;
  }

  nestring& operator=(const const_pointer p) {
    if (buf_() != p) {
      const std::size_t size = std::char_traits<ElemT>::length(p);

      resize(size);
      std::memcpy(buf_(), p, size * sizeof(ElemT));
    }
    return *this;
  }

#if defined(__cpp_rvalue_references)
  nestring(nestring&& right)
      : size_(right.size_), capacity_p1_(right.capacity_p1_) {
    if (!right.is_localbuf_()) {
      buffer_ = right.buffer_;
      right.buffer_ = NULL;
    } else
      std::memcpy(localbuf_, right.localbuf_, size_ * sizeof(ElemT));

    right.size_ = 0;
    right.capacity_p1_ = lbsize_p1_;
  }

  nestring& operator=(nestring&& right) {
    if (this != &right) {
      size_ = right.size_;
      capacity_p1_ = right.capacity_p1_;

      if (!is_localbuf_() && buffer_ != NULL) std::free(buffer_);

      if (!right.is_localbuf_()) {
        buffer_ = right.buffer_;
        right.buffer_ = NULL;
      } else
        std::memcpy(localbuf_, right.localbuf_, size_ * sizeof(ElemT));

      right.size_ = 0;
      right.capacity_p1_ = lbsize_p1_;
    }
    return *this;
  }
#endif

  ~nestring() {
    if (!is_localbuf_() && buffer_ != NULL) std::free(buffer_);
  }

  size_type size() const { return size_; }

  template <typename Traits, typename Alloc>
  operator std::basic_string<ElemT, Traits, Alloc>() const {
    return std::basic_string<ElemT, Traits, Alloc>(buf_(), buf_() + size_);
  }

  nestring substr(size_type pos, size_type len) const {
    return nestring(buf_() + pos, len);
  }

  view_type make_view(size_type pos, size_type len) const {
    return view_type(buf_() + pos, len);
  }

  bool operator==(const nestring& right) const {
    if (size_ != right.size_) return false;

    if (this == &right) return true;

    return std::memcmp(buf_(), right.buf_(), size_ * sizeof(ElemT)) == 0;
  }

  bool operator!=(const nestring& right) const { return !operator==(right); }
  bool operator<(const nestring& right) const { return compare(right) < 0; }

  void clear() { size_ = 0; }

  void resize(const size_type newsize) {
    if (newsize >= capacity_p1_)
      reserve(newsize);
    else if (!is_localbuf_() && newsize <= localbuf_size_)
      heap_to_local_(newsize);

    size_ = newsize;
  }

  void resize(const size_type newsize, const ElemT type) {
    size_type oldsize = size_;

    resize(newsize);

    const pointer base = buf_();
    for (; oldsize < size_; ++oldsize) base[oldsize] = type;
  }

  reference operator[](const size_type pos) { return buf_()[pos]; }

  const_reference operator[](const size_type pos) const { return buf_()[pos]; }

  void push_back(const ElemT n) {
    const size_type oldsize = size_;

    if (++size_ >= capacity_p1_) reserve(size_);

    buf_()[oldsize] = n;
  }

  void assign(const nestring& right) { operator=(right); }

  void assign(const const_pointer p, const size_type len) {
    if (p != buf_()) {
      resize(len);
      std::memcpy(buf_(), p, len * sizeof(ElemT));
    }
  }

  template <typename Iterator>
  void assign(Iterator begin, const Iterator end) {
    resize(end - begin);

    for (size_type i = 0; begin != end; ++i, ++begin)
      buf_()[i] = static_cast<value_type>(*begin);
  }

  nestring& append(const size_type len, const ElemT type) {
    resize(size_ + len, type);
    return *this;
  }

  nestring& append(const const_pointer p, const size_type len) {
    const size_type oldsize = size_;

    resize(size_ + len);
    std::memcpy(buf_() + oldsize, p, len * sizeof(ElemT));
    return *this;
  }

  nestring& append(const const_pointer p) {
    return append(p, std::char_traits<ElemT>::length(p));
  }

  template <typename Iterator>
  nestring& append(Iterator begin, const Iterator end) {
    const size_type len = end - begin;
    size_type oldsize = size_;

    resize(size_ + len);
    pointer base = buf_() + oldsize;
    for (size_type i = 0; begin != end; ++i, ++begin)
      base[i] = static_cast<value_type>(*begin);
    return *this;
  }

  nestring& append(const view_type right) {
    const size_type oldsize = size_;
    const size_type rightsize = right.size();

    resize(size_ + right.size());
    std::memcpy(buf_() + oldsize, right.data(), rightsize * sizeof(ElemT));
    return *this;
  }

  void erase(const size_type pos, const size_type len) {
    if (pos < size_) {
      size_type rmndr = size_ - pos;

      if (rmndr > len) {
        rmndr -= len;
        std::memmove(buf_() + pos, buf_() + pos + len, rmndr * sizeof(ElemT));
        size_ -= len;
      } else
        size_ = pos;

      if (!is_localbuf_() && size_ <= localbuf_size_) heap_to_local_(size_);
    }
  }

  void insert(const size_type pos, const view_type right) {
    const size_type rightsize = right.size();

    move_forwards_(pos, rightsize);
    std::memcpy(buf_() + pos, right.data(), rightsize * sizeof(ElemT));
  }

  void insert(const size_type pos, const size_type count, const ElemT ch) {
    move_forwards_(pos, count);
    pointer base = buf_() + pos;

    for (size_type i = 0; i < count; ++i) base[i] = ch;
  }

  const_pointer data() const { return buf_(); }

  const_iterator begin() const { return buf_(); }
  const_iterator end() const { return buf_() + size_; }

  int compare(size_type pos1, const size_type count1, const view_type right,
              const size_type pos2, const size_type count2) const {
    const size_type count = count1 <= count2 ? count1 : count2;
    const_pointer lbase = buf_() + pos1;
    const_pointer rbase = right.data() + pos2;
    const int cmp = std::memcmp(lbase, rbase, count * sizeof(ElemT));

    if (cmp != 0) return cmp;

    return count1 == count2 ? 0 : (count1 < count2 ? -1 : 1);
  }

  bool no_alloc_failure() const { return capacity_p1_ > 0; }

  void reserve(size_type newsize) {
    if (newsize <= maxsize_) {
      const size_type capa2 = capacity_p1_ * 2;

      if (newsize < capa2) {
        newsize = capa2;
        if (newsize > maxsize_) newsize = maxsize_;
      }

      if (is_localbuf_()) {
        const pointer newbuf =
            static_cast<pointer>(std::malloc(newsize * sizeof(ElemT)));

        if (newbuf != NULL) {
          std::memcpy(newbuf, localbuf_, size_ * sizeof(ElemT));
          buffer_ = newbuf;
          capacity_p1_ = newsize + 1;
          return;
        }
      } else {
        const pointer oldbuffer = buffer_;

        buffer_ = static_cast<pointer>(
            std::realloc(static_cast<void*>(buffer_), newsize * sizeof(ElemT)));
        if (buffer_ != NULL) {
          capacity_p1_ = newsize + 1;
          return;
        }
        std::free(oldbuffer);
      }

      //			buffer_ = NULL;
      size_ = 0;
      capacity_p1_ = 1;
    }
    throw std::bad_alloc();
  }

 private:
  void move_forwards_(const size_type pos, const size_type count) {
    const size_type oldsize = size_;

    resize(size_ + count);

    if (pos < oldsize) {
      const pointer base = buf_() + pos;

      std::memmove(base + count, base, (oldsize - pos) * sizeof(ElemT));
    }
  }

  void heap_to_local_(const size_type newsize) {
    const pointer buf = buffer_;

    std::memcpy(localbuf_, buf, newsize * sizeof(ElemT));
    std::free(buf);
    capacity_p1_ = lbsize_p1_;
  }

  enum { localbuf_size_ = 16, lbsize_p1_ = localbuf_size_ + 1 };

  bool is_localbuf_() const { return capacity_p1_ <= lbsize_p1_; }

  pointer buf_() { return !is_localbuf_() ? buffer_ : localbuf_; }
  const_pointer buf_() const { return !is_localbuf_() ? buffer_ : localbuf_; }

  union {
    pointer buffer_;
    ElemT localbuf_[localbuf_size_];
  };
  size_type size_;
  size_type capacity_p1_;

  static const size_type maxsize_ = npos / sizeof(ElemT) / 2;
};
template <typename ElemT>
const typename nestring<ElemT>::size_type nestring<ElemT>::npos;
//  nestring

template <typename ElemT>
class nevector {
 public:
  typedef ElemT value_type;
  typedef std::size_t size_type;
  typedef ElemT& reference;
  typedef const ElemT& const_reference;
  typedef ElemT* pointer;
  typedef const ElemT* const_pointer;

 public:
  nevector() : buffer_(NULL), size_(0), capacity_p1_(1) {}

  nevector(const size_type initsize)
      : buffer_(NULL), size_(0), capacity_p1_(1) {
    reserve(initsize);

    for (size_type i = 0; i < initsize; ++i) new (&buffer_[i]) ElemT();

    size_ = initsize;
  }

  nevector(const nevector& right) : buffer_(NULL), size_(0), capacity_p1_(1) {
    reserve(right.size_);

    for (size_type i = 0; i < right.size_; ++i)
      new (&buffer_[i]) ElemT(right.buffer_[i]);

    size_ = right.size_;
  }

#if defined(__cpp_rvalue_references)
  nevector(nevector&& right)
      : buffer_(right.buffer_),
        size_(right.size_),
        capacity_p1_(right.capacity_p1_) {
    right.size_ = 0;
    right.capacity_p1_ = 1;
    right.buffer_ = NULL;
  }

  nevector& operator=(nevector&& right) {
    if (this != &right) {
      if (this->buffer_ != NULL) destroy_all_(this->buffer_);

      this->size_ = right.size_;
      this->capacity_p1_ = right.capacity_p1_;
      this->buffer_ = right.buffer_;

      right.size_ = 0;
      right.capacity_p1_ = 1;
      right.buffer_ = NULL;
    }
    return *this;
  }
#endif

  nevector& operator=(const nevector& right) {
    if (this != &right) {
      resize(right.size_);
      for (size_type i = 0; i < right.size_; ++i) buffer_[i] = right.buffer_[i];
    }
    return *this;
  }

  ~nevector() {
    if (buffer_ != NULL) destroy_all_(buffer_);
  }

  size_type size() const { return size_; }

  void clear() {
    for (size_type i = 0; i < size_; ++i) buffer_[i].~ElemT();
    size_ = 0;
  }

  void resize(const size_type newsize) {
    if (newsize > size_) {
      if (newsize >= capacity_p1_) reserve(newsize);

      for (size_type i = size_; i < newsize; ++i) new (&buffer_[i]) ElemT();
    } else {
      for (size_type i = newsize; i < size_; ++i) buffer_[i].~ElemT();
    }
    size_ = newsize;
  }

  void resize(const size_type newsize, const ElemT& type) {
    if (newsize > size_) {
      if (newsize >= capacity_p1_) reserve(newsize);

      for (size_type i = size_; i < newsize; ++i) new (&buffer_[i]) ElemT(type);
    } else {
      for (size_type i = newsize; i < size_; ++i) buffer_[i].~ElemT();
    }
    size_ = newsize;
  }

  reference operator[](const size_type pos) { return buffer_[pos]; }

  const_reference operator[](const size_type pos) const { return buffer_[pos]; }

  void push_back(const_reference n) {
    const size_type oldsize = size_;

    if (++size_ >= capacity_p1_) reserve(size_);

    new (&buffer_[oldsize]) ElemT(n);
  }
  reference push_back() {
    const size_type oldsize = size_;

    if (++size_ >= capacity_p1_) reserve(size_);

    new (&buffer_[oldsize]) ElemT;
    return buffer_[oldsize];
  }

  const_reference back() const { return buffer_[size_ - 1]; }

  reference back() { return buffer_[size_ - 1]; }

  void erase(const size_type pos) {
    if (pos < size_) {
      buffer_[pos].~ElemT();
      std::memmove(static_cast<void*>(buffer_ + pos), buffer_ + pos + 1,
                   (size_ - pos - 1) * sizeof(ElemT));
      --size_;
    }
  }

  void insert(const size_type pos, const ElemT& type) {
    move_forwards_(pos, 1);

    new (&buffer_[pos]) ElemT(type);
  }
  reference insert(const size_type pos) {
    move_forwards_(pos, 1);

    new (&buffer_[pos]) ElemT;
    return buffer_[pos];
  }

  bool no_alloc_failure() const { return capacity_p1_ > 0; }

  void reserve(const size_type newsize) { reserve_<16>(newsize); }

 private:
  void move_forwards_(const size_type pos, const size_type count) {
    const size_type oldsize = size_;

    resize(size_ + count);

    if (pos < oldsize) {
      const pointer base = buffer_ + pos;

      std::memmove(static_cast<void*>(base + count), base,
                   (oldsize - pos) * sizeof(ElemT));
    }
  }

  template <const size_type minsize>
  void reserve_(size_type newsize) {
    if (newsize <= maxsize_) {
      const pointer oldbuffer = buffer_;
      const size_type capa2 = newsize >= minsize ? capacity_p1_ << 1 : minsize;

      if (newsize < capa2) {
        newsize = capa2;
        if (newsize > maxsize_) newsize = maxsize_;
      }

      buffer_ = static_cast<pointer>(
          std::realloc(static_cast<void*>(buffer_), newsize * sizeof(ElemT)));
      capacity_p1_ = newsize + 1;

      if (buffer_ != NULL) return;

      destroy_all_(oldbuffer);
      //			buffer_ = NULL;
      size_ = 0;
      capacity_p1_ = 1;
    }
    throw std::bad_alloc();
  }

  void destroy_all_(const pointer p) {
    for (size_type i = 0; i < size_; ++i) p[i].~ElemT();
    std::free(p);
  }

  pointer buffer_;
  size_type size_;
  size_type capacity_p1_;

  static const size_type maxsize_ =
      static_cast<size_type>(-1) / sizeof(ElemT) / 2;
};
//  nevector

enum rbtree_colour { rb_red, rb_black };

struct rbtree_node_base {
  typedef rbtree_node_base* pointer;
  typedef const rbtree_node_base* const_pointer;

  pointer parent;
  pointer left;
  pointer right;
  rbtree_colour colour;

  rbtree_node_base() : parent(NULL), left(NULL), right(NULL), colour(rb_red) {}
};

struct rbtree_header : public rbtree_node_base {
  std::size_t node_count;

  rbtree_header() { reset(); }

  void reset() {
    this->parent = NULL;  //  Root.
    this->left = NULL;    //  Leftmost.
    this->right = NULL;   //  Rightmost.
    node_count = 0;
  }
};

template <typename T>
struct rbtree_node : public rbtree_node_base {
  typedef rbtree_node<T>* pointer;
  typedef const rbtree_node<T>* const_pointer;

  T value;

  rbtree_node& operator=(const rbtree_node&) { return *this; }
};

template <typename T>
struct const_iterator_base {
  typedef typename rbtree_node<T>::pointer pointer;
  typedef T value_type;
  typedef std::ptrdiff_t difference_type;

  const_iterator_base() : it_(NULL) {}

  const_iterator_base(const pointer p) : it_(p) {}

  bool operator==(const const_iterator_base& right) const {
    return it_ == right.it_;
  }

  bool operator!=(const const_iterator_base& right) const {
    return it_ != right.it_;
  }

  const_iterator_base& operator++() {
    if (it_) {
      if (it_->right != NULL) {
        for (it_ = static_cast<pointer>(it_->right); it_->left != NULL;
             it_ = static_cast<pointer>(it_->left));
      } else if (it_->parent != NULL) {
        pointer p = static_cast<pointer>(it_->parent);

        for (; p && p->right == it_;) {
          it_ = p;
          p = static_cast<pointer>(p->parent);
        }
        it_ = p;
      } else
        it_ = NULL;
    }
    return *this;
  }

  const_iterator_base operator++(int) {
    const const_iterator_base r(*this);
    operator++();
    return r;
  }

  const T& operator*() const { return it_->value; }

  const T* operator->() const { return &it_->value; }

  operator pointer() const { return it_; }

 protected:
  pointer it_;
};

template <typename T>
struct iterator_base : public const_iterator_base<T> {
  typedef const_iterator_base<T> base_type;
  typedef typename base_type::pointer pointer;
  typedef typename base_type::value_type value_type;
  typedef typename base_type::difference_type difference_type;

  iterator_base() : base_type(NULL) {}

  iterator_base(const pointer p) : base_type(p) {}

  T& operator*() const { return this->it_->value; }

  T* operator->() const { return &this->it_->value; }
};

template <typename Key, typename T>
class nemap {
 public:
  typedef Key key_type;
  typedef T mapped_type;
  typedef std::pair<const Key, T> value_type;
  typedef std::size_t size_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;

  typedef iterator_base<value_type> iterator;
  typedef const_iterator_base<value_type> const_iterator;

  nemap() { entire_.reset(); }

  nemap(const nemap& right) { operator=(right); }

  ~nemap() { erase_all_(entire_.parent); }

  std::size_t size() const { return entire_.node_count; }

  void clear() {
    erase_all_(entire_.parent);
    entire_.reset();
  }

  nemap& operator=(const nemap& right) {
    clear();

    if (this != &right && right.entire_.parent != NULL)
      entire_.parent = copy_nodes_(entire_.parent, right.entire_.parent, right);

    return *this;
  }

  mapped_type& operator[](const typename key_type::view_type key) {
    node_pointer p = find_(key);

    return p->value.second;
  }

  iterator begin() const {
    return iterator(static_cast<node_pointer>(entire_.left));
  }

  iterator end() const { return iterator(NULL); }

  void erase(const typename key_type::view_type key) {
    rbtree_node_base* node = entire_.parent;

    for (; node;) {
      const int comp =
          key.compare(static_cast<node_pointer>(node)->value.first);

      if (comp < 0)
        node = node->left;
      else if (comp > 0)
        node = node->right;
      else
        break;
    }
    erase_(node);
  }

  void erase(iterator it) { erase_(it); }

  iterator find(const typename key_type::view_type key) {
    rbtree_node_base* node = entire_.parent;

    for (; node;) {
      const int comp =
          key.compare(static_cast<node_pointer>(node)->value.first);

      if (comp < 0)
        node = node->left;
      else if (comp > 0)
        node = node->right;
      else
        break;
    }
    return iterator(static_cast<node_pointer>(node));
  }

 private:
  typedef typename key_type::value_type char_type;
  typedef rbtree_node<value_type> node_type;
  typedef typename node_type::pointer node_pointer;
  typedef typename node_type::const_pointer const_node_pointer;

  node_pointer find_(const json_internal_::view<char_type> key) {
    rbtree_node_base* node = entire_.parent;
    rbtree_node_base* parentnode = entire_.parent;
    int comp = 0;

    for (; node;) {
      comp = key.compare(static_cast<node_pointer>(node)->value.first);

      parentnode = node;
      if (comp < 0)
        node = node->left;
      else if (comp > 0)
        node = node->right;
      else
        return static_cast<node_pointer>(node);
    }

    const node_pointer nn =
        static_cast<node_pointer>(std::malloc(sizeof(node_type)));

    if (nn == NULL) throw std::bad_alloc();

    ++entire_.node_count;
    nn->left = NULL;
    nn->right = NULL;
    new (&nn->value) value_type(key, mapped_type());

    if (entire_.parent == NULL) {
      nn->parent = NULL;
      nn->colour = rb_black;

      entire_.parent = nn;
      entire_.left = nn;
      entire_.right = nn;
      return nn;
    }

    nn->parent = parentnode;
    nn->colour = rb_red;

    if (comp < 0) {
      parentnode->left = nn;
      if (entire_.left == parentnode) entire_.left = nn;
    } else {
      parentnode->right = nn;
      if (entire_.right == parentnode) entire_.right = nn;
    }

    rbtree_node_base* nn2 = nn;

    while (nn2 != entire_.parent && nn2->parent->colour == rb_red) {
      rbtree_node_base* const pp = nn2->parent->parent;

      if (nn2->parent == pp->left) {
        rbtree_node_base* const oo = pp->right;

        if (oo && oo->colour == rb_red) {
          nn2->parent->colour = rb_black;
          oo->colour = rb_black;
          pp->colour = rb_red;
          nn2 = pp;
        } else {
          if (nn2 == nn2->parent->right) {
            nn2 = nn2->parent;
            rotate_left_(nn2);
          }
          nn2->parent->colour = rb_black;
          pp->colour = rb_red;
          rotate_right_(pp);
        }
      } else {
        rbtree_node_base* const oo = pp->left;

        if (oo && oo->colour == rb_red) {
          nn2->parent->colour = rb_black;
          oo->colour = rb_black;
          pp->colour = rb_red;
          nn2 = pp;
        } else {
          if (nn2 == nn2->parent->left) {
            nn2 = nn2->parent;
            rotate_right_(nn2);
          }
          nn2->parent->colour = rb_black;
          pp->colour = rb_red;
          rotate_left_(pp);
        }
      }
    }
    entire_.parent->colour = rb_black;

    return nn;
  }

  void rotate_left_(rbtree_node_base* nc) {
    rbtree_node_base* const np = nc->right;

    nc->right = np->left;

    if (np->left != NULL) np->left->parent = nc;

    np->parent = nc->parent;

    if (nc == entire_.parent)
      entire_.parent = np;
    else if (nc == nc->parent->left)
      nc->parent->left = np;
    else
      nc->parent->right = np;

    np->left = nc;
    nc->parent = np;
  }

  void rotate_right_(rbtree_node_base* nc) {
    rbtree_node_base* const np = nc->left;

    nc->left = np->right;

    if (np->right != NULL) np->right->parent = nc;

    np->parent = nc->parent;

    if (nc == entire_.parent)
      entire_.parent = np;
    else if (nc == nc->parent->right)
      nc->parent->right = np;
    else
      nc->parent->left = np;

    np->right = nc;
    nc->parent = np;
  }

  rbtree_node_base* copy_nodes_(rbtree_node_base* pnode,
                                const rbtree_node_base* const srcnode,
                                const nemap& srctree) {
    const node_pointer nn =
        static_cast<node_pointer>(std::malloc(sizeof(node_type)));

    if (nn == NULL) throw std::bad_alloc();

    new (&nn->value)
        value_type(static_cast<const_node_pointer>(srcnode)->value);

    nn->parent = pnode;
    nn->colour = srcnode->colour;

    ++entire_.node_count;

    if (srcnode->left)
      nn->left = copy_nodes_(nn, srcnode->left, srctree);
    else
      nn->left = NULL;

    if (srcnode->right)
      nn->right = copy_nodes_(nn, srcnode->right, srctree);
    else
      nn->right = NULL;

    if (srcnode == srctree.entire_.left) entire_.left = nn;

    if (srcnode == srctree.entire_.right) entire_.right = nn;

    return nn;
  }

  void erase_all_(rbtree_node_base* const node) {
    if (node) {
      node_pointer enode = static_cast<node_pointer>(node);

      erase_all_(enode->left);
      erase_all_(enode->right);

      (&enode->value)->~value_type();
      std::free(enode);
    }
  }

  void erase_(rbtree_node_base* const enode) {
    if (enode) {
      rbtree_node_base*& root = entire_.parent;
      rbtree_node_base*& leftmost = entire_.left;
      rbtree_node_base*& rightmost = entire_.right;
      rbtree_node_base* s = enode;
      rbtree_node_base* sc = NULL;
      rbtree_node_base* scp = NULL;
      rbtree_colour s_orgclr = s->colour;

      if (enode->left == NULL)
        sc = enode->right;
      else if (enode->right == NULL)
        sc = enode->left;
      else {
        s = enode->right;

        while (s->left) s = s->left;

        s_orgclr = s->colour;
        sc = s->right;
      }

      if (s == enode) {
        scp = s->parent;

        if (sc) sc->parent = scp;

        if (enode == root)
          root = sc;
        else if (enode == enode->parent->left)
          enode->parent->left = sc;
        else
          enode->parent->right = sc;

        if (enode == leftmost) {
          if (enode->right == NULL)
            leftmost = enode->parent;
          else
            for (leftmost = sc; leftmost->left != NULL;
                 leftmost = leftmost->left);
        }

        if (enode == rightmost) {
          if (enode->left == NULL)
            rightmost = enode->parent;
          else
            for (rightmost = sc; rightmost->right != NULL;
                 rightmost = rightmost->right);
        }
      } else {
        enode->left->parent = s;
        s->left = enode->left;

        if (enode->right == s)
          scp = s;
        else {
          scp = s->parent;

          if (sc) sc->parent = scp;

          s->parent->left = sc;
          s->right = enode->right;
          enode->right->parent = s;
        }

        if (enode == root)
          root = s;
        else if (enode == enode->parent->left)
          enode->parent->left = s;
        else
          enode->parent->right = s;

        s->parent = enode->parent;
        s->colour = enode->colour;
      }

      if (s_orgclr == rb_black) {
        while (sc != root && (sc == NULL || sc->colour == rb_black)) {
          if (sc == scp->left) {
            rbtree_node_base* sib = scp->right;

            if (sib->colour == rb_red) {
              sib->colour = rb_black;
              scp->colour = rb_red;
              rotate_left_(scp);
              sib = scp->right;
            }

            if ((sib->left == NULL || sib->left->colour == rb_black) &&
                (sib->right == NULL || sib->right->colour == rb_black)) {
              sib->colour = rb_red;
              sc = scp;
              scp = scp->parent;
            } else {
              if (sib->right == NULL || sib->right->colour == rb_black) {
                sib->left->colour = rb_black;
                sib->colour = rb_red;
                rotate_right_(sib);
                sib = scp->right;
              }

              sib->colour = scp->colour;
              scp->colour = rb_black;

              if (sib->right) sib->right->colour = rb_black;

              rotate_left_(scp);
              break;
            }
          } else {
            rbtree_node_base* sib = scp->left;

            if (sib->colour == rb_red) {
              sib->colour = rb_black;
              scp->colour = rb_red;
              rotate_right_(scp);
              sib = scp->left;
            }

            if ((sib->right == NULL || sib->right->colour == rb_black) &&
                (sib->left == NULL || sib->left->colour == rb_black)) {
              sib->colour = rb_red;
              sc = scp;
              scp = scp->parent;
            } else {
              if (sib->left == NULL || sib->left->colour == rb_black) {
                sib->right->colour = rb_black;
                sib->colour = rb_red;
                rotate_left_(sib);
                sib = scp->left;
              }

              sib->colour = scp->colour;
              scp->colour = rb_black;

              if (sib->left) sib->left->colour = rb_black;

              rotate_right_(scp);
              break;
            }
          }
        }
        if (sc) sc->colour = rb_black;
      }

      node_pointer enode2 = static_cast<node_pointer>(enode);

      (&enode2->value)->~value_type();
      std::free(enode2);

      --entire_.node_count;
    }
  }

  rbtree_header entire_;
};
//  nemap

struct utf8tag {};
struct utf16tag {};

template <bool, typename T>
struct enable_if {};
template <typename T>
struct enable_if<true, T> {
  typedef T type;
};
template <typename IntType, typename RetType>
struct enable_if_integer
    : enable_if<std::numeric_limits<IntType>::is_integer, RetType> {};

template <typename charT, typename utftype>
class jsonnode {
 public:
  enum value_type {
    array,
    object,
    number,
    string,
    boolean,
    null,
    unassigned,
    fallback
  };

  typedef std::basic_string<charT>
      string_type;  //  For backwards-compatibility.
  typedef charT char_type;

  typedef nestring<charT> nstring_type;
  typedef json_internal_::view<charT> sview_type;

  struct psnum {
    double value;
    int precision;

    psnum(const double n, const int p = -6) : value(n), precision(p) {}
  };

 private:
  explicit jsonnode(const value_type v)
      : type_(v)  //  For the fallback node.
  {}

 public:
  jsonnode() : type_(unassigned) {}

  jsonnode(const jsonnode& right) : type_(right.type_) {
    if (this != &right && !is_fallback() && !right.is_fallback()) {
      if (type_ == array) {
        array_ = new array_type(*right.array_);
      } else if (type_ == object) {
        object_ = new object_type(*right.object_);
      } else if (type_ == number || type_ == string) {
        string_ = new strnum_type(*right.string_);
      } else if (type_ == boolean)
        boolvalue_ = right.boolvalue_;
    }
  }

  template <typename StringT>
  explicit jsonnode(const StringT& s) : type_(unassigned) {
    if (!parse(s)) clear();
  }

  template <typename StringT>
  jsonnode(const StringT& s, json_error& e) : type_(unassigned) {
    parse(s, e);
  }

  template <typename Iterator>
  jsonnode(Iterator begin, const Iterator end) : type_(unassigned) {
    if (!parse(begin, end)) clear();
  }

  template <typename Iterator>
  jsonnode(Iterator begin, const Iterator end, json_error& e)
      : type_(unassigned) {
    parse(begin, end, e);
  }

  ~jsonnode() {
    if (type_ < boolean) destroy_node_();
  }

  void clear() {
    if (!is_fallback()) {
      destroy_node_();
      type_ = unassigned;
    }
  }

  //  Types.

  value_type type() const { return static_cast<value_type>(type_); }

  bool is_num() const { return type_ == number; }
  bool is_str() const { return type_ == string; }
  bool is_bool() const { return type_ == boolean; }
  bool is_true() const { return type_ == boolean && boolvalue_; }
  bool is_false() const { return type_ == boolean && !boolvalue_; }
  bool is_null() const { return type_ == null; }
  bool is_array() const { return type_ == array; }
  bool is_object() const { return type_ == object; }
  bool is_unassigned() const { return type_ == unassigned; }
  bool is_fallback() const { return type_ == fallback; }
  bool is_assigned() const { return type_ < unassigned; }
  bool has_value() const { return type_ < unassigned; }

  bool exists() const { return type_ != fallback; }

  jsonnode& operator=(const jsonnode& right) {
    if (this != &right && !is_fallback() && !right.is_fallback()) {
      const int newtype = right.type_;

      if (newtype == array) {
        array_type* const newarray = new array_type(*right.array_);
        destroy_node_();
        array_ = newarray;
      } else if (newtype == object) {
        object_type* const newobject = new object_type(*right.object_);
        destroy_node_();
        object_ = newobject;
      } else if (newtype == number || newtype == string) {
        strnum_type* const newstring = new strnum_type(*right.string_);
        destroy_node_();
        string_ = newstring;
      } else if (newtype == boolean) {
        const bool bv = right.boolvalue_;
        destroy_node_();
        boolvalue_ = bv;
      }
      type_ = newtype;
    }
    return *this;
  }

  jsonnode& operator=(const double d) { return set_num(d, -6); }

  jsonnode& operator=(const psnum& n) { return set_num(n.value, n.precision); }

  template <typename IntType>
  typename enable_if_integer<IntType, jsonnode>::type& operator=(
      const IntType i) {
    return set_num(static_cast<double>(i), -6);
  }

  jsonnode& operator=(const bool b) { return set_bool(b); }

  jsonnode& operator=(const sview_type s) { return set_str(s); }

  jsonnode& operator=(const char_type* const p) { return set_str(p); }

  jsonnode& operator=(const value_type v) {
    switch (v) {
      case array:
        return empty_array();
      case object:
        return empty_object();
      case number:
        return set_num(0);
      case string:
        return set_str(emptystr_, emptystr_);
      case boolean:
        return set_bool(false);
        //		case null, unassigned, fallback.
      default:;
    }
    return set_null();
  }

  template <typename StringT>
  bool parse(const StringT& s) {
    json_error e;
    return parse(s, e);
  }

  template <typename StringT>
  bool parse(const StringT& s, json_error& e) {
    if (!is_fallback()) {
      return parse(s.data(), s.data() + s.size(), e);
    }
    return e.clear_();
  }

  template <typename charT2>
  bool parse(const charT2* const p) {
    json_error e;
    return parse(p, e);
  }

  template <typename charT2>
  bool parse(const charT2* const p, json_error& e) {
    if (!is_fallback()) {
      const charT2* const end = p + std::char_traits<charT2>::length(p);
      return parse(p, end, e);
    }
    return e.clear_();
  }

  template <typename Iterator>
  bool parse(Iterator begin, const Iterator end) {
    json_error e;
    return parse(begin, end, e);
  }

  template <typename Iterator>
  bool parse(Iterator begin, const Iterator end, json_error& e) {
    if (is_fallback()) return e.clear_();

    const Iterator orgbegin = begin;

    destroy_node_();
    type_ = unassigned;
    begin = skip_spaces_(begin, end);

    e.code_ = parse_value_(begin, end);
    e.pos_ = e.code_ ? std::distance(orgbegin, begin) : 0;

    return e.code_ == 0;
  }

  std::basic_string<char_type> to_string() const {
    std::basic_string<char_type> out;
    value_to_string_(out);
    return out;
  }
  std::basic_string<char_type> stringify() const {
    std::basic_string<char_type> out;
    value_to_string_(out);
    return out;
  }

  template <typename StringT>
  StringT to_string() const {
    StringT out;
    value_to_string_(out);
    return out;
  }
  template <typename StringT>
  StringT stringify() const {
    StringT out;
    value_to_string_(out);
    return out;
  }

  template <typename StringT>
  void to_string(StringT& out) const {
    out.clear();
    value_to_string_(out);
  }
  template <typename StringT>
  void stringify(StringT& out) const {
    out.clear();
    value_to_string_(out);
  }

  //  For number.

  jsonnode& set_num(const double d, const int precision = -6) {
    if (is_fallback()) return fallback_node_;

    if (type_ != number) {
      destroy_node_();
      type_ = number;
      string_ = new strnum_type();
    }

    string_->num = d;
    string_->str.assign(ctl::to_string<nstring_type>(d, 'f', precision));
    return *this;
  }

  //  For string.

  jsonnode& set_str(const sview_type s) {
    return set_str(s.data(), s.data() + s.size());
  }

  template <typename Iterator>
  jsonnode& set_str(Iterator begin, const Iterator end) {
    if (is_fallback()) return fallback_node_;

    if (type_ != string) {
      destroy_node_();
      type_ = string;
      string_ = new strnum_type();
    }
    string_->str.assign(begin, end);

    return *this;
  }

  //  For array.

  bool exists(const std::size_t no) const {
    return !is_fallback() && n(no) != &fallback_node_;
  }

  const jsonnode& operator[](const std::size_t no) const {
    const jsonnode* const node = n(no);
    return *node;
  }
  jsonnode& operator[](const std::size_t no) {
    jsonnode* const node = n(no);
    return *node;
  }

  const jsonnode* n(const std::size_t no) const {
    if (type_ == array) {
      if (no < array_->size()) return &(*array_)[no];
    }
    return &fallback_node_;
  }
  jsonnode* n(const std::size_t no) {
    if (type_ == array) {
      if (no < array_->size()) return &(*array_)[no];
    }
    return &fallback_node_;
  }

  jsonnode& operator()(const std::size_t no) {
    if (is_fallback()) return fallback_node_;

    if (type_ == array) {
      if (no >= array_->size()) array_->resize(no + 1);
    } else {
      destroy_node_();
      type_ = array;
      array_ = new array_type();
      array_->resize(no + 1);
    }
    return (*array_)[no];
  }

  bool erase(const std::size_t no) {
    if (type_ == array && no < array_->size()) {
      array_->erase(no);
      return true;
    }
    return false;
  }

  template <typename ValueType>
  bool insert(const std::size_t no, const ValueType& right) {
    if (type_ == array && no <= array_->size()) {
      jsonnode backup;
      backup = right;
      array_->insert(no).move_(backup);
      return true;
    }
    return false;
  }

  template <typename ValueType>
  void push_back(const ValueType& right) {
    if (type_ == array) {
      jsonnode backup;  //  Consideration for right == *this.
      backup = right;
      array_->push_back().move_(backup);
    }
  }

  jsonnode& empty_array() {
    if (is_fallback()) return fallback_node_;

    if (type_ == array) {
      array_->clear();
    } else {
      destroy_node_();
      type_ = array;
      array_ = new array_type();
    }
    return *this;
  }

  //  For array and object.

  std::size_t size() const {
    if (type_ == array)
      return array_->size();
    else if (type_ == object)
      return object_->objmap.size();

    return 0;
  }

  //  For object.

  bool exists(const sview_type key) const {
    return !is_fallback() && n(key) != &fallback_node_;
  }

  const jsonnode& operator[](const sview_type key) const {
    const jsonnode* const node = n(key);
    return *node;
  }
  jsonnode& operator[](const sview_type key) {
    jsonnode* const node = n(key);
    return *node;
  }

  const jsonnode* n(const sview_type key) const {
    if (type_ == object) {
      const typename object_core::const_iterator it = object_->objmap.find(key);

      if (it != object_->objmap.end()) return &it->second;
    }
    return &fallback_node_;
  }
  jsonnode* n(const sview_type key) {
    if (type_ == object) {
      const typename object_core::iterator it = object_->objmap.find(key);

      if (it != object_->objmap.end()) return &it->second;
    }
    return &fallback_node_;
  }

  jsonnode& operator()(const sview_type key) {
    if (is_fallback()) return fallback_node_;

    if (type_ == object) {
      const typename object_core::iterator it = object_->objmap.find(key);

      if (it != object_->objmap.end()) return it->second;

      append_keyinfo_(object_->order, key, utftype());
      return object_->objmap[key];
    }

    destroy_node_();
    type_ = object;
    object_ = new object_type();
    append_keyinfo_(object_->order, key, utftype());
    return object_->objmap[key];
  }

  bool erase(const sview_type key) {
    if (type_ == object) {
      const typename object_core::iterator it = object_->objmap.find(key);

      if (it != object_->objmap.end()) {
        object_->objmap.erase(it);
        remove_orderkey_(key);
        return true;
      }
    }
    return false;
  }

  template <typename ValueType>
  bool insert(const sview_type inspos, const sview_type newkey,
              const ValueType& newelem) {
    if (type_ == object && newkey != inspos) {
      strsize_type newkeyindex = object_->order.size();
      strsize_type insposindex = newkeyindex;

      for (strsize_type i = 0; (i < object_->order.size()) &&
                               (newkeyindex == object_->order.size() ||
                                insposindex == object_->order.size());) {
        const strsize_type keylen = get_keylen_(&object_->order[i], utftype());
        strsize_type base = i + keylen_size;

        if (keylen == newkey.size() &&
            object_->order.compare(base, keylen, newkey, 0, keylen) == 0)
          newkeyindex = i;
        else if (keylen == inspos.size() &&
                 object_->order.compare(base, keylen, inspos, 0, keylen) == 0)
          insposindex = i;

        i = base + keylen;
      }

      if (insposindex < object_->order.size()) {
        const strsize_type newkeylen = newkey.size();
        jsonnode backup;

        backup = newelem;

        if (newkeyindex < object_->order.size()) {
          const strsize_type remlen = keylen_size + newkeylen;

          if ((newkeyindex + remlen) == insposindex)
            goto UPDATE;
          else {
            object_->order.erase(newkeyindex, remlen);
            if (newkeyindex < insposindex) insposindex -= remlen;
          }
        }
        {
          nstring_type len_newkey;

          append_keyinfo_(len_newkey, newkey, utftype());
          object_->order.insert(insposindex, len_newkey);
        }

      UPDATE:
        object_->objmap[newkey].move_(backup);
        return true;
      }
    }
    return false;
  }

  template <typename ValueType>
  bool push_back(const sview_type key, const ValueType& right) {
    if (type_ == object) {
      jsonnode backup;  //  Consideration for right == *this.

      backup = right;

      for (strsize_type pos = 0;
           (pos + sizeof(keylen_type)) < object_->order.size();) {
        const strsize_type keylen =
            get_keylen_(&object_->order[pos], utftype());
        const strsize_type keypos = pos + keylen_size;

        if (keylen == key.size()) {
          for (strsize_type i = 0;; ++i) {
            if (i == keylen) {
              if (keypos + keylen == object_->order.size()) goto PUSH_VALUE;

              object_->order.erase(keypos - keylen_size, keylen_size + keylen);
              goto PUSH_KEY;
            }
            if (object_->order[keypos + i] != key[i]) break;
          }
        }
        pos = keypos + keylen;
      }

    PUSH_KEY:
      append_keyinfo_(object_->order, key, utftype());

    PUSH_VALUE:
      object_->objmap[key].move_(backup);

      return true;
    }
    return false;
  }

  jsonnode& empty_object() {
    if (is_fallback()) return fallback_node_;

    if (type_ == object) {
      object_->objmap.clear();
      object_->order.clear();
    } else {
      destroy_node_();
      type_ = object;
      object_ = new object_type();
    }
    return *this;
  }

  double num() const {
    if (type_ == number) return string_->num;
    if (type_ == boolean) return static_cast<double>(boolvalue_);
    return 0.0;
  }

  template <typename NumType>
  NumType num() const {
    return static_cast<NumType>(
        type_ == number ? string_->num : (type_ == boolean ? boolvalue_ : 0));
  }

  sview_type numstr_view() const {
    static const char_type ltrue[] = {0x74, 0x72, 0x75, 0x65, 0};  //  "true"
    static const char_type lfalse[] = {0x66, 0x61, 0x6C,
                                       0x73, 0x65, 0};             //  "false"
    static const char_type lnull[] = {0x6E, 0x75, 0x6C, 0x6C, 0};  //  "null"

    if (type_ == number) return string_->str;
    if (type_ == boolean) return boolvalue_ ? ltrue : lfalse;
    if (type_ == null) return lnull;
    return sview_type(lnull, 0);
  }

  std::basic_string<char_type> numstr() const {
    const sview_type sv = numstr_view();
    return std::basic_string<char_type>(sv.data(), sv.size());
  }

  template <typename StringT>
  void numstr(StringT& out) const {
    typedef typename StringT::value_type dchar_type;
    static const dchar_type ltrue[] = {0x74, 0x72, 0x75, 0x65, 0};  //  "true"
    static const dchar_type lfalse[] = {0x66, 0x61, 0x6C,
                                        0x73, 0x65, 0};             //  "false"
    static const dchar_type lnull[] = {0x6E, 0x75, 0x6C, 0x6C, 0};  //  "null"

    out.clear();
    if (type_ == number) {
      out.append(string_->str.begin(), string_->str.end());
    } else if (type_ == boolean)
      out.append(boolvalue_ ? ltrue : lfalse);
    else if (type_ == null)
      out.append(lnull);
  }

  sview_type str_view() const {
    if (type_ == string) return sview_type(string_->str);

    return sview_type(emptystr_, 0);
  }

  std::basic_string<char_type> str() const {
    if (type_ == string) return string_->str;

    return std::basic_string<char_type>();
  }

  template <typename StringT>
  void str(StringT& out) const {
    out.clear();
    if (type_ == string) {
      out.append(string_->str.begin(), string_->str.end());
    }
  }

  //  For boolean, null.

  jsonnode& set_bool(const bool b) {
    if (is_fallback()) return fallback_node_;

    if (type_ != boolean) {
      destroy_node_();
      type_ = boolean;
    }
    boolvalue_ = b;
    return *this;
  }

  jsonnode& set_null() {
    if (is_fallback()) return fallback_node_;

    if (type_ != null) {
      destroy_node_();
      type_ = null;
    }
    return *this;
  }

  jsonnode allkeys() const {
    jsonnode out;
    allkeys(out);
    return out;
  }

  void allkeys(jsonnode& out) const {
    if (is_fallback()) {
    } else if (type_ == object) {
      const std::size_t objsize = object_->objmap.size();

      if (out.type_ != array) {
        out.destroy_node_();
        out.type_ = array;
        out.array_ = new array_type();
      }
      out.array_->resize(objsize);

      strsize_type pos = 0;

      for (std::size_t i = 0;
           i < objsize && (pos + keylen_size) < object_->order.size(); ++i) {
        jsonnode& elem = (*out.array_)[i];
        const strsize_type keylen =
            get_keylen_(&object_->order[pos], utftype());

        if (elem.type_ != string) {
          elem.destroy_node_();
          elem.type_ = string;
          elem.string_ = new strnum_type();
        }
        pos += keylen_size;
        elem.string_->str.assign(object_->order.view(pos, keylen));
        pos += keylen;
      }
      return;
    }
    out.clear();
  }

 private:
  typedef typename nstring_type::size_type strsize_type;
  typedef unsigned int keylen_type;

  typedef nevector<jsonnode> array_type;
  typedef nemap<nstring_type, jsonnode> object_core;

  struct object_type {
    object_core objmap;
    nstring_type order;
  };

  struct strnum_type {
    double num;
    nstring_type str;
  };

  static const std::size_t keylen_size_tmp =
      sizeof(keylen_type) / sizeof(char_type) +
      ((sizeof(keylen_type) % sizeof(char_type)) ? 1 : 0);
  static const std::size_t keylen_size =
      keylen_size_tmp >= 2 ? keylen_size_tmp : 2;

  template <typename ForwardIterator>
  inline ForwardIterator skip_spaces_(ForwardIterator begin,
                                      const ForwardIterator end) {
    for (; begin != end && (*begin == 0x20 || *begin == 0x09 ||
                            *begin == 0x0a || *begin == 0x0d);
         ++begin);

    return begin;
  }

  template <typename utftag>
  void append_keyinfo_(nstring_type& order, const sview_type add,
                       const utftag) const {
    const keylen_type size = static_cast<keylen_type>(add.size());
    const strsize_type base = order.size();

    order.resize(base + keylen_size);
    std::memcpy(&order[base], &size, sizeof(keylen_type));
    order.append(add);
  }
  void append_keyinfo_(nstring_type& order, const sview_type add,
                       const utf16tag) const {
    const keylen_type size = static_cast<keylen_type>(add.size());
    strsize_type i = order.size();

    order.resize(i + keylen_size);
    order[i++] = static_cast<charT>(size & 0xffff);
    order[i] = static_cast<charT>(((size >> 8) >> 8) & 0xffff);
    order.append(add);
  }

  template <typename utftag>
  strsize_type get_keylen_(const charT* const p, const utftag) const {
    keylen_type size;
    std::memcpy(&size, p, sizeof(keylen_type));
    return size;
  }
  strsize_type get_keylen_(const charT* const p, const utf16tag) const {
    return static_cast<strsize_type>(((p[0] & 0xffff)) |
                                     (((p[1] & 0xffff) << 8) << 8));
  }

  //  UTF-8 -> UTF-8.

  template <typename ForwardIterator, typename UTFtag>
  bool parse_string_(nstring_type* const str, ForwardIterator& begin,
                     const ForwardIterator end, const UTFtag) {
    ForwardIterator prevbegin = begin;
    unsigned long prev = 0ul;

    for (; begin != end;) {
      if (*begin == 0x22)  //  '"'
      {
        if (prevbegin != begin) str->append(prevbegin, begin);

        begin = skip_spaces_(++begin, end);
        return true;
      }

      if (*begin == 0x5C)  //  '\\'
      {
        if (++begin == end) break;

        char_type nch = *begin;

        switch (nch) {
          case 0x22:  //  "    quotation mark  U+0022
          case 0x5c:  //  \    reverse solidus U+005C
          case 0x2f:  //  /    solidus         U+002F
            break;

          case 0x62:  //  b    backspace       U+0008
            nch = 0x08;
            break;

          case 0x66:  //  f    form feed       U+000C
            nch = 0x0c;
            break;

          case 0x6e:  //  n    line feed       U+000A
            nch = 0x0a;
            break;

          case 0x72:  //  r    carriage return U+000D
            nch = 0x0d;
            break;

          case 0x74:  //  t    tab             U+0009
            nch = 0x09;
            break;

          case 0x75:  //  uXXXX                U+XXXX
          {
            ForwardIterator upos = begin;
            unsigned long ucp = parse_4hexdigits_(begin, end);

            if (ucp < 0x10000ul) {
              ++begin;

              if (prev && ucp >= 0xDC00ul && ucp <= 0xDFFFul) {
                ucp = ((ucp & 0x3FFul) | ((prev & 0x3FFul) << 10)) + 0x10000;

                (*str)[str->size() - 3] =
                    static_cast<char_type>(((ucp >> 18) & 7) | 0xf0);
                (*str)[str->size() - 2] =
                    static_cast<char_type>(((ucp >> 12) & 0x3f) | 0x80);
                (*str)[str->size() - 1] =
                    static_cast<char_type>(((ucp >> 6) & 0x3f) | 0x80);
                str->push_back(static_cast<char_type>((ucp & 0x3f) | 0x80));
                prev = 0ul;
              } else {
                prev = 0ul;

                if (ucp < 0x80ul) {
                  str->append(prevbegin, upos);
                  (*str)[str->size() - 1] = static_cast<char_type>(ucp);
                } else if (ucp < 0x800ul) {
                  ++upos;
                  str->append(prevbegin, upos);
                  (*str)[str->size() - 2] =
                      static_cast<char_type>(((ucp >> 6) & 0x1f) | 0xc0);
                  (*str)[str->size() - 1] =
                      static_cast<char_type>((ucp & 0x3f) | 0x80);
                } else {
                  ++upos;
                  ++upos;
                  str->append(prevbegin, upos);
                  (*str)[str->size() - 3] =
                      static_cast<char_type>(((ucp >> 12) & 0x0f) | 0xe0);
                  (*str)[str->size() - 2] =
                      static_cast<char_type>(((ucp >> 6) & 0x3f) | 0x80);
                  (*str)[str->size() - 1] =
                      static_cast<char_type>((ucp & 0x3f) | 0x80);
                  if ((ucp & 0xFC00ul) == 0xD800ul) prev = ucp;
                }
              }
              prevbegin = begin;
              continue;
            }
          }
            //  "\u" not followed by four hexdigits.
            prev = 0ul;
            continue;

          default:
            prev = 0ul;
            ++begin;
            continue;
        }

        str->append(prevbegin, begin);
        (*str)[str->size() - 1] = nch;

        prev = 0ul;
        prevbegin = ++begin;
        continue;
      }
      ++begin;
    }
    str->clear();
    return false;
  }

  //  UTF-8 -> UTF-16.

  template <typename ForwardIterator>
  bool parse_string_(nstring_type* const str, ForwardIterator& begin,
                     const ForwardIterator end, const utf16tag) {
    ForwardIterator prevbegin = begin;

    for (; begin != end;) {
      if (*begin == 0x22)  //  '"'
      {
        if (prevbegin != begin) str->append(prevbegin, begin);

        begin = skip_spaces_(++begin, end);
        return true;
      }

      if (*begin == 0x5C)  //  '\\'
      {
        if (++begin == end) break;

        char_type nch = *begin;

        switch (nch) {
          case 0x22:  //  "    quotation mark  U+0022
          case 0x5c:  //  \    reverse solidus U+005C
          case 0x2f:  //  /    solidus         U+002F
            break;

          case 0x62:  //  b    backspace       U+0008
            nch = 0x08;
            break;

          case 0x66:  //  f    form feed       U+000C
            nch = 0x0c;
            break;

          case 0x6e:  //  n    line feed       U+000A
            nch = 0x0a;
            break;

          case 0x72:  //  r    carriage return U+000D
            nch = 0x0d;
            break;

          case 0x74:  //  t    tab             U+0009
            nch = 0x09;
            break;

          case 0x75:  //  uXXXX                U+XXXX
          {
            ForwardIterator upos = begin;
            unsigned long ucp = parse_4hexdigits_(begin, end);

            if (ucp < 0x10000ul) {
              ++begin;

              str->append(prevbegin, upos);
              (*str)[str->size() - 1] = static_cast<char_type>(ucp);
              prevbegin = begin;
            }
          }
            //  "\u" not followed by four hexdigits.
            continue;

          default:
            ++begin;
            continue;
        }

        str->append(prevbegin, begin);
        (*str)[str->size() - 1] = nch;

        prevbegin = ++begin;
        continue;
      } else if (*begin & 0x80) {
        unsigned long ch = *begin & 0xff;

        if (ch >= 0xc2) {
          if ((ch & 0x20) == 0)  //  2 octets?
          {
            if (++begin != end && (*begin & 0xc0) == 0x80)  //  2nd octet.
            {
              ch = ((ch << 6) & 0x7c0) | (*begin & 0x3f);

              str->append(prevbegin, begin);
              (*str)[str->size() - 1] = static_cast<char_type>(ch);
              prevbegin = ++begin;
              continue;
            }
          } else if ((ch & 0x10) == 0)  //  3 octets?
          {
            if (++begin != end && (*begin & 0xc0) == 0x80)  //  2nd octet.
            {
              const ForwardIterator tmpend = begin;

              ch = (ch << 8) | (*begin & 0xff);

              if (ch >= 0xe0a0)  //  E0A080..
              {
                if (++begin != end && (*begin & 0xc0) == 0x80)  //  3rd octet.
                {
                  ch = ((ch << 4) & 0xf000) | ((ch << 6) & 0xfc0) |
                       (*begin & 0x3f);

                  str->append(prevbegin, tmpend);
                  (*str)[str->size() - 1] = static_cast<char_type>(ch);
                  prevbegin = ++begin;
                  continue;
                }
              }
            }
          } else if (ch >= 0xf0 && ch <= 0xf4)  //  4 octets?
          {
            if (++begin != end && (*begin & 0xc0) == 0x80)  //  2nd octet.
            {
              ch = (ch << 8) | (*begin & 0xff);

              if (ch >= 0xf090 && ch <= 0xf48f)  //  F0908080..F48FBFBF.
              {
                if (++begin != end && (*begin & 0xc0) == 0x80)  //  3rd octet.
                {
                  const ForwardIterator tmpend = begin;

                  ch = ((ch << 4) & 0x7000) | ((ch << 6) & 0xfc0) |
                       (*begin & 0x3f);

                  if (++begin != end && (*begin & 0xc0) == 0x80)  //  4th octet.
                  {
                    ch = ((ch << 6) | (*begin & 0x3f)) - 0x10000;

                    str->append(prevbegin, tmpend);
                    (*str)[str->size() - 2] =
                        static_cast<char_type>(0xd800 | (ch >> 10));
                    (*str)[str->size() - 1] =
                        static_cast<char_type>(0xdc00 | (ch & 0x3ff));
                    prevbegin = ++begin;
                    continue;
                  }
                }
              }
            }
          }
          //  F5..FF
        }
        //  80..C1
        break;
      }
      ++begin;
    }
    str->clear();
    return false;
  }

  template <typename ForwardIterator>
  int parse_value_(ForwardIterator& begin, const ForwardIterator end) {
    int ecode = json_error::ec_no_error;

    if (begin != end) {
      if (*begin == 0x5B)  //  '['
      {
        array_ = new array_type();
        type_ = array;
        begin = skip_spaces_(++begin, end);

        for (;;) {
          if (begin == end) {
            ecode = json_error::ec_array;
            break;
          }

          if (*begin == 0x5D)  //  ']'
          {
            begin = skip_spaces_(++begin, end);
            return 0;
          }

          array_->push_back(jsonnode());
          ecode = array_->back().parse_value_(begin, end);
          if (ecode) break;

          if (begin != end && *begin == 0x2C)  //  ','
            begin = skip_spaces_(++begin, end);
        }
        return ecode;
      } else if (*begin == 0x7B)  //  '{'
      {
        nstring_type key;

        object_ = new object_type();
        type_ = object;
        begin = skip_spaces_(++begin, end);

        for (;;) {
          if (begin == end) {
            ecode = json_error::ec_object;
            break;
          }

          if (*begin == 0x7D)  //  '}'
          {
            begin = skip_spaces_(++begin, end);
            return 0;
          }

          if (*begin == 0x22)  //  '"', key.
          {
            key.clear();
            parse_string_(&key, ++begin, end, utftype());

            if (key.size() == 0) {
              ecode = json_error::ec_object_name;
              break;
            }

            const typename object_core::iterator it = object_->objmap.find(key);
            jsonnode* newnode;

            if (it == object_->objmap.end()) {
              append_keyinfo_(object_->order, key, utftype());
              newnode = &object_->objmap[key];
            } else {
              it->second.destroy_node_();
              it->second.type_ = unassigned;
              newnode = &it->second;
            }

            if (begin == end || *begin != 0x3A)  //  ':'
            {
              ecode = json_error::ec_no_colon;
              break;
            }
            begin = skip_spaces_(++begin, end);

            ecode = newnode->parse_value_(begin, end);
            if (ecode) break;

            if (begin != end && *begin == 0x2C)  //  ','
              begin = skip_spaces_(++begin, end);
          }
          //  else if (*begin == ',')
          else {
            ecode = json_error::ec_object;
            break;
          }
        }
        return ecode;
      } else if (*begin == 0x22)  //  '"'
      {
        string_ = new strnum_type();

        if (parse_string_(&string_->str, ++begin, end, utftype())) {
          type_ = string;
          return 0;
        }
        ecode = json_error::ec_string;
        delete string_;
      } else if (*begin == 0x74)  //  't'
      {
        if (++begin != end && *begin == 0x72)      //  'r'
          if (++begin != end && *begin == 0x75)    //  'u'
            if (++begin != end && *begin == 0x65)  //  'e'
            {
              ++begin;
              type_ = boolean;
              boolvalue_ = true;
              return 0;
            }
        ecode = json_error::ec_literal;
      } else if (*begin == 0x66)  //  'f'
      {
        if (++begin != end && *begin == 0x61)        //  'a'
          if (++begin != end && *begin == 0x6C)      //  'l'
            if (++begin != end && *begin == 0x73)    //  's'
              if (++begin != end && *begin == 0x65)  //  'e'
              {
                ++begin;
                type_ = boolean;
                boolvalue_ = false;
                return 0;
              }
        ecode = json_error::ec_literal;
      } else if (*begin == 0x6E)  //  'n'
      {
        if (++begin != end && *begin == 0x75)      //  'u'
          if (++begin != end && *begin == 0x6C)    //  'l'
            if (++begin != end && *begin == 0x6C)  //  'l'
            {
              ++begin;
              type_ = null;
              return 0;
            }
        ecode = json_error::ec_literal;
      } else {
        ForwardIterator orgbegin = begin;
        nestring<char> cdigits;

        if (*begin == 0x2D)  //  '-'
        {
          cdigits.push_back('-');
          ++begin;
        }

        if (begin != end) {
          if (*begin == 0x30)  //  '0'
          {
            cdigits.push_back('0');
            ++begin;
          } else if (*begin >= 0x31 && *begin <= 0x39)  //  '1', '9'.
          {
            do {
              cdigits.push_back(static_cast<char>(*begin - 0x30 + '0'));
            } while (++begin != end && *begin >= 0x30 &&
                     *begin <= 0x39);  //  '0', '9'.
          } else {
            ecode =
                cdigits.size() ? json_error::ec_number : json_error::ec_literal;
            goto FAILED;
          }

          ecode = json_error::ec_number;

          if (begin != end && *begin == 0x2E)  //  '.'
          {
            cdigits.push_back('.');
            if (++begin == end || *begin < 0x30 || *begin > 0x39) goto FAILED;

            do {
              cdigits.push_back(static_cast<char>(*begin - 0x30 + '0'));
            } while (++begin != end && *begin >= 0x30 &&
                     *begin <= 0x39);  //  '0', '9'.
          }

          if (begin != end && (*begin | 0x20) == 0x65)  //  'E' or 'e'.
          {
            cdigits.push_back('E');
            if (++begin == end) goto FAILED;

            if (*begin == 0x2B || *begin == 0x2D)  //  '+', '-'.
            {
              cdigits.push_back(*begin == 0x2B ? '+' : '-');
              ++begin;
            }

            if (begin == end || *begin < 0x30 || *begin > 0x39) goto FAILED;

            do {
              cdigits.push_back(static_cast<char>(*begin - 0x30 + '0'));
            } while (++begin != end && *begin >= 0x30 &&
                     *begin <= 0x39);  //  '0', '9'.
          }

          cdigits.push_back(0);
          type_ = number;
          string_ = new strnum_type();
          string_->num = std::strtod(cdigits.data(), NULL);
          string_->str.assign(orgbegin, begin);
          begin = skip_spaces_(begin, end);
          return 0;
        }
      }
    }
  FAILED:
    return ecode;
  }

  void destroy_node_() {
    if (type_ == array) {
      //			for (typename array_type::size_type i = 0; i <
      //array_->size(); ++i)
      //				(*array_)[i].destroy_node_();
      if (array_) delete array_;
    } else if (type_ == object) {
      //			for (typename object_type::iterator it =
      //object_->begin(); it != object_->end(); ++it)
      //				it->second.destroy_node_();
      if (object_) delete object_;
    } else if (type_ == string || type_ == number) {
      if (string_) delete string_;
    }
    //		else	//  null, boolean.

    //		value_ = NULL;
    //		type_ = unassigned;
  }

  jsonnode& move_(jsonnode& right) {
    if (this != &right && !is_fallback() && !right.is_fallback()) {
      if (type_ < boolean) destroy_node_();

      type_ = right.type_;

      if (type_ == array) {
        array_ = right.array_;
      } else if (type_ == object) {
        object_ = right.object_;
      } else if (type_ == number || type_ == string) {
        string_ = right.string_;
      } else if (type_ == boolean) {
        boolvalue_ = right.boolvalue_;
      }
      right.type_ = unassigned;
    }
    return *this;
  }

  template <typename StringT>
  void value_to_string_(StringT& out) const {
    typedef typename StringT::value_type dchar_type;
    static const dchar_type quot = 0x22;                            //  '"'
    static const dchar_type quotcolon[] = {0x22, 0x3A, 0};          //  "\":"
    static const dchar_type ltrue[] = {0x74, 0x72, 0x75, 0x65, 0};  //  "true"
    static const dchar_type lfalse[] = {0x66, 0x61, 0x6C,
                                        0x73, 0x65, 0};             //  "false"
    static const dchar_type lnull[] = {0x6E, 0x75, 0x6C, 0x6C, 0};  //  "null"

    if (type_ == number) {
      const strsize_type base = out.size();

      out.resize(out.size() + string_->str.size());

      for (strsize_type i = 0; i < string_->str.size(); ++i)
        out[base + i] = string_->str[i];  //  ASCII only.
    } else if (type_ == string) {
      out.push_back(quot);
      escape_(out, string_->str, utftype());
      out.push_back(quot);
    } else if (type_ == boolean)
      out.append(boolvalue_ ? ltrue : lfalse);
    else if (type_ == array) {
      out.push_back(0x5B);  //  '['

      const strsize_type base = out.size();

      if (array_) {
        for (typename array_type::size_type i = 0; i < array_->size(); ++i) {
          (*array_)[i].value_to_string_(out);
          out.push_back(0x2C);  //  ','
        }
      }

      if (out.size() != base)
        out[out.size() - 1] = 0x5D;  //  ']'
      else
        out.push_back(0x5D);  //  ']'
    } else if (type_ == object) {
      out.push_back(0x7B);  //  '{'

      const strsize_type base = out.size();

      if (object_) {
#if 0
				for (typename object_core::const_iterator it = object_->objmap.begin(); it != object_->objmap.end(); ++it)
				{
					out.push_back(quot);
					escape_(out, it->first, utftype());
					out.append(quotcolon);
					it->second.value_to_string_(out);
					out.push_back(0x2C);	//  ','
				}
#else
        for (strsize_type pos = 0;
             (pos + keylen_size) < object_->order.size();) {
          const strsize_type keylen =
              get_keylen_(&object_->order[pos], utftype());
          pos += keylen_size;
          const typename object_core::const_iterator it =
              object_->objmap.find(object_->order.view(pos, keylen));
          pos += keylen;

          if (it != object_->objmap.end()) {
            out.push_back(quot);
            escape_(out, it->first, utftype());
            out.append(quotcolon);
            it->second.value_to_string_(out);
            out.push_back(0x2C);  //  ','
          }
        }
#endif
      }

      if (out.size() != base)
        out[out.size() - 1] = 0x7D;  //  '}'
      else
        out.push_back(0x7D);  //  '}'
    } else
      out.append(lnull);
  }

  void remove_orderkey_(const sview_type key) {
    for (strsize_type pos = 0; (pos + keylen_size) < object_->order.size();) {
      const strsize_type keylen = get_keylen_(&object_->order[pos], utftype());
      const strsize_type keypos = pos + keylen_size;

      if (keylen == key.size()) {
        for (strsize_type i = 0;; ++i) {
          if (i == keylen) {
            object_->order.erase(pos, keylen_size + keylen);
            return;
          }
          if (object_->order[keypos + i] != key[i]) break;
        }
      }
      pos = keypos + keylen;
    }
  }

  template <typename ForwardIterator>
  unsigned long parse_4hexdigits_(ForwardIterator& begin,
                                  const ForwardIterator end) const {
    unsigned long ucp = 0ul;

    for (strsize_type i = 0; i < 4; ++i) {
      if (++begin != end) {
        int b = *begin;

        if (b >= 0x30 && b <= 0x39)
          ucp = (ucp << 4) | (b - 0x30);
        else {
          b |= 0x20;
          if (b >= 0x61 && b <= 0x66)
            ucp = (ucp << 4) | (b - 0x61 + 10);
          else
            return 0x10000ul;
        }
      } else
        break;
    }
    return ucp;
  }

  //  UTF-8 -> UTF-8.

  template <typename StringT, typename UTFtype>
  void escape_(StringT& d, const sview_type s, const UTFtype) const {
    typedef typename StringT::value_type dchar_type;
    typedef typename StringT::size_type dsize_type;

    const dsize_type base = d.size();

    d.append(s.begin(), s.end());

    for (dsize_type i = base; i < d.size(); ++i) {
      unsigned long ch = d[i];

      //  unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
      if (ch < 0x20) {
        d.insert(i, 5, 0);
        d[i++] = static_cast<dchar_type>(0x5c);
        d[i++] = static_cast<dchar_type>(0x75);
        d[i++] = static_cast<dchar_type>(0x30);
        d[i++] = static_cast<dchar_type>(0x30);
        d[i++] = static_cast<dchar_type>((ch >> 4) + 0x30);
        ch &= 0xf;
        d[i] = static_cast<dchar_type>(ch + (ch < 10 ? 0x30 : 0x41 - 10));
      } else if (ch == 0x22 || ch == 0x5c) {
        d.insert(i++, 1, 0x5c);
      }
    }
  }

  //  UTF-16 -> UTF-8.

  template <typename StringT>
  void escape_(StringT& d, const sview_type s, const utf16tag) const {
    typedef typename StringT::value_type dchar_type;
    typedef typename StringT::size_type dsize_type;
    unsigned long prev = 0ul;

    d.reserve(d.size() + s.size());

    for (strsize_type i = 0; i < s.size(); ++i) {
      unsigned long ch = s[i] & 0xffff;

      //  unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
      if (ch < 0x20) {
        dsize_type j = d.size();

        d.resize(j + 6);
        d[j++] = static_cast<dchar_type>(0x5c);
        d[j++] = static_cast<dchar_type>(0x75);
        d[j++] = static_cast<dchar_type>(0x30);
        d[j++] = static_cast<dchar_type>(0x30);
        d[j++] = static_cast<dchar_type>((ch >> 4) + 0x30);
        ch &= 0xf;
        d[j] = static_cast<dchar_type>(ch + (ch < 10 ? 0x30 : 0x41 - 10));
      } else if (ch == 0x22 || ch == 0x5c) {
        d.resize(d.size() + 2, 0x5c);
        d[d.size() - 1] = static_cast<dchar_type>(ch);
      } else if (ch >= 0x80) {
        if (ch >= 0x800) {
          if (prev && ((ch - 0xdc00) < 0x400))  //  DC00..DFFF.
          {
            dsize_type j = d.size();

            ch = (((prev & 0x3ff) << 10) | (ch & 0x3ff)) + 0x10000;

            d.resize(j + 1, static_cast<dchar_type>(0x80 | (ch & 0x3f)));
            d[j - 3] = static_cast<dchar_type>(0xf0 | (ch >> 18));
            d[j - 2] = static_cast<dchar_type>(0x80 | ((ch >> 12) & 0x3f));
            d[j - 1] = static_cast<dchar_type>(0x80 | ((ch >> 6) & 0x3f));
          } else {
            dsize_type j = d.size();

            d.resize(j + 3, static_cast<dchar_type>(0x80 | (ch & 0x3f)));
            d[j++] = static_cast<dchar_type>(0xe0 | (ch >> 12));
            d[j++] = static_cast<dchar_type>(0x80 | ((ch >> 6) & 0x3f));

            prev = ((ch - 0xd800) < 0x400) ? ch : 0ul;  //  D800..DBFF.
            continue;
          }
        } else  //  80..7FF.
        {
          dsize_type j = d.size();

          d.resize(j + 2, static_cast<dchar_type>(0x80 | (ch & 0x3f)));
          d[j] = static_cast<dchar_type>(0xc0 | (ch >> 6));
        }
      } else
        d.push_back(static_cast<dchar_type>(ch));

      prev = 0ul;
    }
  }

 private:
  union {
    void* value_;
    strnum_type* string_;
    array_type* array_;
    object_type* object_;
    bool boolvalue_;
  };
  union {
    //		value_type type_;
    int type_;
    void* padding_;
  };

  static jsonnode fallback_node_;
  static const char_type emptystr_[];
};
//  Thanks to ODR, only one static object exists per type of charT,
//  as fallback_node_.fallback_node_ is also jsonnode<charT>.
template <typename charT, typename utftype>
jsonnode<charT, utftype> jsonnode<charT, utftype>::fallback_node_(
    jsonnode<charT, utftype>::fallback);

template <typename charT, typename utftype>
const typename jsonnode<charT, utftype>::char_type
    jsonnode<charT, utftype>::emptystr_[] = {0};
//  jsonnode

}  //  namespace json_internal_

typedef json_internal_::jsonnode<char, json_internal_::utf8tag> json;
typedef json_internal_::jsonnode<char, json_internal_::utf8tag> u8cjson;

#if defined(WCHAR_MAX) && (WCHAR_MAX >= 0xffff)
#if (WCHAR_MAX >= 0x10ffff)
#else
typedef json_internal_::jsonnode<wchar_t, json_internal_::utf16tag> wjson;
typedef json_internal_::jsonnode<wchar_t, json_internal_::utf16tag> u16wjson;
#endif
#endif

#if defined(__cpp_unicode_characters)
typedef json_internal_::jsonnode<char16_t, json_internal_::utf16tag> u16json;
#endif

#if defined(__cpp_char8_t)
typedef json_internal_::jsonnode<char8_t, json_internal_::utf8tag> u8json;
#endif

}  //  namespace NAMESPACE_CTLJSON
#endif  //  CTL_JSON_TEMPLATE_LIBRARY
