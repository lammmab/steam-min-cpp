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
    
        const VDFNode* get(const std::string& key) const {
            return find(key);
        }

        VDFNode* get(const std::string& key) {
            return find(key);
        }

        // Find an object by key; Returns nullptr if not found (const).
        const VDFNode* find(const std::string& key) const {
            if (auto obj = std::get_if<Object>(&value)) {
                auto it = obj->find(key);
                if (it != obj->end())
                    return &it->second;
            }
            return nullptr;
        }

        // Find an object by key; Returns nullptr if not found.
        VDFNode* find(const std::string& key) {
            if (auto obj = std::get_if<Object>(&value)) {
                auto it = obj->find(key);
                if (it != obj->end())
                    return &it->second;
            }
            return nullptr;
        }

        // Find a string object by key; Returns the default string if not found.
        std::string get_string_or(const std::string& key, std::string def) const {
            if (auto node = find(key)) {
                if (auto val = std::get_if<std::string>(&node->value))
                    return *val;
            }
            return def;
        }

        // Find an int32_t object by key; Returns the default int32_t if not found.
        int32_t get_int_or(const std::string& key, int32_t def) const {
            if (auto node = find(key)) {
                if (auto val = std::get_if<int32_t>(&node->value))
                    return *val;
            }
            return def;
        }

        // Find a uint64_t object by key; Returns the default uint64_t if not found.
        uint64_t get_uint64_or(const std::string& key, uint64_t def) const {
            if (auto node = find(key)) {
                if (auto val = std::get_if<uint64_t>(&node->value))
                    return *val;
            }
            return def;
        }

        // Find a float object by key; Returns the default float if not found.
        float get_float_or(const std::string& key, float def) const {
            if (auto node = find(key)) {
                if (auto val = std::get_if<float>(&node->value))
                    return *val;
            }
            return def;
        }

        // Find an object by multiple keys; Returns nullptr if not found (const).
        const VDFNode* find_path(std::initializer_list<std::string> keys) const {
            const VDFNode* current = this;

            for (const auto& key : keys) {
                if (!current)
                    return nullptr;

                auto obj = std::get_if<Object>(&current->value);
                if (!obj)
                    return nullptr;

                auto it = obj->find(key);
                if (it == obj->end())
                    return nullptr;

                current = &it->second;
            }

            return current;
        }

        // Returns the current value as an object.
        Object& as_object() {
            return std::get<Object>(value);
        }

        // Returns the current value as an object (const).
        const Object& as_object() const {
            return std::get<Object>(value);
        }

        // Returns the current value as a string.
        std::string& as_string() {
            return std::get<std::string>(value);
        }

        // Returns the current value as a string (const).
        const std::string& as_string() const {
            return std::get<std::string>(value);
        }

        // Returns the current value as an int32_t.
        int32_t as_int() const {
            return std::get<int32_t>(value);
        }

        // Returns the current value as a uint64_t.
        uint64_t as_uint64() const {
            return std::get<uint64_t>(value);
        }

        // Returns the current value as a float.
        float as_float() const {
            return std::get<float>(value);
        }
    };

    // Parses VDF text.
    Object parse_text(std::string_view text);

    // Parses VDF from a raw data buffer.
    Object parse_binary(const uint8_t* data, size_t size);

    static void stringify_object(const Object& obj,std::string& out,int indent = 0);

    // Converts a VDF object to a readable string.
    inline std::string stringify(const Object& obj)
    {
        std::string out;
        stringify_object(obj, out, 0);
        return out;
    }
}