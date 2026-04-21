/// @file vdf.hpp
/// Minimal representation and parser for Valve Data Format (VDF).
///
/// Provides a lightweight tree structure for reading VDF data used by
/// various Steam APIs such as appinfo and package metadata.

#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <cstdint>

namespace Steam::Utils::VDF {

    struct VDFNode;

    /// Object node type.
    ///
    /// Represents a dictionary of string keys mapped to VDF nodes.
    using Object = std::unordered_map<std::string, VDFNode>;

    /// Value stored in a VDF node.
    ///
    /// VDF values may be primitive types or nested objects.
    using Value = std::variant<
        std::string,
        int32_t,
        uint64_t,
        float,
        Object
    >;

    /// Node in a VDF tree.
    ///
    /// A node contains a variant value which may represent a primitive
    /// type or a nested object. Object nodes can be queried using
    /// `get` or `get_path`.
    struct VDFNode {

        /// Stored node value.
        Value value;

        /// Find a child node by key.
        ///
        /// @param key Object key to search for.
        /// @return Pointer to the child node or `nullptr` if not found.
        const VDFNode* get(const std::string& key) const {
            if (auto obj = std::get_if<Object>(&value)) {
                auto it = obj->find(key);
                if (it != obj->end())
                    return &it->second;
            }
            return nullptr;
        }

        /// @copydoc get
        VDFNode* get(const std::string& key) {
            if (auto obj = std::get_if<Object>(&value)) {
                auto it = obj->find(key);
                if (it != obj->end())
                    return &it->second;
            }
            return nullptr;
        }

        /// Retrieve a string value by key with a default fallback.
        ///
        /// @param key Key to search for.
        /// @param def Default value returned if the key is missing or
        ///            not a string.
        std::string get_string_or(const std::string& key, std::string def) const {
            if (auto node = get(key)) {
                if (auto val = std::get_if<std::string>(&node->value))
                    return *val;
            }
            return def;
        }

        /// Retrieve an `int32_t` value by key with fallback.
        int32_t get_int_or(const std::string& key, int32_t def) const {
            if (auto node = get(key)) {
                if (auto val = std::get_if<int32_t>(&node->value))
                    return *val;
            }
            return def;
        }

        /// Retrieve a `uint64_t` value by key with fallback.
        uint64_t get_uint64_or(const std::string& key, uint64_t def) const {
            if (auto node = get(key)) {
                if (auto val = std::get_if<uint64_t>(&node->value))
                    return *val;
            }
            return def;
        }

        /// Retrieve a `float` value by key with fallback.
        float get_float_or(const std::string& key, float def) const {
            if (auto node = get(key)) {
                if (auto val = std::get_if<float>(&node->value))
                    return *val;
            }
            return def;
        }

        /// Find a nested node using a path of keys.
        ///
        /// Example:
        /// @code
        /// node.get_path({"appinfo","common","name"});
        /// @endcode
        ///
        /// @param keys Sequence of nested object keys.
        /// @return Pointer to the final node or `nullptr`.
        const VDFNode* get_path(std::initializer_list<std::string> keys) const {
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

        /// Access the node as an object.
        ///
        /// @throws std::bad_variant_access if the node is not an object.
        Object& as_object() {
            return std::get<Object>(value);
        }

        /// @copydoc as_object
        const Object& as_object() const {
            return std::get<Object>(value);
        }

        /// Access the node as a string.
        ///
        /// @throws std::bad_variant_access if the node is not a string.
        std::string& as_string() {
            return std::get<std::string>(value);
        }

        /// @copydoc as_string
        const std::string& as_string() const {
            return std::get<std::string>(value);
        }

        /// Access the node as an `int32_t`.
        int32_t as_int() const {
            return std::get<int32_t>(value);
        }

        /// Access the node as a `uint64_t`.
        uint64_t as_uint64() const {
            return std::get<uint64_t>(value);
        }

        /// Access the node as a `float`.
        float as_float() const {
            return std::get<float>(value);
        }
    };

    /// Parse VDF text into an object tree.
    ///
    /// @param text VDF formatted text.
    /// @return Parsed root object.
    Object parse_text(std::string_view text);

    /// Parse binary VDF data.
    ///
    /// @param data Pointer to binary VDF buffer.
    /// @param size Size of the buffer in bytes.
    /// @return Parsed root object.
    Object parse_binary(const uint8_t* data, size_t size);

    /// Convert a VDF object into formatted text.
    ///
    /// @internal helper used by `stringify`.
    static void stringify_object(const Object& obj, std::string& out, int indent = 0);

    /// Convert a VDF object into a human-readable string.
    ///
    /// Useful for debugging or logging VDF structures.
    inline std::string stringify(const Object& obj)
    {
        std::string out;
        stringify_object(obj, out, 0);
        return out;
    }
}