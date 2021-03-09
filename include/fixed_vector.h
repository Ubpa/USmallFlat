#pragma once

#include <cassert>
#include <stdexcept>
#include <algorithm>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 26495 )
#endif

namespace Ubpa {
    template <class T, std::size_t N>
    class fixed_vector {
    public:
        //////////////////
        // Member types //
        //////////////////

        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = T*;
        using const_pointer = const T*;
        // TODO : improve iterator
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        //////////////////////
        // Member functions //
        //////////////////////

        constexpr fixed_vector() noexcept : size_{ 0 } {}

        constexpr explicit fixed_vector(size_type count) : size_{ count } {
            assert(count <= N);
            for (value_type& elem : *this)
                new(&elem)value_type{};
        }

        constexpr fixed_vector(size_type count, const value_type& value) : size_{ count } {
            assert(count <= N);
            for (value_type& elem : *this)
                new(&elem)value_type{ value };
        }

        constexpr fixed_vector(const fixed_vector& other) : size_(other.size_) {
            if constexpr (std::is_trivially_copy_constructible_v<value_type>)
                std::memcpy(&storage_, &other.storage_, size_ * sizeof(value_type));
            else {
                pointer cursor = data();
                for (const value_type& elem : other)
                    new(cursor++)value_type{ elem };
            }
        }

        constexpr fixed_vector(fixed_vector&& other) noexcept : size_(std::move(other.size_)) {
            if constexpr (std::is_trivially_move_constructible_v<value_type>)
                std::memcpy(&storage_, &other.storage_, size_ * sizeof(value_type));
            else {
                pointer cursor = data();
                for (value_type& elem : other)
                    new(cursor++)value_type{ std::move(elem) };
            }

            if constexpr (!std::is_trivially_destructible_v<value_type>) {
                for (value_type& elem : other)
                    elem.~value_type();
            }
            other.size_ = 0;
        }

        constexpr fixed_vector(std::initializer_list<value_type> initlist) : size_{ initlist.size() } {
            if constexpr (std::is_trivially_move_constructible_v<value_type>)
                std::memcpy(&storage_, initlist.begin(), size_ * sizeof(value_type));
            else {
                pointer cursor = data();
                for (const value_type& elem : initlist)
                    new(cursor++)value_type{ elem };
            }
        }

        ~fixed_vector() {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for (value_type& elem : *this)
                    elem.~value_type();
            }
        }

        constexpr fixed_vector& operator=(const fixed_vector& rhs) {
            if (this != &rhs) {
                if (size_ > rhs.size_) {
                    if constexpr (!std::is_trivially_destructible_v<T>) {
                        pointer last = data() + size_;
                        for (pointer cursor = data() + rhs.size_; cursor != last; ++cursor)
                            cursor->~value_type();
                    }
                    std::copy(rhs.data(), rhs.data() + rhs.size_, data());
                }
                else {
                    if constexpr (std::is_trivially_copy_assignable_v<value_type>) {
                        if constexpr (std::is_trivially_copy_constructible_v<value_type>)
                            std::memcpy(&storage_, &rhs.storage_, rhs.size_ * sizeof(value_type));
                        else { // copy assignment + copy ctor
                            const_pointer copy_ctor_cursor = rhs.data() + size_;
                            std::copy(rhs.data(), copy_ctor_cursor, data());

                            pointer last = data() + rhs.size_;
                            for (pointer cursor = data() + size_; cursor != last; ++cursor)
                                new(cursor) value_type{ *copy_ctor_cursor++ };
                        }
                    }
                    else {
                        const_pointer copy_ctor_cursor = rhs.data() + size_;
                        std::copy(rhs.data(), copy_ctor_cursor, data());

                        if constexpr (std::is_trivially_copy_constructible_v<value_type>)
                            std::memcpy(data() + size_, rhs.data() + size_, (rhs.size_ - size_) * sizeof(value_type));
                        else {
                            pointer last = data() + rhs.size_;
                            for (pointer cursor = data() + size_; cursor != last; ++cursor)
                                new(cursor)value_type{ *copy_ctor_cursor++ };
                        }
                    }
                }
                size_ = rhs.size_;
            }
            return *this;
        }

        constexpr fixed_vector& operator=(fixed_vector&& rhs) noexcept {
            if (this != &rhs) {
                if (size_ > rhs.size_) {
                    if constexpr (!std::is_trivially_destructible_v<T>) {
                        pointer last = data() + size_;
                        for (pointer cursor = data() + rhs.size_; cursor != last; ++cursor)
                            cursor->~value_type();
                    }
                    std::move(rhs.data(), rhs.data() + rhs.size_, data());
                }
                else {
                    if constexpr (std::is_trivially_move_assignable_v<value_type>) {
                        if constexpr (std::is_trivially_move_constructible_v<value_type>)
                            std::memcpy(&storage_, &rhs.storage_, rhs.size_ * sizeof(value_type));
                        else { // move assignment + move ctor
                            pointer move_ctor_cursor = rhs.data() + size_;
                            std::move(rhs.data(), move_ctor_cursor, data());

                            pointer last = data() + rhs.size_;
                            for (pointer cursor = data() + size_; cursor != last; ++cursor)
                                new(cursor) value_type{ std::move(*move_ctor_cursor++) };
                        }
                    }
                    else {
                        pointer move_ctor_cursor = rhs.data() + size_;
                        std::move(rhs.data(), move_ctor_cursor, data());

                        if constexpr (std::is_trivially_move_constructible_v<value_type>)
                            std::memcpy(data() + size_, rhs.data() + size_, (rhs.size_ - size_) * sizeof(value_type));
                        else {
                            pointer last = data() + rhs.size_;
                            for (pointer cursor = data() + size_; cursor != last; ++cursor)
                                new(cursor)value_type{ std::move(*move_ctor_cursor++) };
                        }
                    }
                }
                size_ = rhs.size_;
                rhs.clear();
            }
            return *this;
        }

        constexpr fixed_vector& operator=(std::initializer_list<value_type> rhs) {
            const std::size_t rhs_size = rhs.size();
            if (size_ > rhs_size) {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    pointer last = data() + size_;
                    for (pointer cursor = data() + rhs_size; cursor != last; ++cursor)
                        cursor->~value_type();
                }
                std::copy(rhs.begin(), rhs.end(), data());
            }
            else {
                if constexpr (std::is_trivially_move_assignable_v<value_type>) {
                    if constexpr (std::is_trivially_move_constructible_v<value_type>)
                        std::memcpy(&storage_, rhs.data(), rhs_size * sizeof(value_type));
                    else { // move assignment + move ctor
                        const_pointer move_ctor_cursor = rhs.data() + size_;
                        std::move(rhs.data(), move_ctor_cursor, data());

                        pointer last = data() + rhs_size;
                        for (pointer cursor = data() + size_; cursor != last; ++cursor)
                            new(cursor) value_type{ std::move(*move_ctor_cursor++) };
                    }
                }
                else {
                    const_pointer move_ctor_cursor = rhs.data() + size_;
                    std::move(rhs.data(), move_ctor_cursor, data());

                    if constexpr (std::is_trivially_move_constructible_v<value_type>)
                        std::memcpy(data() + size_, rhs.data() + size_, (rhs_size - size_) * sizeof(value_type));
                    else {
                        pointer last = data() + rhs_size;
                        for (pointer cursor = data() + size_; cursor != last; ++cursor)
                            new(cursor)value_type{ std::move(*move_ctor_cursor++) };
                    }
                }
            }
            size_ = rhs_size;
        }
        
        // TODO: assign

        //
        // Element access
        ///////////////////

        constexpr reference at(size_type pos) {
            if (pos >= size())
                throw_out_of_range();

            return *(data() + pos);
        }

        constexpr const_reference at(size_type pos) const {
            if (pos >= size())
                throw_out_of_range();

            return *(data() + pos);
        }

        constexpr reference operator[](size_type pos) noexcept {
            assert(pos < size());
            return *(data() + pos);
        }

        constexpr const_reference operator[](size_type pos) const noexcept {
            assert(pos < size());
            return *(data() + pos);
        }

        constexpr reference front() noexcept {
            assert(!empty());
            return *data();
        }

        constexpr const_reference front() const noexcept {
            assert(!empty());
            return *data();
        }

        constexpr reference back() noexcept {
            assert(!empty());
            return *(data() + size_ - 1);
        }

        constexpr const_reference back() const noexcept {
            assert(!empty());
            return *(data() + size_ - 1);
        }

        constexpr pointer data() noexcept { return reinterpret_cast<pointer>(&storage_); }
        constexpr const_pointer data() const noexcept { return reinterpret_cast<const_pointer>(&storage_); }

        //
        // Iterators
        //////////////

        iterator begin() noexcept { return data(); }
        const_iterator begin() const noexcept { return data(); }
        const_iterator cbegin() const noexcept { return begin(); }

        constexpr iterator end() noexcept { return data() + size_; }
        constexpr const_iterator end() const noexcept { return data() + size_; }
        constexpr const_iterator cend() const noexcept { return end(); }

        constexpr reverse_iterator rbegin() noexcept { return reverse_iterator{ end() }; }
        constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{ end() }; }
        constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

        constexpr reverse_iterator rend() noexcept { return reverse_iterator{ begin() }; }
        constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{ begin() }; }
        constexpr const_reverse_iterator crend() const noexcept { return rend(); }

        //
        // Capacity
        /////////////

        constexpr bool empty() const noexcept { return size_ == 0; }

        constexpr size_type size() const noexcept { return size_; }

        constexpr size_type max_size() const noexcept { return N; }

        constexpr size_type capacity() const noexcept { return N; }

        //
        // Modifiers
        //////////////

        constexpr void clear() noexcept {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for(value_type& elem : *this)
                    elem.~value_type();
            }
            size_ = 0;
        }

        // TODO: insert

        // TODO: emplace

        // TODO: erase

        constexpr void push_back(const value_type& value) {
            if (size() == max_size())
                throw_length_error();
            new(data() + size_)value_type{ value };
            ++size_;
        }

        constexpr void push_back(T&& value) {
            if (size() == max_size())
                throw_length_error();
            new(data() + size_)value_type{ std::move(value) };
            ++size_;
        }

        template<typename... Args>
        constexpr reference emplace_back(Args&&... args) {
            if (size() == max_size())
                throw_length_error();
            pointer addr = data() + size_;
            new(addr)value_type{ std::forward<Args>(args)... };
            ++size_;
            return *addr;
        }

        constexpr void pop_back() {
            assert(!empty());
            if constexpr (!std::is_trivially_destructible_v<value_type>)
                back().~value_type();
            --size_;
        }

        constexpr void resize(size_type count) {
            if (count > max_size())
                throw_length_error();

            if (count > size_) {
                pointer last = data() + count;
                for (pointer cursor = data() + size_; cursor != last; ++cursor)
                    new(cursor) value_type{};
            }
            else {
                if constexpr (!std::is_trivially_destructible_v<value_type>) {
                    pointer last = data() + size_;
                    for (pointer cursor = data() + count; cursor != last; ++cursor)
                        cursor->~value_type();
                }
            }

            size_ = count;
        }

        constexpr void resize(size_type count, const T& value) {
            if (count > max_size())
                throw_length_error();

            if (count > size_) {
                pointer last = data() + count;
                for (pointer cursor = data() + size_; cursor != last; ++cursor)
                    new(cursor) value_type{value};
            }
            else {
                if constexpr (!std::is_trivially_destructible_v<value_type>) {
                    pointer last = data() + size_;
                    for (pointer cursor = data() + count; cursor != last; ++cursor)
                        cursor->~value_type();
                }
            }

            size_ = count;
        }

        void swap(fixed_vector& other) noexcept {
            pointer from, to;
            std::size_t from_size, to_size;

            if (size_ > other.size_) {
                from = data();
                to = other.data();
                from_size = size_;
                to_size = other.size_;
            }
            else {
                from = other.data();
                to = data();
                from_size = other.size_;
                to_size = size_;
            }


            // low  : [0, to_size)
            // high : [to_size, from_size)
            // rest : [from_size, N)

            // 1. to   low  --move-ctor----> temp low
            // 2. from low  --move-assign--> to   low
            // 3. from high --move-ctor----> to   high
            // 4. temp low  --move-assign--> from low
            // 5. from high dtor
            // 6. temp low  dtor

            std::aligned_storage_t<sizeof(T)* N, alignof(T)> temp;
            pointer temp_addr = reinterpret_cast<pointer>(&temp);

            // 1, 2, 3
            if constexpr (std::is_trivially_move_constructible_v<value_type>) {
                std::memcpy(&temp, to, to_size * sizeof(value_type)); // 1
                if constexpr (std::is_trivially_move_assignable_v<value_type>)
                    std::memcpy(to, from, from_size * sizeof(value_type)); // 2, 3
                else {
                    std::move(from, from + to_size, to); // 2
                    std::memcpy(from + to_size, from + from_size, to + to_size); // 3
                }
            }
            else {
                // 1
                pointer temp_last = temp_addr + to_size;
                for (pointer temp_cursor = temp_addr, to_cursor = to; temp_cursor != temp_last; ++temp_cursor, ++to_cursor)
                    new(temp_cursor)value_type{ std::move(*to_cursor) };

                std::move(from, from + to_size, to); // 2

                // 3
                pointer to_last = to + from_size;
                for (pointer to_cursor = to + to_size, from_cursor = from + to_size; to_cursor != to_last; ++to_cursor, ++from_cursor)
                    new(to_cursor)value_type{ std::move(*from_cursor) };
            }

            std::move(temp_addr, temp_addr + to_size * sizeof(value_type), from); // 4

            if constexpr (!std::is_trivially_destructible_v<value_type>) {
                // 5
                pointer from_last = from + from_size;
                for (pointer cursor = from + to_size; cursor != from_last; ++cursor)
                    cursor->~value_type();

                // 6
                pointer temp_last = temp_addr + to_size;
                for (pointer cursor = temp_addr; cursor != temp_last; ++cursor)
                    cursor->~value_type();
            }

            std::swap(size_, other.size_);
        };

    private:
        [[noreturn]] void throw_out_of_range() const { throw std::out_of_range("invalid fixed_vector subscript"); }
        [[noreturn]] void throw_length_error() const { throw std::length_error("fixed_vector too long"); }

        std::aligned_storage_t<sizeof(T)* N, alignof(T)> storage_;
        size_type size_;
    };

    template<typename T, std::size_t N>
    constexpr bool operator==(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<typename T, std::size_t N>
    constexpr bool operator<(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename T, std::size_t N>
    constexpr bool operator!=(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return !(lhs == rhs);
    }

    template<typename T, std::size_t N>
    constexpr bool operator>(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return rhs < lhs;
    }

    template<typename T, std::size_t N>
    constexpr bool operator<=(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return !(rhs < lhs);
    }

    template<typename T, std::size_t N>
    constexpr bool operator>=(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return !(lhs < rhs);
    }
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
