#pragma once

#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <concepts>
#include <type_traits>
#include <cstring>
#include <limits>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 26495 )
#endif

namespace Ubpa::details {
    template <std::size_t N>
    using static_vector_size_type
        = std::conditional_t<(N < std::numeric_limits<uint8_t>::max()), std::uint8_t,
        std::conditional_t<(N < std::numeric_limits<uint16_t>::max()), std::uint16_t,
        std::conditional_t<(N < std::numeric_limits<uint32_t>::max()), std::uint32_t,
        std::conditional_t<(N < std::numeric_limits<uint64_t>::max()), std::uint64_t,
        std::size_t>>>>;
}

namespace Ubpa {
    template <class T, std::size_t N = 16>
    class static_vector {
    public:
        //////////////////
        // Member types //
        //////////////////

        using value_type = T;
        using size_type = details::static_vector_size_type<N>;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = T*;
        using const_pointer = const T*;
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        //////////////////////
        // Member functions //
        //////////////////////

        static_vector() noexcept : m_size{ 0 } {}

        explicit static_vector(size_type count) : m_size{ count } {
            assert(count <= N);
            std::uninitialized_value_construct(begin(), end());
        }

        static_vector(size_type count, const value_type& value) : m_size{ count } {
            assert(count <= N);
            std::uninitialized_fill(begin(), end(), value);
        }

        static_vector(const static_vector& other) : m_size{ other.m_size } {
            std::uninitialized_copy(other.begin(), other.end(), begin());
        }

        static_vector(static_vector&& other) noexcept : m_size{ other.m_size } {
            std::uninitialized_move(other.begin(), other.end(), begin());
            other.clear();
        }

        static_vector(std::initializer_list<value_type> ilist) : m_size{ static_cast<size_type>(ilist.size()) } {
            assert(ilist.size() <= N);
            std::uninitialized_copy(ilist.begin(), ilist.end(), begin());
        }

        template<typename Iter> requires std::input_iterator<Iter>
        static_vector(Iter first, Iter last) : static_vector(first, last, typename std::iterator_traits<Iter>::iterator_category{}) {}

        ~static_vector() { std::destroy(begin(), end()); }

        static_vector& operator=(const static_vector& rhs) {
            if (this != &rhs) {
                if (m_size > rhs.m_size) {
                    std::destroy(begin() + rhs.m_size, end());
                    std::copy(rhs.begin(), rhs.end(), begin());
                }
                else {
                    if constexpr (std::is_trivially_copy_assignable_v<value_type> && std::is_trivially_copy_constructible_v<value_type>) {
                        std::memcpy(&m_storage, &rhs.m_storage, rhs.m_size * sizeof(value_type));
                    }
                    else {
                        std::copy(rhs.begin(), rhs.begin() + m_size, begin());
                        std::uninitialized_copy(rhs.begin() + m_size, rhs.end(), end());
                    }
                }
                m_size = rhs.m_size;
            }
            return *this;
        }

        static_vector& operator=(static_vector&& rhs) noexcept {
            if (this != &rhs) {
                if (m_size > rhs.m_size) {
                    std::destroy(begin() + rhs.m_size, end());
                    std::move(rhs.begin(), rhs.end(), begin());
                }
                else {
                    if constexpr (std::is_trivially_move_assignable_v<value_type> && std::is_trivially_move_constructible_v<value_type>) {
                        std::memcpy(&m_storage, &rhs.m_storage, rhs.m_size * sizeof(value_type));
                    }
                    else {
                        std::copy(rhs.begin(), rhs.begin() + m_size, begin());
                        std::uninitialized_move(rhs.begin() + m_size, rhs.end(), end());
                    }
                }
                m_size = rhs.m_size;
                rhs.clear();
            }
            return *this;
        }

        static_vector& operator=(std::initializer_list<value_type> rhs) {
            assert(rhs.size() <= N);
            const size_type rhs_size = static_cast<size_type>(rhs.size());
            if (m_size > rhs_size) {
                std::destroy(begin() + rhs_size, end());
                std::copy(rhs.begin(), rhs.end(), begin());
            }
            else {
                if constexpr (std::is_trivially_copy_assignable_v<value_type> && std::is_trivially_copy_constructible_v<value_type>) {
                    std::memcpy(&m_storage, rhs.begin(), rhs_size * sizeof(value_type));
                }
                else {
                    std::copy(rhs.begin(), rhs.begin() + m_size, begin());
                    std::uninitialized_copy(rhs.begin() + m_size, rhs.end(), end());
                }
            }
            m_size = rhs_size;
            return *this;
        }

        void assign(size_type count, const value_type& value) {
            assert(count <= N);

            const pointer myfirst = begin();
            const pointer mylast = end();

            if (count > m_size) {
                std::fill(myfirst, mylast, value);

                std::uninitialized_fill_n(mylast, count - m_size, value);
            }
            else {
                const pointer newlast = myfirst + count;
                std::fill(myfirst, newlast, value);
                std::destroy(newlast, mylast);
            }
            m_size = count;
        }

        template<class Iter> requires std::input_iterator<Iter>
        void assign(Iter first, Iter last) {
            assign_range(first, last, typename std::iterator_traits<Iter>::iterator_category{});
        }

        void assign(std::initializer_list<T> ilist) {
            assign_range(ilist.begin(), ilist.end(), std::random_access_iterator_tag{});
        }

        //
        // Element access
        ///////////////////

        reference at(size_type pos) {
            if (pos >= size())
                throw_out_of_range();

            return *(data() + pos);
        }

        const_reference at(size_type pos) const {
            if (pos >= size())
                throw_out_of_range();

            return *(data() + pos);
        }

        reference operator[](size_type pos) noexcept {
            assert(pos < size());
            return *(data() + pos);
        }

        const_reference operator[](size_type pos) const noexcept {
            assert(pos < size());
            return *(data() + pos);
        }

        reference front() noexcept {
            assert(!empty());
            return *data();
        }

        const_reference front() const noexcept {
            assert(!empty());
            return *data();
        }

        reference back() noexcept {
            assert(!empty());
            return *(data() + m_size - 1);
        }

        const_reference back() const noexcept {
            assert(!empty());
            return *(data() + m_size - 1);
        }

        pointer data() noexcept { return reinterpret_cast<pointer>(&m_storage); }
        const_pointer data() const noexcept { return reinterpret_cast<const_pointer>(&m_storage); }

        //
        // Iterators
        //////////////

        iterator begin() noexcept { return data(); }
        const_iterator begin() const noexcept { return data(); }
        const_iterator cbegin() const noexcept { return begin(); }

        iterator end() noexcept { return data() + m_size; }
        const_iterator end() const noexcept { return data() + m_size; }
        const_iterator cend() const noexcept { return end(); }

        reverse_iterator rbegin() noexcept { return reverse_iterator{ end() }; }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{ end() }; }
        const_reverse_iterator crbegin() const noexcept { return rbegin(); }

        reverse_iterator rend() noexcept { return reverse_iterator{ begin() }; }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator{ begin() }; }
        const_reverse_iterator crend() const noexcept { return rend(); }

        //
        // Capacity
        /////////////

        bool empty() const noexcept { return m_size == 0; }

        size_type size() const noexcept { return m_size; }

        size_type max_size() const noexcept { return N; }

        size_type capacity() const noexcept { return N; }

        //
        // Modifiers
        //////////////

        void clear() noexcept {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for(value_type& elem : *this)
                    elem.~value_type();
            }
            m_size = 0;
        }

        iterator insert(const_iterator pos, const value_type& value) {
            return emplace(pos, value);
        }

        iterator insert(const_iterator pos, value_type&& value) {
            return emplace(pos, std::move(value));
        }

        iterator insert(const_iterator pos, size_type count, const value_type& value) {
            assert(size() + count <= N);
            iterator last = end();
            assert(begin() <= pos && pos <= last);
            const pointer posptr = const_cast<pointer>(pos);

            const auto affected_elements = static_cast<size_type>(last - posptr);

            if (affected_elements == 0)
                ;
            else if (affected_elements < count) {
                // [pos, last) --move--> [last, ...)
                std::uninitialized_move(posptr, last, last);
                std::destroy(posptr, last);
            }
            else {
                if constexpr (std::is_trivially_move_assignable_v<value_type>) {
                    if constexpr (std::is_trivially_move_constructible_v<value_type>)
                        // 1. + 2. [pos, last) --move--> [pos+count, last+count)
                        std::memmove(posptr + count, posptr, (last - posptr) * sizeof(value_type));
                    else {
                        // 1. [last-count, last) -> [last, last+count)
                        std::uninitialized_move(last - count, last, last);
                        // 2. [pos, last-count) --move--> [pos+count, last)
                        std::memmove(posptr + count, posptr, (last - posptr - count) * sizeof(value_type));
                    }
                }
                else {
                    // 1. [last-count, last) -> [last, last+count)
                    std::uninitialized_move(last - count, last, last);
                    // 2. [pos, last-count) --move--> [pos+count, last)
                    for (auto cursor = last - count - 1; cursor >= pos; --cursor)
                        *(cursor + count) = std::move(*cursor);
                }

                std::destroy(posptr, posptr + count);
            }

            std::uninitialized_fill(posptr, posptr + count, value);

            m_size += count;

            return posptr;
        }
        
        template<typename Iter> requires std::input_iterator<Iter>
        iterator insert(const_iterator pos, Iter first, Iter last) {
            insert_range(pos, first, last, typename std::iterator_traits<Iter>::iterator_category{});
            return const_cast<iterator>(pos);
        }

        iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) {
            return insert(pos, ilist.begin(), ilist.end());
        }

        template<typename... Args>
        iterator emplace(const_iterator pos, Args&&... args) {
            pointer posptr = const_cast<pointer>(pos);
            assert(size() < max_size());
            iterator last = end();
            assert(begin() <= pos && pos <= last);

            if (posptr != last) {
                if constexpr (std::is_trivially_move_assignable_v<value_type>) {
                    if constexpr (std::is_trivially_move_constructible_v<value_type>)
                        // 1. + 2. [pos, last) --move--> [pos+1, last+1)
                        std::memmove(posptr + 1, posptr, (last - posptr) * sizeof(value_type));
                    else {
                        // 1. pos -> last
                        new(last)value_type{ std::move(*(last - 1)) };
                        // 2. [pos, last-1) --move--> [pos+1, last)
                        std::memmove(posptr + 1, posptr, (last - posptr - 1) * sizeof(value_type));
                    }
                }
                else {
                    // 1. pos -> last
                    new(last)value_type{ std::move(*(last - 1)) };
                    // 2. [pos, last-1) --move--> [pos+1, last)
                    for (auto cursor = last - 2; cursor >= pos; --cursor)
                        *(cursor + 1) = std::move(*cursor);
                }
                
                // 3. destroy
                if constexpr (!std::is_trivially_destructible_v<value_type>)
                    posptr->~value_type();
            }
            // 4. copy ctor at pos
            new(posptr)value_type{ std::forward<Args>(args)... };

            ++m_size;

            return posptr;
        }

        iterator erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
            const pointer posptr = const_cast<pointer>(pos);
            const pointer mylast = end();
            assert(begin() <= pos && pos < mylast);

            std::move(posptr + 1, mylast, posptr);
            if constexpr (!std::is_trivially_destructible_v<value_type>)
                (mylast - 1)->~value_type();
            m_size--;
            return posptr;
        }

        iterator erase(const_iterator first, const_iterator last) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
            const pointer mylast = end();
            assert(begin() <= first && first <= last && last <= mylast);

            const pointer firstptr = const_cast<pointer>(first);
            const pointer lastptr = const_cast<pointer>(last);
            
            const size_type affected_elements = conver_size(static_cast<size_t>(last - first));

            if (affected_elements > 0) {
                const pointer newlast = std::move(lastptr, mylast, firstptr);
                std::destroy(newlast, mylast);
                m_size -= affected_elements;
            }

            return firstptr;
        }

        void push_back(const value_type& value) {
            assert(size() < max_size());
            new(data() + m_size)value_type{ value };
            ++m_size;
        }

        void push_back(T&& value) {
            assert(size() < max_size());
            new(data() + m_size)value_type{ std::move(value) };
            ++m_size;
        }

        template<typename... Args>
        reference emplace_back(Args&&... args) {
            assert(size() < max_size());
            pointer addr = data() + m_size;
            new(addr)value_type{ std::forward<Args>(args)... };
            ++m_size;
            return *addr;
        }

        void pop_back() {
            assert(!empty());
            if constexpr (!std::is_trivially_destructible_v<value_type>)
                back().~value_type();
            --m_size;
        }

        void resize(size_type count) {
            assert(count < max_size());

            if (count > m_size)
                std::uninitialized_default_construct(end(), begin() + count);
            else
                std::destroy(begin() + count, end());

            m_size = count;
        }

        void resize(size_type count, const T& value) {
            assert(count < max_size());

            if (count > m_size)
                std::uninitialized_fill(end(), begin() + count, value);
            else
                std::destroy(begin() + count, end());

            m_size = count;
        }

    private:
        [[noreturn]] void throw_out_of_range() const { throw std::out_of_range("invalid static_vector subscript"); }

        template<typename Iter> requires std::input_iterator<Iter>
        static_vector(Iter first, Iter last, std::input_iterator_tag) {
            for (; first != last; ++first)
                emplace_back(*first);
        }
        template<typename Iter> requires std::input_iterator<Iter>
        static_vector(Iter first, Iter last, std::forward_iterator_tag) {
            auto mylast = std::uninitialized_copy(first, last, begin());
            m_size = conver_size(static_cast<size_t>(mylast - begin()));
        }

        template <class Iter>
        void assign_range(Iter first, Iter last, std::input_iterator_tag) { // assign input range [first, last)
            const pointer myfirst = begin();
            const pointer mylast = end();

            pointer cursor = myfirst;

            for (; first != last && cursor != mylast; ++first, ++cursor)
                *cursor = *first;

            // Trim.
            std::destroy(cursor, mylast);
            m_size = cursor - myfirst;

            // Append.
            for (; first != last; ++first)
                emplace_back(*first);
        }

        template <class Iter>
        void assign_range(Iter first, Iter last, std::forward_iterator_tag) { // assign forward range [first, last)
            const auto newsize = conver_size(static_cast<size_t>(std::distance(first, last)));
            assert(newsize <= N);
            const pointer myfirst = begin();
            const pointer mylast = end();

            if (newsize > m_size) {
                // performance note: traversing [first, _Mid) twice
                const Iter mid = std::next(first, static_cast<difference_type>(m_size));
                std::copy(first, mid, myfirst);
                std::uninitialized_copy(mid, last, mylast);
            }
            else {
                const pointer newlast = myfirst + newsize;
                std::copy(first, last, myfirst);
                std::destroy(newlast, mylast);
            }

            m_size = newsize;
        }

        template<typename Iter>
        void insert_range(const_iterator pos, Iter first, Iter last, std::input_iterator_tag) {
            assert(begin() <= pos && pos <= end());

            if (first == last)
                return; // nothing to do, avoid invalidating iterators

            pointer myfirst = begin();
            const auto whereoff = static_cast<size_type>(pos - myfirst);
            const auto oldsize = m_size;

            for (; first != last; ++first) {
                emplace_back(*first);
                assert(m_size < max_size());
                ++m_size;
            }

            std::rotate(myfirst + whereoff, myfirst + oldsize, end());
        }

        template<typename S>
        static constexpr size_type conver_size(const S& s) noexcept {
            assert(s <= N);
            return static_cast<size_type>(s);
        }

        template<typename Iter>
        void insert_range(const_iterator pos, Iter first, Iter last, std::forward_iterator_tag) {

            // insert forward range [first, last) at pos
            const pointer posptr = const_cast<pointer>(pos);
            const auto count = conver_size(static_cast<size_t>(std::distance(first, last)));

            assert(count + m_size <= N);
            assert(begin() <= pos && pos <= end());

            const pointer oldlast = end();

            // Attempt to provide the strong guarantee for EmplaceConstructible failure.
            // If we encounter copy/move construction/assignment failure, provide the basic guarantee.
            // (For one-at-back, this provides the strong guarantee.)

            const auto affected_elements = static_cast<size_type>(oldlast - posptr);

            if (count < affected_elements) { // some affected elements must be assigned
                if constexpr (std::is_trivially_move_assignable_v<value_type>) {
                    if constexpr (std::is_trivially_move_constructible_v<value_type>)
                        std::memmove(posptr + count, posptr, affected_elements * sizeof(value_type));
                    else {
                        /*mylast = */std::uninitialized_move(oldlast - count, oldlast, oldlast);
                        std::memmove(posptr + count, posptr, (affected_elements - count) * sizeof(value_type));
                    }
                }
                else {
                    /*mylast = */std::uninitialized_move(oldlast - count, oldlast, oldlast);
                    std::move(posptr, oldlast - count, posptr + count);
                    for (auto cursor = oldlast - count - 1; cursor >= posptr; --cursor)
                        *(cursor + count) = std::move(*cursor);
                }
                std::destroy(posptr, posptr + count);
                std::uninitialized_copy(first, last, posptr);
            }
            else { // affected elements don't overlap before/after
                const pointer relocated = posptr + count;
                std::uninitialized_move(posptr, oldlast, relocated);
                std::destroy(posptr, oldlast);

                std::uninitialized_copy(first, last, posptr);
            }

            m_size += count;
        }

        std::aligned_storage_t<sizeof(T)* N, alignof(T)> m_storage;
        size_type m_size;
    };

    template<typename T, std::size_t N>
    bool operator==(const static_vector<T, N>& lhs, const static_vector<T, N>& rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<typename T, std::size_t N>
    bool operator<(const static_vector<T, N>& lhs, const static_vector<T, N>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename T, std::size_t N>
    bool operator!=(const static_vector<T, N>& lhs, const static_vector<T, N>& rhs) {
        return !(lhs == rhs);
    }

    template<typename T, std::size_t N>
    bool operator>(const static_vector<T, N>& lhs, const static_vector<T, N>& rhs) {
        return rhs < lhs;
    }

    template<typename T, std::size_t N>
    bool operator<=(const static_vector<T, N>& lhs, const static_vector<T, N>& rhs) {
        return !(rhs < lhs);
    }

    template<typename T, std::size_t N>
    bool operator>=(const static_vector<T, N>& lhs, const static_vector<T, N>& rhs) {
        return !(lhs < rhs);
    }
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
