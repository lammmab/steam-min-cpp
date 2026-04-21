/// @internal
/// \cond
/*
MIT License

Copyright (c) 2018

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef MEDOOZE_EVENTEMITTER_H
#define MEDOOZE_EVENTEMITTER_H

#include <functional>
#include <map>
#include <memory>
#include <typeindex>
#include <typeinfo>

namespace medooze {

class EventEmitter {
 private:
  class BaseListener {
   public:
    BaseListener() = default;
    BaseListener(const BaseListener&) = delete;
    BaseListener& operator=(const BaseListener&) = delete;
    virtual ~BaseListener() {}
  };

  template <typename Type>
  class Listener : public BaseListener {
   public:
    explicit Listener(const std::function<void(const Type&)>& callback)
        : callback(callback) {}

    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;
    virtual ~Listener() {}

    void emit(const Type& type) {
      try {
        callback(type);
      } catch (...) {
        // ignore exceptions
      }
    }

   private:
    std::function<void(const Type&)> callback;
  };

 public:
  template <typename Type>
  void emit(const Type& type) {
    auto it = listeners.find(typeid(type));
    if (it != listeners.end())
      static_cast<Listener<Type>*>(it->second.get())->emit(type);
  }

  template <typename Type>
  EventEmitter& on(const std::function<void(const Type&)>& callback) {
    listeners[typeid(Type)] = std::make_unique<Listener<Type>>(callback);
    return *this;
  }

 private:
  std::map<std::type_index, std::unique_ptr<BaseListener>> listeners;
};

};  // namespace medooze

#endif /* MEDOOZE_EVENTEMITTER_H */
/// \endcond