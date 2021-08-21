#pragma once

#include "static_vector.hpp"

#include <vector>
#include <iterator>
#include <optional>
#include <limits>

namespace Ubpa {
    template <template<typename>class Vector, typename T, std::size_t N = 16>
    class basic_small_vector {
        using stack_type = static_vector<T, N>;
        using heap_type = Vector<T>;
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
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        
        //////////////////////
        // Member functions //
        //////////////////////

        basic_small_vector() noexcept : m_first{ m_stack.begin() }, m_size{ m_stack.size() } {};

        basic_small_vector(const Vector<T>& storage) : m_heap{ storage }, m_size{ storage.size() } {
            if (m_heap->size() <= N)
                re_ctor_m_stackfrom_heap();
        }

        basic_small_vector(Vector<T>&& storage) noexcept : m_heap{ std::move(storage) }, m_size{ storage.size() } {
            if (m_heap->size() <= N)
                re_ctor_m_stackfrom_heap();
        }

        explicit basic_small_vector(size_type count) :
            m_stack(static_cast<typename stack_type::size_type>(count <= N ? count : 0)),
            m_size{ count }
        {
            if (count > N) {
                m_heap = heap_type(count);
                m_first = m_heap->data();
            }
            else
                m_first = m_stack.begin();
        }

        basic_small_vector(size_type count, const value_type& value) :
            m_stack(static_cast<typename stack_type::size_type>(count <= N ? count : 0), value),
            m_size{ count }
        {
            if (count > N) {
                m_heap = heap_type(count, value);
                m_first = m_heap->data();
            }
            else
                m_first = m_stack.begin();
        }

        template<typename Iter> requires std::input_iterator<Iter>
        basic_small_vector(Iter first, Iter last) :
            basic_small_vector(first, last, typename std::iterator_traits<Iter>::iterator_category{}) {}

        basic_small_vector(const basic_small_vector& other) :
            m_stack(other.m_stack),
            m_heap{other.m_heap},
            m_first{other.is_on_stack()?m_stack.begin():m_heap->data()},
            m_size{ other.m_size }{}

        basic_small_vector(basic_small_vector&& other) noexcept :
            m_stack( std::move(other.m_stack) ),
            m_heap{ std::move(other.m_heap) },
            m_first{ other.is_on_stack() ? m_stack.begin() : m_heap->data() },
            m_size{ other.m_size }
        {
            other.m_first = m_stack.begin();
        }

        basic_small_vector(std::initializer_list<T> ilist) :
            m_stack(ilist.size() <= N ? ilist : std::initializer_list<T>{}),
            m_size{ ilist.size() }
        {
            if (ilist.size() > N) {
                m_heap = heap_type(ilist);
                m_first = m_heap->data();
            }
            else
                m_first = m_stack.begin();
        }

        basic_small_vector& operator=(const basic_small_vector& rhs) {
            if (this != &rhs) {
                m_stack = rhs.m_stack;
                m_heap = rhs.m_heap;
                if (rhs.is_on_stack())
                    m_first = m_stack.begin();
                else
                    m_first = m_heap->data();
                m_size = rhs.m_size;
            }
            return *this;
        }

        basic_small_vector& operator=(basic_small_vector&& rhs) noexcept {
            if (this != &rhs) {
                m_stack = std::move(rhs.m_stack);
                m_heap = std::move(rhs.m_heap);
                if (rhs.is_on_stack())
                    m_first = m_stack.begin();
                else
                    m_first = m_heap->data();
                rhs.m_first = m_stack.begin();
                m_size = rhs.m_size;
                rhs.m_size = 0;
            }
            return *this;
        }

        basic_small_vector& operator=(std::initializer_list<value_type> rhs) {
            if (rhs.size() <= N) {
                if (m_heap.has_value())
                    m_heap->clear();

                m_stack = rhs;
                m_first = m_stack.begin();
            }
            else {
                m_stack.clear();
                if (m_heap.has_value())
                    *m_heap = rhs;
                else
                    m_heap.emplace(rhs);
                m_first = m_heap->data();
                m_size = rhs.m_size;
                rhs.m_size = 0;
            }
            return *this;
        }

        void assign(size_type count, const value_type& value) {
            if (count <= N) {
                if (!is_on_stack())
                    m_heap->clear();
                m_stack.assign(static_cast<typename stack_type::size_type>(count), value);
                m_first = m_stack.begin();
            }
            else {
                if (is_on_stack())
                    m_stack.clear();
                if (m_heap.has_value())
                    m_heap->assign(count, value);
                else
                    m_heap = heap_type(count, value);
                m_first = m_heap->data();
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
            return m_first[pos];
        }

        const_reference at(size_type pos) const {
            if (pos >= size())
                throw_out_of_range();
            return m_first[pos];
        }

        reference operator[](size_type pos) noexcept {
            assert(pos < m_size);

            pointer p = m_first + pos;
            return *p;
        }

        const_reference operator[](size_type pos) const noexcept {
            assert(pos < m_size);

            const_pointer p = m_first + pos;
            return *p;
        }

        pointer data() noexcept { return m_first; }
        const_pointer data() const noexcept { return m_first; }

        reference front() noexcept { return *m_first; }
        const_reference front() const noexcept { return *m_first; }

        reference back() noexcept {
            assert(!empty());
            return *(end() - 1);
        }

        const_reference back() const noexcept {
            assert(!empty());
            return *(end() - 1);
        }

        //
        // Iterators
        //////////////

        iterator begin() noexcept { return m_first; }
        iterator end() noexcept { return m_first + m_size; }

        const_iterator begin() const noexcept { return m_first; }
        const_iterator end() const noexcept { return m_first + m_size; }

        const_iterator cbegin() const noexcept { return begin(); }
        const_iterator cend() const noexcept { return end(); }

        reverse_iterator rbegin() noexcept { return end(); }
        reverse_iterator rend() noexcept { return begin(); }

        const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        const_reverse_iterator crend() const noexcept { return rend(); }

        //
        // Capacity
        /////////////

        bool empty() const noexcept { return m_size == 0; }

        size_type size() const noexcept { return m_size; }

        size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }

        void resize(size_type count) {
            if (count <= N) {
                if (size() > N) {
                    m_stack.assign(std::make_move_iterator(m_heap->begin()), std::make_move_iterator(m_heap->end()));
                    m_heap->clear();
                    m_first = m_stack.begin();
                }
                else {
                    m_stack.resize(static_cast<typename stack_type::size_type>(count));
                }
            }
            else {
                if (is_on_stack())
                    move_stack_to_empty_heap();
                m_heap->resize(count);
                m_first = m_heap->data();
            }
            m_size = count;
        }

        void resize(size_type count, const T& value) {
            if (count <= N) {
                if (size() > N) {
                    m_stack.assign(std::make_move_iterator(m_heap->begin()), std::make_move_iterator(m_heap->end()));
                    m_heap->clear();
                    m_first = m_stack.begin();
                }
                else {
                    m_stack.resize(static_cast<typename stack_type::size_type>(count), value);
                }
            }
            else {
                if (is_on_stack())
                    move_stack_to_empty_heap();
                m_heap->resize(count, value);
                m_first = m_heap->data();
            }
            m_size = count;
        }

        size_type capacity() const noexcept {
            if (is_on_stack())
                return N;
            else
                return m_heap->capacity();
        }

        void reserve(size_type new_cap) {
            if (is_on_stack()){
                if (m_heap.has_value())
                    m_heap->reserve(new_cap);
                else if (new_cap > N) {
                    m_heap = heap_type();
                    m_heap->reserve(new_cap);
                }
            }
            else {
                m_heap->reserve(new_cap);
                m_first = m_heap->data();
            }
        }

        void shrink_to_fit() {
            if (m_heap.has_value()) {
                if (is_on_stack())
                    m_heap->shrink_to_fit();
                else {
                    m_heap->shrink_to_fit();
                    m_first = m_heap->data();
                }
            }
        }

        //
        // Modifiers
        //////////////

        void clear() noexcept {
            if (is_on_stack())
                m_stack.clear();
            else
                m_heap->clear();
            m_first = m_stack.begin();
            m_size = 0;
        }

        iterator insert(const_iterator pos, const value_type& value) {
            return emplace(pos, value);
        }

        iterator insert(const_iterator pos, value_type&& value) {
            return emplace(pos, std::move(value));
        }

        iterator insert(const_iterator pos, size_type count, const value_type& value) {
            assert(begin() <= pos && pos <= end());
            if (size() + count > N) {
                auto offset = pos - m_first;
                if (is_on_stack())
                    move_stack_to_empty_heap();
                m_heap->insert(m_heap->begin() + offset, count, value);
                m_first = m_heap->data();
                m_size = m_heap->size();
                return m_first + offset;
            }
            else {
                m_stack.insert(pos, count, value);
                m_size = m_stack.size();
                return const_cast<iterator>(pos);
            }
        }
        
        template<typename Iter> requires std::input_iterator<Iter>
        iterator insert(const_iterator pos, Iter first, Iter last) {
            return insert_range(pos, first, last, typename std::iterator_traits<Iter>::iterator_category{});
        }

        iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) {
            return insert(pos, ilist.begin(), ilist.end());
        }

        template<typename... Args>
        iterator emplace(const_iterator pos, Args&&... args) {
            iterator rst;
            const auto oldsize = size();
            if (oldsize < N)
                rst = m_stack.emplace(pos, std::forward<Args>(args)...);
            else {
                assert(begin() <= pos && pos <= end());
                auto offset = pos - m_first;
                if (oldsize == N)
                    move_stack_to_empty_heap();
                m_heap->emplace(m_heap->begin() + offset, std::forward<Args>(args)...);
                m_first = m_heap->data();
                rst = m_first + offset;
            }
            ++m_size;
            return rst;
        }

        iterator erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
            iterator rst;
            if (is_on_stack())
                rst = m_stack.erase(pos);
            else {
                assert(begin() <= pos && pos < end());
                auto offset = pos - m_first;
                m_heap->erase(m_heap->begin() + offset);
                if (m_heap->size() == N)
                    re_ctor_m_stackfrom_heap();
                else
                    m_first = m_heap->data();

                rst = m_first + offset;
            }
            --m_size;
            return rst;
        }

        iterator erase(const_iterator first, const_iterator last) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
            iterator rst;
            if (is_on_stack()) {
                rst = m_stack.erase(first, last);
                m_size = m_stack.size();
            }
            else {
                auto offset = first - m_first;
                m_heap->erase(m_heap->begin() + offset, m_heap->begin() + (last - m_heap->data()));
                m_size = m_heap->size();
                if (m_heap->size() <= N)
                    re_ctor_m_stackfrom_heap();
                else
                    m_first = m_heap->data();

                rst = m_first + offset;
            }
            return rst;
        }

        void push_back(const value_type& value) {
            const auto oldsize = size();
            if (oldsize < N)
                m_stack.push_back(value);
            else {
                if (oldsize == N)
                    move_stack_to_empty_heap();
                m_heap->push_back(value);
                m_first = m_heap->data();
            }
            ++m_size;
        }

        void push_back(T&& value) {
            const auto oldsize = size();
            if (oldsize < N)
                m_stack.push_back(std::move(value));
            else {
                if (oldsize == N)
                    move_stack_to_empty_heap();
                m_heap->push_back(std::move(value));
                m_first = m_heap->data();
            }
            ++m_size;
        }

        template<typename... Args>
        void emplace_back(Args&&... args) {
            const auto oldsize = size();
            if (oldsize < N)
                m_stack.emplace_back(std::forward<Args>(args)...);
            else {
                if (oldsize == N)
                    move_stack_to_empty_heap();
                m_heap->emplace_back(std::forward<Args>(args)...);
                m_first = m_heap->data();
            }
            ++m_size;
        }

        void pop_back() {
            if (is_on_stack())
                m_stack.pop_back();
            else if (size() == N + 1)
                re_ctor_m_stackfrom_heap();
            else
                m_heap->pop_back();
            --m_size;
        }

        void swap(basic_small_vector& other) noexcept {
            bool lhs_on_stack = is_on_stack();
            bool rhs_on_stack = other.is_on_stack();
            std::swap(m_stack, other.m_stack);
            std::swap(m_heap, other.m_heap);

            if (rhs_on_stack)
                m_first = m_stack.begin();
            else
                m_first = m_heap->data();

            if (lhs_on_stack)
                other.m_first = m_stack.begin();
            else
                other.m_first = other.m_heap->data();

            std::swap(m_size, other.m_size);
        };

    private:
        bool is_on_stack() const noexcept { return m_first == m_stack.begin(); }
        [[noreturn]] void throw_out_of_range() const { throw std::out_of_range("invalid basic_small_vector subscript"); }

        template<typename U>
        static size_type convert_size(const U& s) noexcept {
            if constexpr (std::is_signed_v<U>)
                assert(s >= 0);
            assert(static_cast<std::size_t>(s) <= std::numeric_limits<size_type>::max());
            return static_cast<size_type>(s);
        }

        template<typename Iter>
        void emplace_range(Iter first, Iter last) {
            for (; first != last && m_stack.size() < N; ++first)
                m_stack.emplace_back(*first);

            if (first != last) {
                move_stack_to_empty_heap();

                for (; first != last; ++first)
                    m_heap->emplace_back(*first);

                m_first = m_heap->data();
                m_size = m_heap->size();
            }
            else{
                m_first = m_stack.begin();
                m_size = m_stack.size();
            }
        }

        template<typename Iter>
        basic_small_vector(Iter first, Iter last, std::input_iterator_tag) { emplace_range(first, last); }

        template<typename Iter>
        basic_small_vector(Iter first, Iter last, std::forward_iterator_tag) {
            m_size = convert_size(std::distance(first, last));
            if (m_size > N) {
                m_heap = heap_type(first, last);
                m_first = m_heap->data();
            }
            else {
                m_stack.assign(first, last);
                m_first = m_stack.begin();
            }
        }

        template<typename Iter>
        void assign_range(Iter first, Iter last, std::input_iterator_tag) { // assign input range [first, last)
            size_type cnt = 0;
            while (first != last) {
                if (cnt < m_stack.max_size()) {
                    if (cnt < m_stack.m_size)
                        m_stack[cnt] = *first;
                    else
                        std::construct_at(m_stack.data() + cnt, *first);
                }
                else if (cnt == m_stack.max_size()) {
                    move_stack_to_empty_heap();
                    m_heap->insert(m_heap->end(), first, last);
                    m_size = m_heap->size();
                    return;
                }

                ++cnt;
                ++first;
            }
            if (cnt < m_size)
                m_stack.erase(m_stack.begin() + cnt, m_stack.end());
            m_size = cnt;
        }

        template <class Iter>
        void assign_range(Iter first, Iter last, std::forward_iterator_tag) { // assign forward range [first, last)
            const auto newsize = convert_size(std::distance(first, last));
            
            if (newsize > N) {
                m_stack.clear();
                if (m_heap.has_value())
                    m_heap->assign(first, last);
                else
                    m_heap = heap_type(first, last);

                m_first = m_heap->data();
            }
            else {
                if (m_heap.has_value())
                    m_heap->clear();
                m_stack.assign(first, last);
                m_first = m_stack.begin();
            }

            m_size = newsize;
        }

        template<typename Iter>
        iterator insert_range(const_iterator pos, Iter first, Iter last, std::input_iterator_tag) {
            assert(begin() <= pos && pos <= end());
            if (first == last)
                return const_cast<iterator>(pos); // nothing to do, avoid invalidating iterators

            const auto whereoff = static_cast<size_type>(pos - m_first);
            if (is_on_stack()) {
                const auto oldsize = size();

                for (; first != last && m_stack.size() < N; ++first)
                    m_stack.emplace_back(*first);

                if (first != last) {
                    move_stack_to_empty_heap();

                    for (; first != last; ++first)
                        m_heap->emplace_back(*first);
                    m_first = m_heap->data();
                    m_size = m_heap->size();
                    std::rotate(m_heap->begin() + whereoff, m_heap->begin() + oldsize, m_heap->end());
                }
                else {
                    m_size = m_stack.size();
                    std::rotate(m_first + whereoff, m_first + oldsize, m_stack.end());
                }
            }
            else {
                m_heap->insert(m_heap->begin() + whereoff, first, last);
                m_first = m_heap->data();
                m_size = m_heap->size();
            }

            return m_first + whereoff;
        }

        template<typename Iter>
        iterator insert_range(const_iterator pos, Iter first, Iter last, std::forward_iterator_tag) {
            const auto count = convert_size(std::distance(first, last));
            auto offset = pos - m_first;
            if (size() + count > N) {
                if (is_on_stack()) {
                    assert(m_stack.begin() <= pos && pos <= m_stack.end());
                    move_stack_to_empty_heap();
                }

                m_heap->insert(m_heap->begin() + offset, first, last);
                m_first = m_heap->data();
                m_size = m_heap->size();
            }
            else {
                m_stack.insert(pos, first, last);
                m_size = m_stack.size();
            }
            return m_first + offset;
        }

        void re_ctor_m_stackfrom_heap() noexcept {
            assert(m_stack.empty());
            new(&m_stack)stack_type(std::make_move_iterator(m_first), std::make_move_iterator(m_first + std::min(m_heap->size(), N)));
            m_heap->clear();
            m_first = m_stack.begin();
        }

        void move_stack_to_empty_heap() {
            if (m_heap.has_value()) {
                assert(m_heap->empty());
                m_heap->assign(std::make_move_iterator(m_stack.begin()), std::make_move_iterator(m_stack.end()));
            }
            else
                m_heap = heap_type(std::make_move_iterator(m_stack.begin()), std::make_move_iterator(m_stack.end()));
            m_stack.clear();
        }

        stack_type m_stack;
        std::optional<heap_type> m_heap;
        pointer m_first{ nullptr };
        size_type m_size{ 0 };
    };

    template<template<typename>class Vector, typename T, std::size_t N>
    bool operator==(const basic_small_vector<Vector, T, N>& lhs, const basic_small_vector<Vector, T, N>& rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<template<typename>class Vector, typename T, std::size_t N>
    bool operator<(const basic_small_vector<Vector, T, N>& lhs, const basic_small_vector<Vector, T, N>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<template<typename>class Vector, typename T, std::size_t N>
    bool operator!=(const basic_small_vector<Vector, T, N>& lhs, const basic_small_vector<Vector, T, N>& rhs) {
        return !(lhs == rhs);
    }

    template<template<typename>class Vector, typename T, std::size_t N>
    bool operator>(const basic_small_vector<Vector, T, N>& lhs, const basic_small_vector<Vector, T, N>& rhs) {
        return rhs < lhs;
    }

    template<template<typename>class Vector, typename T, std::size_t N>
    bool operator<=(const basic_small_vector<Vector, T, N>& lhs, const basic_small_vector<Vector, T, N>& rhs) {
        return !(rhs < lhs);
    }

    template<template<typename>class Vector, typename T, std::size_t N>
    bool operator>=(const basic_small_vector<Vector, T, N>& lhs, const basic_small_vector<Vector, T, N>& rhs) {
        return !(lhs < rhs);
    }
}
