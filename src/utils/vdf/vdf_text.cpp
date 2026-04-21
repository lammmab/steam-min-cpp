#include <cctype>
#include <stdexcept>
#include <steamclient/utilities/vdf.hpp>

namespace Steam::Utils::VDF {
class TextParser {
 public:
  TextParser(std::string_view s) : input(s), pos(0) {}

  Object parse() {
    Object root;

    while (!eof()) {
      skip_ws();

      if (eof()) break;

      auto key = parse_string();
      skip_ws();

      if (peek() == '{') {
        consume();
        root[key].value = parse_object();
      } else {
        auto val = parse_string();
        root[key].value = val;
      }
    }

    return root;
  }

 private:
  std::string_view input;
  size_t pos;

  bool eof() const { return pos >= input.size(); }

  char peek() const { return input[pos]; }

  char consume() { return input[pos++]; }

  void skip_ws() {
    while (!eof() && std::isspace(peek())) pos++;

    if (!eof() && peek() == '/' && pos + 1 < input.size() &&
        input[pos + 1] == '/') {
      while (!eof() && peek() != '\n') pos++;
      skip_ws();
    }
  }

  std::string parse_string() {
    skip_ws();

    if (consume() != '"') throw std::runtime_error("Expected quote");

    std::string out;

    while (!eof()) {
      char c = consume();

      if (c == '"') break;

      if (c == '\\') {
        char n = consume();
        switch (n) {
          case 'n':
            out += '\n';
            break;
          case 't':
            out += '\t';
            break;
          case '\\':
            out += '\\';
            break;
          case '"':
            out += '"';
            break;
          default:
            out += n;
        }
      } else {
        out += c;
      }
    }

    return out;
  }

  Object parse_object() {
    Object obj;

    while (true) {
      skip_ws();

      if (peek() == '}') {
        consume();
        break;
      }

      auto key = parse_string();
      skip_ws();

      if (peek() == '{') {
        consume();
        obj[key].value = parse_object();
      } else {
        auto val = parse_string();
        obj[key].value = val;
      }
    }

    return obj;
  }
};

Object parse_text(std::string_view text) {
  TextParser parser(text);
  return parser.parse();
}

static void stringify_object(const Object& obj, std::string& out, int indent) {
  std::string pad(indent, '\t');

  for (const auto& [key, node] : obj) {
    const auto& val = node.value;

    if (std::holds_alternative<Object>(val)) {
      out += pad + "\"" + key + "\"\n";
      out += pad + "{\n";

      stringify_object(std::get<Object>(val), out, indent + 1);

      out += pad + "}\n";
    } else if (std::holds_alternative<std::string>(val)) {
      out += pad + "\"" + key + "\" \"" + std::get<std::string>(val) + "\"\n";
    } else if (std::holds_alternative<int32_t>(val)) {
      out += pad + "\"" + key + "\" \"" +
             std::to_string(std::get<int32_t>(val)) + "\"\n";
    } else if (std::holds_alternative<uint64_t>(val)) {
      out += pad + "\"" + key + "\" \"" +
             std::to_string(std::get<uint64_t>(val)) + "\"\n";
    } else if (std::holds_alternative<float>(val)) {
      out += pad + "\"" + key + "\" \"" + std::to_string(std::get<float>(val)) +
             "\"\n";
    }
  }
}
}  // namespace Steam::Utils::VDF
