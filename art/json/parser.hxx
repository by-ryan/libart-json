#ifndef art__json__parser_hxx_
#define art__json__parser_hxx_

#include <art/json/diagnostics.hxx>
#include <art/json/exception.hxx>
#include <art/json/optional.hxx>
#include <art/json/variant.hxx>

#include <art/unicode/utf8-decoder.hxx>
#include <art/unicode/utf8-encoder.hxx>

#include <iostream> // TODO remove
#include <sstream>
#include <string>
#include <limits>

namespace art::json {

class variant;

class parser {
public:
  parser() = default;

  template< typename InputIterator >
  optional< variant >
  try_parse(diagnostics& d, InputIterator& first, InputIterator last)
  {
    auto v = parse(d, first, last);

    if (d.errors().size() > 0)
      return variant::undefined;

    return v;
  }

  template< typename InputIterator >
  optional< variant >
  try_parse(diagnostics& d, InputIterator&& first, InputIterator last)
  {
    return try_parse(d, first, last);
  }

  template< typename InputIterator >
  variant
  parse(diagnostics& d, InputIterator& first, InputIterator last)
  {
    skip_whitespace(first, last);

    auto v = try_parse_any(d, first, last);

    if (d.errors().size() > 0)
      return variant::undefined;

    skip_whitespace(first, last);

    if (first != last) {
      d.error(location(), "unexpected trailing data");
      return variant::undefined;
    }

    return v;
  }

  template< typename InputIterator >
  variant
  parse(diagnostics& d, InputIterator&& first, InputIterator last)
  {
    return parse(d, first, last);
  }

private:
  template< typename InputIterator >
  variant
  try_parse_undefined(diagnostics& d, InputIterator& i, InputIterator last)
  {
    auto loc = location();

    if (*i != 'n') {
      d.error(location(), "expected 'n'");
      return { loc, variant::undefined };
    }

    advance(i, last);

    if (i == last || *i != 'u') {
      d.error(location(), "expected 'u'");
      return { loc, variant::undefined };
    }

    advance(i, last);

    if (i == last || *i != 'l') {
      d.error(location(), "expected 'l'");
      return { loc, variant::undefined };
    }

    advance(i, last);

    if (i == last || *i != 'l') {
      d.error(location(), "expected 'l'");
      return { loc, variant::undefined };
    }

    advance(i, last);

    return { std::move(loc), variant::undefined };
  }

  template< typename InputIterator >
  variant
  try_parse_array(diagnostics& d, InputIterator& i, InputIterator last)
  {
    auto loc = location();

    if (*i != '[') {
      d.error(location(), "expected '['");
      return variant::undefined;
    }

    advance(i, last); // skip '['

    skip_whitespace(i, last);

    if (i == last) {
      d.error(location(), "premature end");
      return variant::undefined;
    }

    if (*i == ']') {
      advance(i, last);
      return variant{ std::move(loc), std::vector< variant >{} };
    }

    std::vector< variant > a;

    for (;;) {
      auto v = try_parse_any(d, i, last);

      if (d.errors().size() > 0)
        return variant::undefined;

      a.emplace_back(std::move(v));

      skip_whitespace(i, last);

      if (i == last) {
        d.error(location(), "premature end");
        return variant::undefined;
      }

      if (',' != *i)
        break;

      advance(i, last);

      skip_whitespace(i, last);
    }

    if (i == last) {
      d.error(location(), "premature end");
      return variant::undefined;
    }

    if (*i != ']') {
      d.error(location(), "expected ']'");
      return variant::undefined;
    }

    advance(i, last);

    return variant{ std::move(loc), std::move(a) };
  }

  template< typename InputIterator >
  variant
  try_parse_boolean(diagnostics& d, InputIterator& i, InputIterator last)
  {
    auto loc = location();

    if (*i == 't') {
      advance(i, last);

      if (i == last) {
        d.error(location(), "premature end");
        return variant::undefined;
      }

      if (*i != 'r') {
        d.error(location(), "unexpected character");
        return variant::undefined;
      }

      advance(i, last); // skip 'r'

      if (i == last) {
        d.error(location(), "premature end");
        return variant::undefined;
      }

      if (*i != 'u') {
        d.error(location(), "unexpected character");
        return variant::undefined;
      }

      advance(i, last); // skip 'u'

      if (i == last) {
        d.error(location(), "premature end");
        return variant::undefined;
      }

      if (*i != 'e') {
        d.error(location(), "unexpected character");
        return variant::undefined;
      }

      advance(i, last); // skip 'e'

      return variant{ std::move(loc), true };
    }

    if (*i != 'f')
      return {};

    advance(i, last);

    if (i == last || *i != 'a')
      return {};

    advance(i, last);

    if (i == last || *i != 'l')
      return {};

    advance(i, last);

    if (i == last || *i != 's')
      return {};

    advance(i, last);

    if (i == last || *i != 'e')
      return {};

    advance(i, last);

    return variant{ std::move(loc), false };
  }

  template< typename InputIterator >
  variant
  try_parse_number(diagnostics& d, InputIterator& i, InputIterator last)
  {
    auto loc = location();

    std::string buf;
    buf.reserve(std::numeric_limits< long long >::digits10 * 2);

    bool is_signed{ false };

    if (i != last && *i == '-') {
      buf.push_back(*i++);
      is_signed = true;
    }

    if (i == last) {
      d.error(location(), "premature end");
      return variant::undefined;
    }

    // int
    if (*i == '0') {
      buf.push_back(*i++);
    }
    else if ('1' <= *i && *i <= '9') {
      do {
        buf.push_back(*i++);
      } while (i != last && '0' <= *i && *i <= '9');
    }
    else {
      d.error(location(), "unexpected character");
      return variant::undefined;
    }

    bool is_real{ false };

    // frac
    if (i != last && *i == '.') {
      is_real = true;
      buf.push_back(*i++);

      if (i == last) {
        d.error(location(), "premature end");
        return variant::undefined;
      }

      if (*i < '0' || '9' < *i) {
        d.error(location(), "unexpected character");
        return variant::undefined;
      }

      do {
        buf.push_back(*i++);
      } while (i != last && '0' <= *i && *i <= '9');
    }

    // exp
    if (i != last && (*i == 'e' || *i == 'E')) {
      is_real = true;
      buf.push_back(*i++);

      if (i != last && (*i == '-' || *i == '+'))
        buf.push_back(*i++);

      if (i == last) {
        d.error(location(), "premature end");
        return variant::undefined;
      }

      if (*i < '0' || '9' < *i) {
        d.error(location(), "unexpected character");
        return variant::undefined;
      }

      do {
        buf.push_back(*i++);
      } while (i != last && '0' <= *i && *i <= '9');
    }

    try {
      if (is_real) {
        return variant{ loc, std::stold(buf) };
      }

      if (is_signed) {
        return variant{ loc, std::stoll(buf) };
      }

      return variant{ loc, std::stoull(buf) };
    }
    catch (std::out_of_range const&) {
      d.error(loc, "number too large");
    }

    return variant::undefined;
  }

  template< typename InputIterator >
  variant
  try_parse_object(diagnostics& d, InputIterator& i, InputIterator last)
  {
    auto loc = location();

    if (*i != '{')
      return {};

    advance(i, last);

    skip_whitespace(i, last);

    if (i == last)
      return {};

    if (*i == '}') {
      advance(i, last);
      return std::map< std::string, variant >{};
    }

    std::map< std::string, variant > o;

    for (;;) {
      auto k = try_parse_string(d, i, last);

      if (d.errors().size() > 0)
        return {};

      skip_whitespace(i, last);

      if (i == last || *i != ':')
        return {};

      advance(i, last);

      skip_whitespace(i, last);

      if (i == last) {
        d.error(location(), "unexpected end");
        return {};
      }

      auto v = try_parse_any(d, i, last);

      if (d.errors().size() > 0)
        return {};

      variant v_v{ std::move(v) };
      o.emplace(k.get_string(), std::move(v_v));

      skip_whitespace(i, last);

      if (i == last)
        return {};

      if (',' != *i)
        break;
      advance(i, last);

      skip_whitespace(i, last);
    }

    if (i == last || *i != '}')
      return {};

    advance(i, last);

    return variant{ std::move(loc), std::move(o) };
  }

  template< typename InputIterator >
  variant
  try_parse_string(diagnostics& d, InputIterator& i, InputIterator last)
  {
    auto loc = location();

    std::string text;

    if (*i != '"') {
      d.error(location(), "unexpected character");
      return variant::undefined;
    }

    advance(i, last);

    if (i == last) {
      d.error(location(), "premature end");
      return variant::undefined;
    }

    while (i != last && *i != '"') {
      if (*i == '\\') { // Parse escape sequence.
        advance(i, last); // skip '\'

        if (i == last) {
          d.error(location(), "premature end");
          return variant::undefined;
        }

        switch (*i) {
          case '"':
            text += '\x22';
            advance(i, last);
            break;
          case '\\':
            text += '\x5c';
            advance(i, last);
            break;
          case '/':
            text += '\x2f';
            advance(i, last);
            break;
          case 'b':
            text += '\x62';
            advance(i, last);
            break;
          case 'f':
            text += '\x66';
            advance(i, last);
            break;
          case 'n':
            text += '\x6e';
            advance(i, last);
            break;
          case 'r':
            text += '\x72';
            advance(i, last);
            break;
          case 't':
            text += '\x74';
            advance(i, last);
            break;
          case 'u': {
            advance(i, last); // skip u
            std::uint32_t u[4];

            for (int index = 0; index < 4; ++index, advance(i, last)) {
              if (i == last) {
                d.error(location(), "premature end");
                return variant::undefined;
              }

              std::uint32_t c = (unsigned char)*i;

              if (U'0' <= c && c <= U'9') {
                u[index] = c - U'0';
              }
              else if (U'a' <= c && c <= U'f') {
                u[index] = (c - U'a') + 10;
              }
              else if (U'A' <= c && c <= U'F') {
                u[index] = (c - U'A') + 10;
              }
              else {
                d.error(location(), "expected digit");
                return variant::undefined;
              }
            }

            std::uint32_t utf32 = u[0] << 12 | u[1] << 8 | u[2] << 4 | u[3];

            if (0xdc00 <= utf32 && utf32 <= 0xdfff) {
              d.error(location(), "invalid unicode code point");
              return variant::undefined;
            }

            if (0xd800 <= utf32 && utf32 <= 0xdbff) {
              // We found a high surrogate, expect a following low-surrogate.
              auto high = utf32;

              if (i == last) {
                d.error(location(), "premature end");
                return variant::undefined;
              }

              if (*i != '\\') {
                d.error(location(), "unexpected character");
                return variant::undefined;
              }

              advance(i, last); // skip backslash

              if (i == last) {
                d.error(location(), "premature end");
                return variant::undefined;
              }

              if (*i != 'u') {
                d.error(location(), "unexpected character2");
                return variant::undefined;
              }

              advance(i, last); // skip 'u'

              for (int index = 0; index < 4; ++index, advance(i, last)) {
                if (i == last) {
                  d.error(location(), "premature end");
                  return variant::undefined;
                }

                std::uint32_t c = (unsigned char)*i;

                if (U'0' <= c && c <= U'9') {
                  u[index] = c - U'0';
                }
                else if (U'a' <= c && c <= U'f') {
                  u[index] = (c - U'a') + 10;
                }
                else if (U'A' <= c && c <= U'F') {
                  u[index] = (c - U'A') + 10;
                }
                else {
                  d.error(location(), "expected digit");
                  return {};
                }
              }

              std::uint32_t low = u[0] << 12 | u[1] << 8 | u[2] << 4 | u[3];

              utf32 = (high << 10) + low - 0x35fdc00;
            }

            std::string str;
            unicode::string_writer_t writer{str};
            unicode::utf8_encoder_t encoder{writer};
            encoder.encode(utf32);

            text += str;
            break;
          }
        }

        continue;
      }

      if (is_control(i)) {
        d.error(location(), "invalid character detected");
        return variant::undefined;
      }

      text += *i++;
    }

    if (i == last) {
      d.error(location(), "premature end");
      return variant::undefined;
    }

    if (*i != '"') {
      d.error(location(), "unexpected character");
      return variant::undefined;
    }

    advance(i, last);

    return variant{ std::move(loc), std::move(text) };
  }

  template< typename InputIterator >
  variant
  try_parse_any(diagnostics& d, InputIterator& i, InputIterator last)
  {
    auto loc = location();

    if (i == last) {
      d.error(loc, "premature end");
      return variant::undefined;
    }

    switch (*i) {
      case 'n':
        return try_parse_undefined(d, i, last);
      case '[':
        return try_parse_array(d, i, last);
      case 't':
      case 'f':
        return try_parse_boolean(d, i, last);
      case '-':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        return try_parse_number(d, i, last);
      case '{':
        return try_parse_object(d, i, last);
      case '"':
        return try_parse_string(d, i, last);
    }

    std::ostringstream str;
    str << "unexpected character '" << *i << "'";
    d.error(loc, str.str());

    return variant::undefined;
  }

  template< typename InputIterator >
  void
  skip_whitespace(InputIterator& i, InputIterator last)
  {
    while (i != last && is_whitespace< InputIterator >(i))
      advance(i, last);
  }

  template< typename InputIterator >
  bool
  is_whitespace(InputIterator const& c)
  {
    switch (*c) {
      case 0x09: // tab
      case 0x0a: // lf
      case 0x0d: // cr
      case 0x20: // space
        return true;
    }

    return false;
  }

  template< typename InputIterator >
  bool
  is_control(InputIterator const& c)
  {
    switch (*c) {
      case '\x00':
      case '\x01':
      case '\x02':
      case '\x03':
      case '\x04':
      case '\x05':
      case '\x06':
      case '\x07':
      case '\x08':
      case '\x09':
      case '\x0A':
      case '\x0B':
      case '\x0C':
      case '\x0D':
      case '\x0E':
      case '\x0F':
      case '\x10':
      case '\x11':
      case '\x12':
      case '\x13':
      case '\x14':
      case '\x15':
      case '\x16':
      case '\x17':
      case '\x18':
      case '\x19':
      case '\x1A':
      case '\x1B':
      case '\x1C':
      case '\x1D':
      case '\x1E':
      case '\x1F':
        return true;
    }

    return false;
  }

  diagnostics::location
  location()
  {
    return diagnostics::location{ line_, column_ };
  }

  template< typename InputIterator >
  void
  advance(InputIterator& i, InputIterator last)
  {
    if (i == last)
      return;

    if (*i == '\r') {
      ++line_;
      column_ = 1;
      ++i;

      if (i != last && *i == '\n')
        ++i;

      return;
    }

    if (*i == '\n') {
      ++line_;
      column_ = 1;
      ++i;
      return;
    }

    ++column_;
    ++i;
  }

  std::uint32_t line_{ 1 };
  std::uint32_t column_{ 1 };
};

} // namespace art::json

#endif
