#include <stdexcept>
#include <utility>
#include <new>
#include <type_traits>

namespace gx {
    class BadOptionalAccess : public std::exception {
    public:
        using exception::exception;

        virtual const char* what() const noexcept override {
            return "Bad optional access";
        }
    };

    template <typename T>
    class Optional {
    public:
        Optional() = default;
        Optional(const T& value) {
            new (data_) T(value);
            is_initialized_ = true;
        }
        Optional(T&& value) {
            new (data_) T(std::move(value));
            is_initialized_ = true;
        }
        Optional(const Optional& other) {
            if (other.is_initialized_) {
                new (data_) T(*other);
                is_initialized_ = true;
            }
        }
        Optional(Optional&& other) {
            if (other.is_initialized_) {
                new (data_) T(std::move(*other));
                is_initialized_ = true;
            }
        }

        Optional& operator=(const T& value) {
            if (is_initialized_) {
                **this = value;
            } else {
                new (data_) T(value);
                is_initialized_ = true;
            }
            return *this;
        }
        Optional& operator=(T&& rhs) {
            if (is_initialized_) {
                **this = std::move(rhs);
            } else {
                new (data_) T(std::move(rhs));
                is_initialized_ = true;
            }
            return *this;
        }
        Optional& operator=(const Optional& rhs) {
            if (this != &rhs) {
                if (is_initialized_ && rhs.is_initialized_) {
                    **this = *rhs;
                } else if (is_initialized_ && !rhs.is_initialized_) {
                    Reset();
                } else if (!is_initialized_ && rhs.is_initialized_) {
                    new (data_) T(*rhs);
                    is_initialized_ = true;
                }
            }
            return *this;
        }
        Optional& operator=(Optional&& rhs) {
            if (this != &rhs) {
                if (is_initialized_ && rhs.is_initialized_) {
                    **this = std::move(*rhs);
                } else if (is_initialized_ && !rhs.is_initialized_) {
                    Reset();
                } else if (!is_initialized_ && rhs.is_initialized_) {
                    new (data_) T(std::move(*rhs));
                    is_initialized_ = true;
                }
            }
            return *this;
        }

        ~Optional() {
            Reset();
        }

        [[nodiscard]] bool HasValue() const {
            return is_initialized_;
        }

        T& operator*() & {
            return reinterpret_cast<T&>(data_);
        }
        T&& operator*() && {
            return reinterpret_cast<T&&>(data_);
        }
        const T& operator*() const& {
            return reinterpret_cast<const T&>(data_);
        }

        T* operator->() {
            return reinterpret_cast<T*>(data_);
        }
        const T* operator->() const {
            return reinterpret_cast<const T*>(data_);
        }

        T& Value() & {
            if (!is_initialized_) {
                throw BadOptionalAccess();
            }

            return reinterpret_cast<T&>(data_);
        }

        const T& Value() const& {
            if (!HasValue()) {
                throw BadOptionalAccess();
            }

            return reinterpret_cast<const T&>(data_);
        }

        T&& Value() && {
            if (!is_initialized_) {
                throw BadOptionalAccess();
            }

            return reinterpret_cast<T&&>(data_);
        }

        void Reset() {
            if (is_initialized_) {
                reinterpret_cast<T*>(data_)->~T();
                is_initialized_ = false;
            }
        }

        template <typename... Args>
        void Emplace(Args&&... args) {
            Reset();
            new (data_) T(std::forward<decltype(args)>(args)...);
            is_initialized_ = true;
        }

    private:
        alignas(T) char data_[sizeof(T)];
        bool is_initialized_ = false;
    };

}