#pragma once

#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <concepts>
#include <type_traits>
#include <cstring>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 26495 )
#endif

namespace Ubpa::details {
    template <std::size_t N>
    using fixed_vector_size_type
        = std::conditional_t<(N < std::numeric_limits<uint8_t>::max()), std::uint8_t,
        std::conditional_t<(N < std::numeric_limits<uint16_t>::max()), std::uint16_t,
        std::conditional_t<(N < std::numeric_limits<uint32_t>::max()), std::uint32_t,
        std::conditional_t<(N < std::numeric_limits<uint64_t>::max()), std::uint64_t,
        std::size_t>>>>;
}

namespace Ubpa {
    template <class T, std::size_t N = 16>
    class fixed_vector {
    public:
        //////////////////
        // Member types //
        //////////////////

        using value_type = T;
        using size_type = details::fixed_vector_size_type<N>;
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

        fixed_vector() noexcept : size_{ 0 } {}

        explicit fixed_vector(size_type count) : size_{ count } {
            assert(count <= N);
            std::uninitialized_value_construct(begin(), end());
        }

        fixed_vector(size_type count, const value_type& value) : size_{ count } {
            assert(count <= N);
            std::uninitialized_fill(begin(), end(), value);
        }

        fixed_vector(const fixed_vector& other) : size_{ other.size_ } {
            std::uninitialized_copy(other.begin(), other.end(), begin());
        }

        fixed_vector(fixed_vector&& other) noexcept : size_{ other.size_ } {
            std::uninitialized_move(other.begin(), other.end(), begin());
            other.clear();
        }

        fixed_vector(std::initializer_list<value_type> ilist) : size_{ static_cast<size_type>(ilist.size()) } {
            assert(ilist.size() <= N);
            std::uninitialized_copy(ilist.begin(), ilist.end(), begin());
        }

        template<typename Iter> requires std::input_iterator<Iter>
        fixed_vector(Iter first, Iter last) : fixed_vector(first, last, typename std::iterator_traits<Iter>::iterator_category{}) {}

        ~fixed_vector() { std::destroy(begin(), end()); }

        fixed_vector& operator=(const fixed_vector& rhs) {
            if (this != &rhs) {
                if (size_ > rhs.size_) {
                    std::destroy(begin() + rhs.size_, end());
                    std::copy(rhs.begin(), rhs.end(), begin());
                }
                else {
                    if constexpr (std::is_trivially_copy_assignable_v<value_type> && std::is_trivially_copy_constructible_v<value_type>) {
                        std::memcpy(&storage_, &rhs.storage_, rhs.size_ * sizeof(value_type));
                    }
                    else {
                        std::copy(rhs.begin(), rhs.begin() + size_, begin());
                        std::uninitialized_copy(rhs.begin() + size_, rhs.end(), end());
                    }
                }
                size_ = rhs.size_;
            }
            return *this;
        }

        fixed_vector& operator=(fixed_vector&& rhs) noexcept {
            if (this != &rhs) {
                if (size_ > rhs.size_) {
                    std::destroy(begin() + rhs.size_, end());
                    std::move(rhs.begin(), rhs.end(), begin());
                }
                else {
                    if constexpr (std::is_trivially_move_assignable_v<value_type> && std::is_trivially_move_constructible_v<value_type>) {
                        std::memcpy(&storage_, &rhs.storage_, rhs.size_ * sizeof(value_type));
                    }
                    else {
                        std::copy(rhs.begin(), rhs.begin() + size_, begin());
                        std::uninitialized_move(rhs.begin() + size_, rhs.end(), end());
                    }
                }
                size_ = rhs.size_;
                rhs.clear();
            }
            return *this;
        }

        fixed_vector& operator=(std::initializer_list<value_type> rhs) {
            assert(rhs.size() <= N);
            const size_type rhs_size = static_cast<size_type>(rhs.size());
            if (size_ > rhs_size) {
                std::destroy(begin() + rhs_size, end());
                std::copy(rhs.begin(), rhs.end(), begin());
            }
            else {
                if constexpr (std::is_trivially_copy_assignable_v<value_type> && std::is_trivially_copy_constructible_v<value_type>) {
                    std::memcpy(&storage_, rhs.begin(), rhs_size * sizeof(value_type));
                }
                else {
                    std::copy(rhs.begin(), rhs.begin() + size_, begin());
                    std::uninitialized_copy(rhs.begin() + size_, rhs.end(), end());
                }
            }
            size_ = rhs_size;
            return *this;
        }

        void assign(size_type count, const value_type& value) {
            assert(count <= N);

            const pointer myfirst = begin();
            const pointer mylast = end();

            if (count > size_) {
                std::fill(myfirst, mylast, value);

                std::uninitialized_fill_n(mylast, count - size_, value);
            }
            else {
                const pointer newlast = myfirst + count;
                std::fill(myfirst, newlast, value);
                std::destroy(newlast, mylast);
            }
            size_ = count;
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
            return *(data() + size_ - 1);
        }

        const_reference back() const noexcept {
            assert(!empty());
            return *(data() + size_ - 1);
        }

        pointer data() noexcept { return reinterpret_cast<pointer>(&storage_); }
        const_pointer data() const noexcept { return reinterpret_cast<const_pointer>(&storage_); }

        //
        // Iterators
        //////////////

        iterator begin() noexcept { return data(); }
        const_iterator begin() const noexcept { return data(); }
        const_iterator cbegin() const noexcept { return begin(); }

        iterator end() noexcept { return data() + size_; }
        const_iterator end() const noexcept { return data() + size_; }
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

        bool empty() const noexcept { return size_ == 0; }

        size_type size() const noexcept { return size_; }

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
            size_ = 0;
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
                // 1. pos -> end
                if (pos != last)
                    std::uninitialized_move(last - count, last, last);

                // 2. [pos, last-count) --move--> [pos+count, last)
                std::move(posptr, last - count, posptr + count);

                // 3. destroy [pos, pos+count)
                std::destroy(posptr, posptr + count);
            }
            else {
                // [pos, last) --move--> [last, ...)
                std::uninitialized_move(posptr, last, last);
                std::destroy(posptr, last);
            }

            // 4. copy ctor at pos
            std::uninitialized_fill(posptr, posptr + count, value);

            size_ += count;

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
                // 1. pos -> end
                new(last)value_type{ *(last - 1) };
                // 2. [pos, last-1) --move--> [pos+1, last)
                std::move(posptr, last - 1, posptr + 1);
                // 3. destroy
                if constexpr (!std::is_trivially_destructible_v<value_type>)
                    posptr->~value_type();
            }
            // 4. copy ctor at pos
            new(posptr)value_type{ std::forward<Args>(args)... };

            ++size_;

            return posptr;
        }

        iterator erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
            const pointer posptr = const_cast<pointer>(pos);
            const pointer mylast = end();
            assert(begin() <= pos && pos < mylast);

            std::move(posptr + 1, mylast, posptr);
            if constexpr (!std::is_trivially_destructible_v<value_type>)
                (mylast - 1)->~value_type();
            size_--;
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
                size_ -= affected_elements;
            }

            return firstptr;
        }

        void push_back(const value_type& value) {
            assert(size() < max_size());
            new(data() + size_)value_type{ value };
            ++size_;
        }

        void push_back(T&& value) {
            assert(size() < max_size());
            new(data() + size_)value_type{ std::move(value) };
            ++size_;
        }

        template<typename... Args>
        reference emplace_back(Args&&... args) {
            assert(size() < max_size());
            pointer addr = data() + size_;
            new(addr)value_type{ std::forward<Args>(args)... };
            ++size_;
            return *addr;
        }

        void pop_back() {
            assert(!empty());
            if constexpr (!std::is_trivially_destructible_v<value_type>)
                back().~value_type();
            --size_;
        }

        void resize(size_type count) {
            assert(count < max_size());

            if (count > size_)
                std::uninitialized_default_construct(end(), begin() + count);
            else
                std::destroy(begin() + count, end());

            size_ = count;
        }

        void resize(size_type count, const T& value) {
            assert(count < max_size());

            if (count > size_)
                std::uninitialized_fill(end(), begin() + count, value);
            else
                std::destroy(begin() + count, end());

            size_ = count;
        }

    private:
        [[noreturn]] void throw_out_of_range() const { throw std::out_of_range("invalid fixed_vector subscript"); }

        template<typename Iter> requires std::input_iterator<Iter>
        fixed_vector(Iter first, Iter last, std::input_iterator_tag) {
            for (; first != last; ++first)
                emplace_back(*first);
        }
        template<typename Iter> requires std::input_iterator<Iter>
        fixed_vector(Iter first, Iter last, std::forward_iterator_tag) {
            auto mylast = std::uninitialized_copy(first, last, begin());
            size_ = conver_size(static_cast<size_t>(mylast - begin()));
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
            size_ = cursor - myfirst;

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

            if (newsize > size_) {
                // performance note: traversing [first, _Mid) twice
                const Iter mid = std::next(first, static_cast<difference_type>(size_));
                std::copy(first, mid, myfirst);
                std::uninitialized_copy(mid, last, mylast);
            }
            else {
                const pointer newlast = myfirst + newsize;
                std::copy(first, last, myfirst);
                std::destroy(newlast, mylast);
            }

            size_ = newsize;
        }

        template<typename Iter>
        void insert_range(const_iterator pos, Iter first, Iter last, std::input_iterator_tag) {
            assert(begin() <= pos && pos <= end());

            if (first == last)
                return; // nothing to do, avoid invalidating iterators

            pointer myfirst = begin();
            const auto whereoff = static_cast<size_type>(pos - myfirst);
            const auto oldsize = size_;

            for (; first != last; ++first) {
                emplace_back(*first);
                assert(size_ < max_size());
                ++size_;
            }

            std::rotate(myfirst + whereoff, myfirst + oldsize, end());
        }

        template<typename T>
        static constexpr size_type conver_size(const T& s) noexcept {
            assert(s <= N);
            return static_cast<size_type>(s);
        }

        template<typename Iter>
        void insert_range(const_iterator pos, Iter first, Iter last, std::forward_iterator_tag) {

            // insert forward range [first, last) at pos
            const pointer posptr = const_cast<pointer>(pos);
            const auto count = conver_size(static_cast<size_t>(std::distance(first, last)));

            assert(count + size_ <= N);
            assert(begin() <= pos && pos <= end());

            const pointer oldlast = end();

            // Attempt to provide the strong guarantee for EmplaceConstructible failure.
            // If we encounter copy/move construction/assignment failure, provide the basic guarantee.
            // (For one-at-back, this provides the strong guarantee.)

            const auto affected_elements = static_cast<size_type>(oldlast - posptr);

            if (count < affected_elements) { // some affected elements must be assigned
                /*mylast = */std::uninitialized_move(oldlast - count, oldlast, oldlast);
                std::move(posptr, oldlast - count, oldlast);
                std::destroy(posptr, posptr + count);
                std::uninitialized_copy(first, last, posptr);
            }
            else { // affected elements don't overlap before/after
                const pointer relocated = posptr + count;
                std::uninitialized_move(posptr, oldlast, relocated);
                std::destroy(posptr, oldlast);

                std::uninitialized_copy(first, last, posptr);
            }

            size_ += count;
        }

        std::aligned_storage_t<sizeof(T)* N, alignof(T)> storage_;
        size_type size_;
    };

    template<typename T, std::size_t N>
    bool operator==(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<typename T, std::size_t N>
    bool operator<(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename T, std::size_t N>
    bool operator!=(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return !(lhs == rhs);
    }

    template<typename T, std::size_t N>
    bool operator>(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return rhs < lhs;
    }

    template<typename T, std::size_t N>
    bool operator<=(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return !(rhs < lhs);
    }

    template<typename T, std::size_t N>
    bool operator>=(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
        return !(lhs < rhs);
    }
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
