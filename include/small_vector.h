#pragma once

#include "fixed_vector.h"

#include <vector>
#include <iterator>
#include <optional>

namespace Ubpa {
    template <typename T, std::size_t N, typename Allocator = std::allocator<T>>
    class small_vector {
        using stack_type = fixed_vector<T, N>;
        using heap_type = std::vector<T, Allocator>;
    public:
        //////////////////
        // Member types //
        //////////////////

        using value_type = T;
        using size_type = std::size_t;
        using reference = typename std::vector<T, Allocator>::reference;
        using const_reference = typename std::vector<T, Allocator>::const_reference;
        using allocator_type = Allocator;
        using pointer = T*;
        using const_pointer = const T*;
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        
        //////////////////////
        // Member functions //
        //////////////////////

        small_vector() noexcept : first_{ stack_.begin() }, last_{ stack_.end() } {};

        small_vector(const allocator_type& alloc) :
            heap_{ std::in_place_t{}, alloc },
            first_{ stack_.begin() },
            last_{ stack_.begin() } {}

        explicit small_vector(size_type count) :
            stack_(static_cast<typename stack_type::size_type>(count <= N ? count : 0))
        {
            if (count > N) {
                heap_ = heap_type(count);
                set_range_to_heap();
            }
            else
                set_range_to_stack();
        }

        small_vector(size_type count, const allocator_type& alloc) :
            stack_(static_cast<typename stack_type::size_type>(count <= N ? 0 : count)),
            heap_{ count <= N ? heap_type{ alloc } : heap_type{ count, alloc } },
            first_{ count <= N ? stack_.begin() : heap_->data() },
            last_{ count <= N ? stack_.end() : heap_->data() + count }{}

        small_vector(size_type count, const value_type& value) :
            stack_(static_cast<typename stack_type::size_type>(count <= N ? count : 0), value)
        {
            if (count > N) {
                heap_ = heap_type(count, value);
                set_range_to_heap();
            }
            else
                set_range_to_stack();
        }

        small_vector(size_type count, const value_type& value, const allocator_type& alloc) :
            stack_(static_cast<typename stack_type::size_type>(count <= N ? 0 : count), value),
            heap_{ count <= N ? heap_type{ alloc } : heap_type{ count, value, alloc } },
            first_{ count <= N ? stack_.begin() : heap_->data() },
            last_{ count <= N ? stack_.end() : heap_->data() + count }{}

        template<typename Iter> requires std::input_iterator<Iter>
        small_vector(Iter first, Iter last) :
            small_vector(first, last, typename std::iterator_traits<Iter>::iterator_category{}) {}

        template<typename Iter> requires std::input_iterator<Iter>
        small_vector(Iter first, Iter last, const allocator_type& alloc) :
            small_vector(first, last, alloc, typename std::iterator_traits<Iter>::iterator_category{}) {}

        small_vector(const small_vector& other) :
            stack_{other.stack_},
            heap_{other.heap_},
            first_{other.is_on_stack()?stack_.begin():heap_->data()},
            last_{ other.is_on_stack() ? stack_.end() : heap_->data() + heap_->size() }{}

        small_vector(const small_vector& other, const allocator_type& alloc) :
            stack_{ other.stack_ },
            heap_{ other.heap_.has_value() ? heap_type{ *other.heap_, alloc } : heap_type{ alloc } },
            first_{ other.is_on_stack() ? stack_.begin() : heap_->data() },
            last_{ other.is_on_stack() ? stack_.end() : heap_->data() + heap_->size() } {}

        small_vector(small_vector&& other) noexcept :
            stack_{ std::move(other.stack_) },
            heap_{ std::move(other.heap_) },
            first_{ other.is_on_stack() ? stack_.begin() : heap_->data() },
            last_{ other.is_on_stack() ? stack_.end() : heap_->data() + heap_->size() }
        {
            other.set_range_to_stack();
        }

        small_vector(small_vector&& other, const allocator_type& alloc) noexcept :
            stack_{ std::move(other.stack_) },
            heap_{ other.heap_.has_value() ? heap_type{ std::move(*other.heap_), alloc } : heap_type{ alloc } },
            first_{ other.is_on_stack() ? stack_.begin() : heap_->data() },
            last_{ other.is_on_stack() ? stack_.end() : heap_->data() + heap_->size() }
        {
            other.set_range_to_stack();
        }

        small_vector(std::initializer_list<T> ilist) :
            stack_{ ilist.size() <= N ? ilist : std::initializer_list<T>{} }
        {
            if (ilist.size() > N) {
                heap_ = heap_type{ ilist };
                set_range_to_heap();
            }
            else
                set_range_to_stack();
        }

        small_vector(std::initializer_list<T> ilist, const allocator_type& alloc) :
            stack_{ ilist.size() <= N ? ilist : std::initializer_list<T>{} },
            heap_{ ilist.size() <= N ? heap_type{ alloc } : heap_type{ ilist } },
            first_{ ilist.size() <= N ? stack_.begin() : heap_->data() },
            last_{ ilist.size() <= N ? stack_.end() : heap_->data() + heap_->size() }{}

        small_vector& operator=(const small_vector& rhs) {
            if (this != &rhs) {
                stack_ = rhs.stack_;
                heap_ = rhs.heap_;
                if (rhs.is_on_stack())
                    set_range_to_stack();
                else
                    set_range_to_heap();
            }
            return *this;
        }

        small_vector& operator=(small_vector&& rhs) noexcept {
            if (this != &rhs) {
                stack_ = std::move(rhs.stack_);
                heap_ = std::move(rhs.heap_);
                if (rhs.is_on_stack())
                    set_range_to_stack();
                else
                    set_range_to_heap();
                rhs.set_range_to_stack();
            }
            return *this;
        }

        small_vector& operator=(std::initializer_list<value_type> rhs) {
            if (rhs.size() <= N) {
                if (heap_.has_value())
                    heap_->clear();

                stack_ = rhs;
                set_range_to_stack();
            }
            else {
                stack_.clear();
                if (heap_.has_value())
                    *heap_ = rhs;
                else
                    heap_.emplace(rhs);
                set_range_to_heap();
            }
            return *this;
        }

        void assign(size_type count, const value_type& value) {
            if (count <= N) {
                if (!is_on_stack())
                    heap_->clear();
                stack_.assign(static_cast<typename stack_type::size_type>(count), value);
                set_range_to_stack();
            }
            else {
                if (is_on_stack())
                    stack_.clear();
                if (heap_.has_value())
                    heap_->assign(count, value);
                else
                    heap_ = heap_type(count, value);
                set_range_to_heap();
            }
        }

        template<class Iter> requires std::input_iterator<Iter>
        void assign(Iter first, Iter last) {
            assign_range(first, last, typename std::iterator_traits<Iter>::iterator_category{});
        }

        void assign(std::initializer_list<T> ilist) {
            assign_range(ilist.begin(), ilist.end(), std::random_access_iterator_tag{});
        }

        allocator_type get_allocator() const noexcept {
            return heap_.has_value() ? heap_->get_allocator() : allocator_type{};
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
            pointer p = first_ + pos;
            assert(p < last_);
            return *p;
        }

        const_reference operator[](size_type pos) const noexcept {
            const_pointer p = first_ + pos;
            assert(p < last_);
            return *p;
        }

        reference front() noexcept { return *begin(); }
        const_reference front() const noexcept { return *begin(); }

        reference back() noexcept {
            assert(!empty());
            return *(end() - 1);
        }

        const_reference back() const noexcept {
            assert(!empty());
            return *(end() - 1);
        }

        pointer data() noexcept { return first_; }
        const_pointer data() const noexcept { return first_; }

        //
        // Iterators
        //////////////

        iterator begin() noexcept { return first_; }
        iterator end() noexcept { return last_; }

        const_iterator begin() const noexcept { return first_; }
        const_iterator end() const noexcept { return last_; }

        const_iterator cbegin() const noexcept { return begin(); }
        const_iterator cend() const noexcept { return end(); }

        reverse_iterator rbegin() noexcept { return end(); }
        reverse_iterator rend() noexcept { return begin(); }

        const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        const_reverse_iterator crend() const noexcept { return rend(); }

        //
        // Capacity
        /////////////

        bool empty() const noexcept { return first_ == last_; }

        size_type size() const noexcept { return last_ - first_; }

        void shrink_to_fit() {
            if (heap_.has_value()) {
                if (is_on_stack())
                    heap_->shrink_to_fit();
                else {
                    heap_->shrink_to_fit();
                    set_range_to_heap();
                }
            }
        }

        void reserve(size_type new_cap) {
            if (is_on_stack()){
                if (!heap_.has_value())
                    heap_ = heap_type{};
                heap_->reserve(N);
            }
            else {
                heap_->reserve(N);
                set_range_to_heap();
            }
        }

        size_type capacity() const noexcept {
            if (is_on_stack())
                return N;
            else
                return heap_->capacity();
        }

        //
        // Modifiers
        //////////////

        void clear() noexcept {
            if (is_on_stack())
                stack_.clear();
            else
                heap_->clear();
            set_range_to_stack();
        }

        iterator insert(const_iterator pos, const value_type& value) {
            return emplace(pos, value);
        }

        iterator insert(const_iterator pos, value_type&& value) {
            return emplace(pos, std::move(value));
        }

        iterator insert(const_iterator pos, size_type count, const value_type& value) {
            assert(first_ <= pos && pos <= last_);
            if (size() + count > N) {
                auto offset = pos - first_;
                if (is_on_stack())
                    move_stack_to_empty_heap();
                heap_->insert(heap_->begin() + offset, count, value);
                set_range_to_heap();
                return first_ + offset;
            }
            else {
                stack_.insert(pos, count, value);
                ++last_;
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
            if (oldsize < N) {
                rst = stack_.emplace(pos, std::forward<Args>(args)...);
                ++last_;
            }
            else {
                assert(first_ <= pos && pos <= last_);
                auto offset = pos - first_;
                if (oldsize == N)
                    move_stack_to_empty_heap();
                heap_->emplace(heap_->begin() + offset, std::forward<Args>(args)...);
                set_range_to_heap();
                rst = first_ + offset;
            }
            return rst;
        }

        iterator erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
            iterator rst;
            if (is_on_stack()) {
                rst = stack_.erase(pos);
                --last_;
            }
            else {
                assert(first_ <= pos && pos < last_);
                auto offset = pos - first_;
                heap_->erase(heap_->begin() + offset);
                if (heap_->size() == N) {
                    re_ctor_stack_from_heap();
                    heap_->clear();
                    set_range_to_stack();
                }
                else
                    set_range_to_heap();

                rst = first_ + offset;
            }
            return rst;
        }

        iterator erase(const_iterator first, const_iterator last) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
            iterator rst;
            if (is_on_stack()) {
                rst = stack_.erase(first, last);
                last_ -= last - first;
            }
            else {
                auto offset = first - first_;
                heap_->erase(heap_->begin() + offset, heap_->begin() + (last - heap_->data()));
                if (heap_->size() <= N) {
                    re_ctor_stack_from_heap();
                    heap_->clear();
                    set_range_to_stack();
                }
                else
                    set_range_to_heap();

                rst = first_ + offset;
            }
            return rst;
        }

        void push_back(const value_type& value) {
            const auto oldsize = size();
            if (oldsize < N) {
                stack_.push_back(value);
                ++last_;
            }
            else {
                if (oldsize == N)
                    move_stack_to_empty_heap();
                heap_->push_back(value);
                set_range_to_heap();
            }
        }

        void push_back(T&& value) {
            const auto oldsize = size();
            if (oldsize < N) {
                stack_.push_back(std::move(value));
                ++last_;
            }
            else {
                if (oldsize == N)
                    move_stack_to_empty_heap();
                heap_->push_back(std::move(value));
                set_range_to_heap();
            }
        }

        template<typename... Args>
        void emplace_back(Args&&... args) {
            const auto oldsize = size();
            if (oldsize < N) {
                stack_.emplace_back(std::forward<Args>(args)...);
                ++last_;
            }
            else {
                if (oldsize == N)
                    move_stack_to_empty_heap();
                heap_->emplace_back(std::forward<Args>(args)...);
                set_range_to_heap();
            }
        }

        void pop_back() {
            if (is_on_stack()) {
                stack_.pop_back();
                --last_;
            }
            else if (size() == N + 1) {
                // move back to stack
                re_ctor_stack_from_heap();
                set_range_to_stack();
                heap_->clear();
            }
            else {
                // currently using heap
                heap_->pop_back();
                --last_;
            }
        }

        void resize(size_type count) {
            if (count <= N) {
                if (size() > N) {
                    stack_.assign(std::make_move_iterator(heap_->begin()), std::make_move_iterator(heap_->end()));
                    heap_->clear();
                    set_range_to_stack();
                }
                else {
                    stack_.resize(static_cast<typename stack_type::size_type>(count));
                    last_ = first_ - count;
                }
            }
            else {
                if (is_on_stack())
                    move_stack_to_empty_heap();
                heap_->resize(count);
                set_range_to_heap();
            }
        }

        void resize(size_type count, const T& value) {
            if (count <= N) {
                if (size() > N) {
                    stack_.assign(std::make_move_iterator(heap_->begin()), std::make_move_iterator(heap_->end()));
                    heap_->clear();
                    set_range_to_stack();
                }
                else {
                    stack_.resize(static_cast<typename stack_type::size_type>(count), value);
                    last_ = first_ - count;
                }
            }
            else {
                if (is_on_stack())
                    move_stack_to_empty_heap();
                heap_->resize(count, value);
                set_range_to_heap();
            }
        }

        void swap(small_vector& other) noexcept {
            bool lhs_on_stack = is_on_stack();
            bool rhs_on_stack = other.is_on_stack();
            std::swap(stack_, other.stack_);
            std::swap(heap_, other.heap_);

            if (rhs_on_stack)
                set_range_to_stack();
            else
                set_range_to_heap();

            if (lhs_on_stack)
                other.set_range_to_stack();
            else
                other.set_range_to_heap();
        };

    private:
        bool is_on_stack() const noexcept { return first_ == stack_.begin(); }
        void set_range_to_stack() noexcept { first_ = stack_.begin(); last_ = stack_.end(); }
        void set_range_to_heap() noexcept { first_ = heap_->data(); last_ = first_ + heap_->size(); }
        [[noreturn]] void throw_out_of_range() const { throw std::out_of_range("invalid small_vector subscript"); }

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

                set_range_to_heap();
            }
            else
                set_range_to_stack();
        }

        template<typename Iter>
        small_vector(Iter first, Iter last, std::input_iterator_tag) { emplace_range(first, last); }

        template<typename Iter>
        small_vector(Iter first, Iter last, std::forward_iterator_tag) {
            const auto newsize = convert_size(std::distance(first, last));
            if (newsize > N) {
                heap_ = heap_type(first, last);
                set_range_to_heap();
            }
            else {
                stack_.assign(first, last);
                set_range_to_stack();
            }
        }

        template<typename Iter>
        small_vector(Iter first, Iter last, const allocator_type& alloc, std::input_iterator_tag) :
            heap_{ std::in_place_t{}, alloc }
        {
            emplace_range(first, last);
        }

        template<typename Iter>
        small_vector(Iter first, Iter last, const allocator_type& alloc, std::forward_iterator_tag) :
            heap_{ std::in_place_t{}, alloc }
        {
            const auto newsize = convert_size(std::distance(first, last));
            if (newsize > N) {
                heap_->assign(first, last);
                set_range_to_heap();
            }
            else {
                stack_.assign(first, last);
                set_range_to_stack();
            }
        }

        template<typename Iter>
        void assign_range(Iter first, Iter last, std::input_iterator_tag) { // assign input range [first, last)
            const pointer stackfirst = stack_.begin();
            const pointer stacklast = stack_.end();

            pointer cursor = stackfirst;
            while (true) {
                if (first == last) {
                    last_ = stack_.erase(cursor, stacklast);
                    break;
                }
                else if (cursor == stacklast) {
                    emplace_range(first, last);
                    break;
                }
                ++first;
                ++cursor;
            }
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

                set_range_to_heap();
            }
            else {
                if (heap_.has_value())
                    heap_->clear();
                stack_.assign(first, last);
                set_range_to_stack();
            }
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
                    set_range_to_heap();
                    std::rotate(heap_->begin() + whereoff, heap_->begin() + oldsize, heap_->end());
                }
                else {
                    last_ = stack_.end();
                    std::rotate(first_ + whereoff, first_ + oldsize, last_);
                }
            }
            else {
                heap_->insert(heap_->begin() + whereoff, first, last);
                set_range_to_heap();
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
                set_range_to_heap();
            }
            else {
                stack_.insert(pos, first, last);
                last_ = stack_.end();
            }
            return first_ + offset;
        }

        void re_ctor_stack_from_heap() {
            assert(stack_.empty() && heap_->size() >= N);
            new(&stack_)stack_type(std::make_move_iterator(first_), std::make_move_iterator(first_ + N));
        }

        void move_stack_to_empty_heap() {
            if (heap_.has_value()) {
                assert(heap_->empty());
                heap_->assign(std::make_move_iterator(stack_.begin()), std::make_move_iterator(stack_.end()));
            }
            else
                heap_ = heap_type{ std::make_move_iterator(stack_.begin()), std::make_move_iterator(stack_.end()) };
            stack_.clear();
        }

        stack_type stack_;
        std::optional<heap_type> heap_;
        pointer first_;
        pointer last_;
    };

    template<typename T, std::size_t N, typename Allocator>
    bool operator==(const small_vector<T, N, Allocator>& lhs, const small_vector<T, N, Allocator>& rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<typename T, std::size_t N, typename Allocator>
    bool operator<(const small_vector<T, N, Allocator>& lhs, const small_vector<T, N, Allocator>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename T, std::size_t N, typename Allocator>
    bool operator!=(const small_vector<T, N, Allocator>& lhs, const small_vector<T, N, Allocator>& rhs) {
        return !(lhs == rhs);
    }

    template<typename T, std::size_t N, typename Allocator>
    bool operator>(const small_vector<T, N, Allocator>& lhs, const small_vector<T, N, Allocator>& rhs) {
        return rhs < lhs;
    }

    template<typename T, std::size_t N, typename Allocator>
    bool operator<=(const small_vector<T, N, Allocator>& lhs, const small_vector<T, N, Allocator>& rhs) {
        return !(rhs < lhs);
    }

    template<typename T, std::size_t N, typename Allocator>
    bool operator>=(const small_vector<T, N, Allocator>& lhs, const small_vector<T, N, Allocator>& rhs) {
        return !(lhs < rhs);
    }
}
