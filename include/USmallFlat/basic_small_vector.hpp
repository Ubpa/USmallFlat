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

        basic_small_vector() noexcept : first_{ stack_.begin() }, size_{ stack_.size() } {};

        basic_small_vector(const Vector<T>& storage) : heap_{ storage }, size_{ storage.size() } {
            if (heap_->size() <= N)
                re_ctor_stack_from_heap();
        }

        basic_small_vector(Vector<T>&& storage) noexcept : heap_{ std::move(storage) }, size_{ storage.size() } {
            if (heap_->size() <= N)
                re_ctor_stack_from_heap();
        }

        explicit basic_small_vector(size_type count) :
            stack_(static_cast<typename stack_type::size_type>(count <= N ? count : 0)),
            size{ count }
        {
            if (count > N) {
                heap_ = heap_type(count);
                first_ = heap_->data();
            }
            else
                first_ = stack_.begin();
        }

        basic_small_vector(size_type count, const value_type& value) :
            stack_(static_cast<typename stack_type::size_type>(count <= N ? count : 0), value),
            size_{ count }
        {
            if (count > N) {
                heap_ = heap_type(count, value);
                first_ = heap_->data();
            }
            else
                first_ = stack_.begin();
        }

        template<typename Iter> requires std::input_iterator<Iter>
        basic_small_vector(Iter first, Iter last) :
            basic_small_vector(first, last, typename std::iterator_traits<Iter>::iterator_category{}) {}

        basic_small_vector(const basic_small_vector& other) :
            stack_(other.stack_),
            heap_{other.heap_},
            first_{other.is_on_stack()?stack_.begin():heap_->data()},
            size_{ other.size_ }{}

        basic_small_vector(basic_small_vector&& other) noexcept :
            stack_( std::move(other.stack_) ),
            heap_{ std::move(other.heap_) },
            first_{ other.is_on_stack() ? stack_.begin() : heap_->data() },
            size_{ other.size_ }
        {
            other.first_ = stack_.begin();
        }

        basic_small_vector(std::initializer_list<T> ilist) :
            stack_(ilist.size() <= N ? ilist : std::initializer_list<T>{}),
            size_{ ilist.size() }
        {
            if (ilist.size() > N) {
                heap_ = heap_type(ilist);
                first_ = heap_->data();
            }
            else
                first_ = stack_.begin();
        }

        basic_small_vector& operator=(const basic_small_vector& rhs) {
            if (this != &rhs) {
                stack_ = rhs.stack_;
                heap_ = rhs.heap_;
                if (rhs.is_on_stack())
                    first_ = stack_.begin();
                else
                    first_ = heap_->data();
                size_ = rhs.size_;
            }
            return *this;
        }

        basic_small_vector& operator=(basic_small_vector&& rhs) noexcept {
            if (this != &rhs) {
                stack_ = std::move(rhs.stack_);
                heap_ = std::move(rhs.heap_);
                if (rhs.is_on_stack())
                    first_ = stack_.begin();
                else
                    first_ = heap_->data();
                rhs.first_ = stack_.begin();
                size_ = rhs.size_;
                rhs.size_ = 0;
            }
            return *this;
        }

        basic_small_vector& operator=(std::initializer_list<value_type> rhs) {
            if (rhs.size() <= N) {
                if (heap_.has_value())
                    heap_->clear();

                stack_ = rhs;
                first_ = stack_.begin();
            }
            else {
                stack_.clear();
                if (heap_.has_value())
                    *heap_ = rhs;
                else
                    heap_.emplace(rhs);
                first_ = heap_->data();
                size_ = rhs.size_;
                rhs.size_ = 0;
            }
            return *this;
        }

        void assign(size_type count, const value_type& value) {
            if (count <= N) {
                if (!is_on_stack())
                    heap_->clear();
                stack_.assign(static_cast<typename stack_type::size_type>(count), value);
                first_ = stack_.begin();
            }
            else {
                if (is_on_stack())
                    stack_.clear();
                if (heap_.has_value())
                    heap_->assign(count, value);
                else
                    heap_ = heap_type(count, value);
                first_ = heap_->data();
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
            return first_[pos];
        }

        const_reference at(size_type pos) const {
            if (pos >= size())
                throw_out_of_range();
            return first_[pos];
        }

        reference operator[](size_type pos) noexcept {
            assert(pos < size_);

            pointer p = first_ + pos;
            return *p;
        }

        const_reference operator[](size_type pos) const noexcept {
            assert(pos < size_);

            const_pointer p = first_ + pos;
            return *p;
        }

        pointer data() noexcept { return first_; }
        const_pointer data() const noexcept { return first_; }

        reference front() noexcept { return *first_; }
        const_reference front() const noexcept { return *first_; }

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

        iterator begin() noexcept { return first_; }
        iterator end() noexcept { return first_ + size_; }

        const_iterator begin() const noexcept { return first_; }
        const_iterator end() const noexcept { return first_ + size_; }

        const_iterator cbegin() const noexcept { return begin(); }
        const_iterator cend() const noexcept { return end(); }

        reverse_iterator rbegin() noexcept { return end(); }
        reverse_iterator rend() noexcept { return begin(); }

        const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        const_reverse_iterator crend() const noexcept { return rend(); }

        //
        // Capacity
        /////////////

        bool empty() const noexcept { return size_ == 0; }

        size_type size() const noexcept { return size_; }

        size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }

        void resize(size_type count) {
            if (count <= N) {
                if (size() > N) {
                    stack_.assign(std::make_move_iterator(heap_->begin()), std::make_move_iterator(heap_->end()));
                    heap_->clear();
                    first_ = stack_.begin();
                }
                else {
                    stack_.resize(static_cast<typename stack_type::size_type>(count));
                }
            }
            else {
                if (is_on_stack())
                    move_stack_to_empty_heap();
                heap_->resize(count);
                first_ = heap_->data();
            }
            size_ = count;
        }

        void resize(size_type count, const T& value) {
            if (count <= N) {
                if (size() > N) {
                    stack_.assign(std::make_move_iterator(heap_->begin()), std::make_move_iterator(heap_->end()));
                    heap_->clear();
                    first_ = stack_.begin();
                }
                else {
                    stack_.resize(static_cast<typename stack_type::size_type>(count), value);
                }
            }
            else {
                if (is_on_stack())
                    move_stack_to_empty_heap();
                heap_->resize(count, value);
                first_ = heap_->data();
            }
            size_ = count;
        }

        size_type capacity() const noexcept {
            if (is_on_stack())
                return N;
            else
                return heap_->capacity();
        }

        void reserve(size_type new_cap) {
            if (is_on_stack()){
                if (heap_.has_value())
                    heap_->reserve(new_cap);
                else if (new_cap > N) {
                    heap_ = heap_type();
                    heap_->reserve(new_cap);
                }
            }
            else {
                heap_->reserve(new_cap);
                first_ = heap_->data();
            }
        }

        void shrink_to_fit() {
            if (heap_.has_value()) {
                if (is_on_stack())
                    heap_->shrink_to_fit();
                else {
                    heap_->shrink_to_fit();
                    first_ = heap_->data();
                }
            }
        }

        //
        // Modifiers
        //////////////

        void clear() noexcept {
            if (is_on_stack())
                stack_.clear();
            else
                heap_->clear();
            first_ = stack_.begin();
            size_ = 0;
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
                auto offset = pos - first_;
                if (is_on_stack())
                    move_stack_to_empty_heap();
                heap_->insert(heap_->begin() + offset, count, value);
                first_ = heap_->data();
                size_ = heap_->size();
                return first_ + offset;
            }
            else {
                stack_.insert(pos, count, value);
                size_ = stack_.size();
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
                rst = stack_.emplace(pos, std::forward<Args>(args)...);
            else {
                assert(begin() <= pos && pos <= end());
                auto offset = pos - first_;
                if (oldsize == N)
                    move_stack_to_empty_heap();
                heap_->emplace(heap_->begin() + offset, std::forward<Args>(args)...);
                first_ = heap_->data();
                rst = first_ + offset;
            }
            ++size_;
            return rst;
        }

        iterator erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
            iterator rst;
            if (is_on_stack())
                rst = stack_.erase(pos);
            else {
                assert(begin() <= pos && pos < end());
                auto offset = pos - first_;
                heap_->erase(heap_->begin() + offset);
                if (heap_->size() == N)
                    re_ctor_stack_from_heap();
                else
                    first_ = heap_->data();

                rst = first_ + offset;
            }
            --size_;
            return rst;
        }

        iterator erase(const_iterator first, const_iterator last) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
            iterator rst;
            if (is_on_stack()) {
                rst = stack_.erase(first, last);
                size_ = stack_.size();
            }
            else {
                auto offset = first - first_;
                heap_->erase(heap_->begin() + offset, heap_->begin() + (last - heap_->data()));
                size_ = heap_->size();
                if (heap_->size() <= N)
                    re_ctor_stack_from_heap();
                else
                    first_ = heap_->data();

                rst = first_ + offset;
            }
            return rst;
        }

        void push_back(const value_type& value) {
            const auto oldsize = size();
            if (oldsize < N)
                stack_.push_back(value);
            else {
                if (oldsize == N)
                    move_stack_to_empty_heap();
                heap_->push_back(value);
                first_ = heap_->data();
            }
            ++size_;
        }

        void push_back(T&& value) {
            const auto oldsize = size();
            if (oldsize < N)
                stack_.push_back(std::move(value));
            else {
                if (oldsize == N)
                    move_stack_to_empty_heap();
                heap_->push_back(std::move(value));
                first_ = heap_->data();
            }
            ++size_;
        }

        template<typename... Args>
        void emplace_back(Args&&... args) {
            const auto oldsize = size();
            if (oldsize < N)
                stack_.emplace_back(std::forward<Args>(args)...);
            else {
                if (oldsize == N)
                    move_stack_to_empty_heap();
                heap_->emplace_back(std::forward<Args>(args)...);
                first_ = heap_->data();
            }
            ++size_;
        }

        void pop_back() {
            if (is_on_stack())
                stack_.pop_back();
            else if (size() == N + 1)
                re_ctor_stack_from_heap();
            else
                heap_->pop_back();
            --size_;
        }

        void swap(basic_small_vector& other) noexcept {
            bool lhs_on_stack = is_on_stack();
            bool rhs_on_stack = other.is_on_stack();
            std::swap(stack_, other.stack_);
            std::swap(heap_, other.heap_);

            if (rhs_on_stack)
                first_ = stack_.begin();
            else
                first_ = heap_->data();

            if (lhs_on_stack)
                other.first_ = stack_.begin();
            else
                other.first_ = other.heap_->data();

            std::swap(size_, other.size_);
        };

    private:
        bool is_on_stack() const noexcept { return first_ == stack_.begin(); }
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
            for (; first != last && stack_.size() < N; ++first)
                stack_.emplace_back(*first);

            if (first != last) {
                move_stack_to_empty_heap();

                for (; first != last; ++first)
                    heap_->emplace_back(*first);

                first_ = heap_->data();
                size_ = heap_->size();
            }
            else{
                first_ = stack_.begin();
                size_ = stack_.size();
            }
        }

        template<typename Iter>
        basic_small_vector(Iter first, Iter last, std::input_iterator_tag) { emplace_range(first, last); }

        template<typename Iter>
        basic_small_vector(Iter first, Iter last, std::forward_iterator_tag) {
            size_ = convert_size(std::distance(first, last));
            if (size_ > N) {
                heap_ = heap_type(first, last);
                first_ = heap_->data();
            }
            else {
                stack_.assign(first, last);
                first_ = stack_.begin();
            }
        }

        template<typename Iter>
        void assign_range(Iter first, Iter last, std::input_iterator_tag) { // assign input range [first, last)
            size_type cnt = 0;
            while (first != last) {
                if (cnt < stack_.max_size()) {
                    if (cnt < stack_.size_)
                        stack_[cnt] = *first;
                    else
                        std::construct_at(stack_.data() + cnt, *first);
                }
                else if (cnt == stack_.max_size()) {
                    move_stack_to_empty_heap();
                    heap_->insert(heap_->end(), first, last);
                    size_ = heap_->size();
                    return;
                }

                ++cnt;
                ++first;
            }
            if (cnt < size_)
                stack_.erase(stack_.begin() + cnt, stack_.end());
            size_ = cnt;
        }

        template <class Iter>
        void assign_range(Iter first, Iter last, std::forward_iterator_tag) { // assign forward range [first, last)
            const auto newsize = convert_size(std::distance(first, last));
            
            if (newsize > N) {
                stack_.clear();
                if (heap_.has_value())
                    heap_->assign(first, last);
                else
                    heap_ = heap_type(first, last);

                first_ = heap_->data();
            }
            else {
                if (heap_.has_value())
                    heap_->clear();
                stack_.assign(first, last);
                first_ = stack_.begin();
            }

            size_ = newsize;
        }

        template<typename Iter>
        iterator insert_range(const_iterator pos, Iter first, Iter last, std::input_iterator_tag) {
            assert(begin() <= pos && pos <= end());
            if (first == last)
                return const_cast<iterator>(pos); // nothing to do, avoid invalidating iterators

            const auto whereoff = static_cast<size_type>(pos - first_);
            if (is_on_stack()) {
                const auto oldsize = size();

                for (; first != last && stack_.size() < N; ++first)
                    stack_.emplace_back(*first);

                if (first != last) {
                    move_stack_to_empty_heap();

                    for (; first != last; ++first)
                        heap_->emplace_back(*first);
                    first_ = heap_->data();
                    size_ = heap_->size();
                    std::rotate(heap_->begin() + whereoff, heap_->begin() + oldsize, heap_->end());
                }
                else {
                    size_ = stack_.size();
                    std::rotate(first_ + whereoff, first_ + oldsize, stack_.end());
                }
            }
            else {
                heap_->insert(heap_->begin() + whereoff, first, last);
                first_ = heap_->data();
                size_ = heap_->size();
            }

            return first_ + whereoff;
        }

        template<typename Iter>
        iterator insert_range(const_iterator pos, Iter first, Iter last, std::forward_iterator_tag) {
            const auto count = convert_size(std::distance(first, last));
            auto offset = pos - first_;
            if (size() + count > N) {
                if (is_on_stack()) {
                    assert(stack_.begin() <= pos && pos <= stack_.end());
                    move_stack_to_empty_heap();
                }

                heap_->insert(heap_->begin() + offset, first, last);
                first_ = heap_->data();
                size_ = heap_->size();
            }
            else {
                stack_.insert(pos, first, last);
                size_ = stack_.size();
            }
            return first_ + offset;
        }

        void re_ctor_stack_from_heap() noexcept {
            assert(stack_.empty());
            new(&stack_)stack_type(std::make_move_iterator(first_), std::make_move_iterator(first_ + std::min(heap_->size(), N)));
            heap_->clear();
            first_ = stack_.begin();
        }

        void move_stack_to_empty_heap() {
            if (heap_.has_value()) {
                assert(heap_->empty());
                heap_->assign(std::make_move_iterator(stack_.begin()), std::make_move_iterator(stack_.end()));
            }
            else
                heap_ = heap_type(std::make_move_iterator(stack_.begin()), std::make_move_iterator(stack_.end()));
            stack_.clear();
        }

        stack_type stack_;
        std::optional<heap_type> heap_;
        pointer first_{ nullptr };
        size_type size_{ 0 };
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
