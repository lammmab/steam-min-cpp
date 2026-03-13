#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <cstdint>

namespace Steam::Utils::VDF {

    struct VDFNode;

    using Object = std::unordered_map<std::string, VDFNode>;

    using Value = std::variant<
        std::string,
        int32_t,
        uint64_t,
        float,
        Object
    >;

    struct VDFNode {
        Value value;

        Object& as_object() {
            return std::get<Object>(value);
        }

        const Object& as_object() const {
            return std::get<Object>(value);
        }

        std::string& as_string() {
            return std::get<std::string>(value);
        }

        const std::string& as_string() const {
            return std::get<std::string>(value);
        }

        int32_t as_int() const {
            return std::get<int32_t>(value);
        }

        uint64_t as_uint64() const {
            return std::get<uint64_t>(value);
        }

        float as_float() const {
            return std::get<float>(value);
        }
    };

    Object parse_text(std::string_view text);
    Object parse_binary(const uint8_t* data, size_t size);

    static void stringify_object(
    const Object& obj,
    std::string& out,
    int indent = 0)
    {
        std::string pad(indent, '\t');

        for (const auto& [key, node] : obj)
        {
            const auto& val = node.value;

            if (std::holds_alternative<Object>(val))
            {
                out += pad + "\"" + key + "\"\n";
                out += pad + "{\n";

                stringify_object(std::get<Object>(val), out, indent + 1);

                out += pad + "}\n";
            }
            else if (std::holds_alternative<std::string>(val))
            {
                out += pad + "\"" + key + "\" \"" +
                    std::get<std::string>(val) + "\"\n";
            }
            else if (std::holds_alternative<int32_t>(val))
            {
                out += pad + "\"" + key + "\" \"" +
                    std::to_string(std::get<int32_t>(val)) + "\"\n";
            }
            else if (std::holds_alternative<uint64_t>(val))
            {
                out += pad + "\"" + key + "\" \"" +
                    std::to_string(std::get<uint64_t>(val)) + "\"\n";
            }
            else if (std::holds_alternative<float>(val))
            {
                out += pad + "\"" + key + "\" \"" +
                    std::to_string(std::get<float>(val)) + "\"\n";
            }
        }
    }

    inline std::string stringify(const Object& obj)
    {
        std::string out;
        stringify_object(obj, out, 0);
        return out;
    }
}