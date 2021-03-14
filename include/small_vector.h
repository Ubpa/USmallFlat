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
        // TODO : improve iterator
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        
        //////////////////////
        // Member functions //
        //////////////////////

        small_vector() noexcept : size_{ 0 } {};

        explicit small_vector(size_type count) :
            stack_(static_cast<typename stack_type::size_type>(count <= N ? count : 0)),
            size_{ count }
        {
            if (count > N)
                heap_ = heap_type(count);
        }

        small_vector(size_type count, const value_type& value) :
            stack_(static_cast<typename stack_type::size_type>(count <= N ? count : 0), value),
            size_{ count }
        {
            if (count > N)
                heap_ = heap_type(count, value);
        }

        small_vector(size_type count,
            const value_type& value,
            const allocator_type& alloc) :
            stack_(static_cast<typename stack_type::size_type>(count <= N ? 0 : count), value),
            heap_{ count <= N ? heap_type{ alloc } : heap_type{ count, value, alloc } },
            size_{ count } {}

        small_vector(const small_vector& other) = default;

        small_vector(const small_vector& other, const allocator_type& alloc) :
            stack_{ other.stack_ },
            heap_{ other.heap_.has_value() ? heap_type{ *other.heap_, alloc } : heap_type{ alloc } },
            size_{ other.size_ } {}

        small_vector(small_vector&& other) noexcept = default;

        small_vector(small_vector&& other, const allocator_type& alloc) noexcept :
            stack_{ other.stack_ },
            heap_{ other.heap_.has_value() ? heap_type{ std::move(*other.heap_), alloc } : heap_type{ alloc } },
            size_{ other.size_ } {}

        small_vector(std::initializer_list<T> ilist) :
            stack_{ ilist.size() <= N ? ilist : std::initializer_list<T>{} },
            size_{ ilist.size() }
        {
            if (ilist.size() > N)
                heap_ = heap_type{ ilist };
        }

        small_vector(std::initializer_list<T> ilist, const allocator_type& alloc) :
            stack_{ ilist.size() <= N ? ilist : std::initializer_list<T>{} },
            heap_{ ilist.size() <= N ? heap_type{ alloc } : heap_type{ ilist } },
            size_{ ilist.size() }
        {}

        small_vector& operator=(const small_vector& rhs) = default;

        small_vector& operator=(small_vector&& rhs) noexcept = default;

        small_vector& operator=(std::initializer_list<value_type> rhs) {
            if (rhs.size() <= N) {
                if (heap_.has_value())
                    heap_->clear();

                stack_ = rhs;
            }
            else {
                stack_.clear();
                if (heap_.has_value())
                    *heap_ = rhs;
                else
                    heap_.emplace(rhs);
            }
        }

        void assign(size_type count, const value_type& value) {
            if (count <= N) {
                if (size_ > N)
                    heap_->clear();
                stack_.assign(static_cast<typename stack_type::size_type>(count), value);
            }
            else {
                if (size_ <= N)
                    stack_.clear();
                if (!heap_.has_value())
                    heap_ = heap_type(count, value);
                else
                    heap_->assign(count, value);
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

        allocator_type get_allocator() const noexcept {
            return heap_.has_value() ? heap_->get_allocator() : allocator_type{};
        }

        //
        // Element access
        ///////////////////

        reference at(size_type pos) {
            if (size_ <= N)
                return stack_.at(static_cast<typename stack_type::size_type>(pos));
            else
                return heap_->at(pos);
        }

        const_reference at(size_type pos) const {
            if (size_ <= N)
                return stack_.at(static_cast<typename stack_type::size_type>(pos));
            else
                return heap_->at(pos);
        }

        reference operator[](size_type pos) noexcept {
            if (size_ <= N)
                return stack_[static_cast<typename stack_type::size_type>(pos)];
            else
                return (*heap_)[pos];
        }

        const_reference operator[](size_type pos) const noexcept {
            if (size_ <= N)
                return stack_[static_cast<typename stack_type::size_type>(pos)];
            else
                return (*heap_)[pos];
        }

        reference front() noexcept {
            if (size_ <= N)
                return stack_.front();
            else
                return heap_->front();
        }

        const_reference front() const noexcept {
            if (size_ <= N)
                return stack_.front();
            else
                return heap_->front();
        }

        reference back() noexcept {
            if (size_ <= N)
                return stack_.back();
            else
                return heap_->back();
        }

        const_reference back() const noexcept {
            if (size_ <= N)
                return stack_.back();
            else
                return heap_->back();
        }

        pointer data() noexcept {
            if (size_ <= N)
                return stack_.data();
            else
                return heap_->data();
        }

        const_pointer data() const noexcept {
            if (size_ <= N)
                return stack_.data();
            else
                return heap_->data();
        }

        //
        // Iterators
        //////////////

        iterator begin() noexcept {
            if (size_ <= N)
                return stack_.data();
            else
                return heap_->data();
        }

        iterator end() noexcept {
            if (size_ <= N)
                return stack_.data() + size_;
            else
                return heap_->data() + size_;
        }

        const_iterator begin() const noexcept {
            if (size_ <= N)
                return stack_.data();
            else
                return heap_->data();
        }

        const_iterator end() const noexcept {
            if (size_ <= N)
                return stack_.data() + size_;
            else
                return heap_->data() + size_;
        }

        const_iterator cbegin() const noexcept { return begin(); }
        const_iterator cend() const noexcept { return end(); }

        reverse_iterator rbegin() noexcept {
            if (size_ <= N)
                return reverse_iterator(stack_.data() + size_);
            else
                return reverse_iterator(heap_->data() + size_);
        }

        reverse_iterator rend() noexcept {
            if (size_ <= N)
                return reverse_iterator(stack_.data());
            else
                return reverse_iterator(heap_->data());
        }

        const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        const_reverse_iterator crend() const noexcept { return rend(); }

        //
        // Capacity
        /////////////

        bool empty() const noexcept { return size_ == 0; }

        size_type size() const noexcept { return size_; }

        void shrink_to_fit() {
            if (heap_.has_value())
                heap_->shrink_to_fit();
        }

        void reserve(size_type new_cap) {
            if (!heap_.has_value())
                heap_ = heap_type{};
            heap_->reserve(N);
        }

        size_type capacity() const noexcept {
            if (size_ <= N)
                return N;
            else
                return heap_->capacity();
        }

        //
        // Modifiers
        //////////////

        void clear() noexcept {
            if (size_ <= N)
                stack_.clear();
            else
                heap_->clear();
            size_ = 0;
        }

        iterator insert(const_iterator pos, const value_type& value) {
            return emplace(pos, value);
        }

        iterator insert(const_iterator pos, value_type&& value) {
            return emplace(pos, std::move(value));
        }

        iterator insert(const_iterator pos, size_type count, const value_type& value) {
            const pointer myfirst = begin();
            const pointer mylast = end();
            assert(myfirst <= pos && pos <= mylast);
            if (size_ + count > N) {
                if (size_ <= N) {
                    if (!heap_.has_value())
                        heap_ = heap_type{};
                    std::move(stack_.begin(), stack_.end(), std::back_inserter(*heap_));
                    auto offset = pos - myfirst;
                    return heap_->insert(heap_->begin() + offset, count, value);
                }
                else {
                    auto offset = pos - heap_->data();
                    return heap_->insert(heap_->begin() + offset, count, value);
                }
            }
            else
                return stack_.insert(pos, count, value);
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
            if (size_ < N)
                rst = stack_.emplace(pos, std::forward<Args>(args)...);
            else if (size_ == N) {
                assert(stack_.begin() <= pos && pos <= stack_.begin() + N);
                auto offset = pos - stack_.begin();
                move_stack_to_empty_heap();
                heap_->emplace(heap_->begin() + offset, std::forward<Args>(args)...);
                rst = heap_->data() + offset;
            }
            else {
                auto offset = pos - heap_->data();
                heap_->emplace(heap_->begin() + offset, std::forward<Args>(args)...);
                rst = heap_->data() + offset;
            }

            ++size_;
            return rst;
        }

        iterator erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
            iterator rst;
            if (size_ <= N)
                rst = stack_.erase(pos);
            else {
                auto iter = heap_->erase(pos);
                auto offset = std::distance(iter, heap_->begin());
                if (size_ == N + 1) {
                    re_ctor_stack_from_heap();
                    heap_->clear();
                    rst = stack_.begin() + offset;
                }
                else
                    rst = heap_->data() + offset;
            }

            --size_;
            return rst;
        }

        iterator erase(const_iterator first, const_iterator last) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
            iterator rst;
            if (size_ <= N) {
                rst = stack_.erase(first, last);
                size_ = stack_.size();
            }
            else {
                auto offset = first - heap_->data();
                auto iter = heap_->erase(heap_->begin() + offset, heap_->begin() + (last - heap_->data()));
                if (heap_->size() <= N) {
                    std::move(heap_->begin(), heap_->end(), std::back_inserter(stack_));
                    heap_->clear();
                    rst = stack_.begin() + offset;
                    size_ = stack_.size();
                }
                else {
                    rst = heap_->data() - offset;
                    size_ = heap_->size();
                }
            }
            return rst;
        }

        void push_back(const value_type& value) {
            if (size_ < N)
                stack_.push_back(value);
            else {
                if (size_ == N)
                    move_stack_to_empty_heap();
                heap_->push_back(value);
            }
            size_++;
        }

        void push_back(T&& value) {
            if (size_ < N)
                stack_.push_back(std::move(value));
            else {
                if (size_ == N)
                    move_stack_to_empty_heap();
                heap_->push_back(std::move(value));
            }
            size_++;
        }

        template<typename... Args>
        void emplace_back(Args&&... args) {
            if (size_ < N)
                stack_.emplace_back(std::forward<Args>(args)...);
            else {
                if (size_ == N)
                    move_stack_to_empty_heap();
                heap_->emplace_back(std::forward<Args>(args)...);
            }
            size_++;
        }

        void pop_back() {
            if (size_ <= N)
                stack_.pop_back();
            else if (size_ == N + 1) {
                // move back to stack
                re_ctor_stack_from_heap();
                heap_->clear();
            }
            else {
                // currently using heap
                heap_->pop_back();
            }
            size_--;
        }

        void resize(size_type count) {
            if (count <= N) {
                if (size_ > N) {
                    stack_.assign(std::make_move_iterator(heap_->begin()), std::make_move_iterator(heap_->end()));
                    heap_->clear();
                }
                else
                    stack_.resize(static_cast<typename stack_type::size_type>(count));
            }
            else {
                if (size_ <= N)
                    move_stack_to_empty_heap();
                heap_->resize(count);
            }
            size_ = count;
        }

        void resize(size_type count, const T& value) {
            if (count <= N) {
                if (size_ > N) {
                    stack_.assign(std::make_move_iterator(heap_->begin()), std::make_move_iterator(heap_->end()));
                    heap_->clear();
                }
                else
                    stack_.resize(static_cast<typename stack_type::size_type>(count), value);
            }
            else {
                if (size_ <= N)
                    move_stack_to_empty_heap();
                heap_->resize(count, value);
            }
            size_ = count;
        }

        void swap(small_vector& other) noexcept {
            std::swap(stack_, other.stack_);
            std::swap(heap_, other.heap_);
            std::swap(size_, other.size_);
        };

    private:
        template <class Iter>
        void assign_range(Iter first, Iter last, std::input_iterator_tag) { // assign input range [first, last)
            const pointer stackfirst = stack_.begin();
            const pointer stacklast = stack_.end();

            pointer cursor = stackfirst;

            for (; first != last && cursor != stacklast; ++first, ++cursor)
                *cursor = *first;

            stack_.erase(cursor, stacklast);

            for (; first != last && stack_.size() < N; ++first)
                stack_.emplace_back(*first);

            if (first != last) {
                move_stack_to_empty_heap();

                for (; first != last; ++first)
                    heap_->emplace_back(*first);

                size_ = heap_->size();
            }
            else
                size_ = stack_.size();
        }

        template <class Iter>
        void assign_range(Iter first, Iter last, std::forward_iterator_tag) { // assign forward range [first, last)
            const auto newsize = conver_size(static_cast<size_t>(std::distance(first, last)));
            
            if (newsize > N) {
                stack_.clear();
                if (heap_.has_value())
                    heap_->assign(first, last);
                else
                    heap_ = heap_type(first, last);

                size_ = heap_->size();
            }
            else {
                if (heap_.has_value())
                    heap_->clear();
                stack_.assign(first, last);
                size_ = stack_.size();
            }
        }

        template<typename Iter>
        iterator insert_range(const_iterator pos, Iter first, Iter last, std::input_iterator_tag) {
            assert(begin() <= pos && pos <= end());
            if (first == last)
                return const_cast<iterator>(pos); // nothing to do, avoid invalidating iterators

            iterator rst;
            if (size_ <= N) {
                pointer mystackfirst = stack_.begin();
                const auto whereoff = static_cast<size_type>(pos - mystackfirst);
                const auto oldsize = size_;

                for (; first != last && size_ < N; ++first) {
                    stack_.emplace_back(*first);
                    ++size_;
                }

                if (first != last) {
                    move_stack_to_empty_heap();

                    for (; first != last; ++first) {
                        heap_->emplace_back(*first);
                        ++size_;
                    }
                    rst = heap_->begin() + whereoff;
                    std::rotate(rst, heap_->begin() + oldsize, heap_->end());
                }
                else {
                    rst = mystackfirst + whereoff;
                    std::rotate(rst, mystackfirst + oldsize, stack_.end());
                }
            }
            else
                rst = heap_->insert(pos, first, last);

            return rst;
        }

        template<typename T>
        static constexpr size_type conver_size(const T& s) noexcept {
            assert(s <= N);
            return static_cast<size_type>(s);
        }

        template<typename Iter>
        iterator insert_range(const_iterator pos, Iter first, Iter last, std::forward_iterator_tag) {
            const auto count = conver_size(static_cast<size_t>(std::distance(first, last)));
            iterator rst;
            if (size_ + count > N) {
                if (size_ <= N) {
                    assert(stack_.begin() <= pos && pos <= stack_.end());
                    auto offset = pos - stack_.begin();
                    move_stack_to_empty_heap();
                    heap_->insert(heap_->begin() + offset, first, last);
                    rst = heap_->data() + offset;
                }
                else {
                    auto offset = pos - heap_->data();
                    heap_->insert(heap_->begin() + offset, first, last);
                    rst = heap_->data() + offset;
                }
            }
            else
                rst = stack_.insert(pos, first, last);
            size_ += count;
            return rst;
        }

        template<std::size_t... Ns>
        void re_ctor_stack_from_heap(std::index_sequence<Ns...>) {
            assert(stack_.empty() && heap_->size() >= N);
            static_assert(sizeof...(Ns) == N);
            heap_type& heap_ref = *heap_;
            new(&stack_)stack_type{ std::move(heap_ref[Ns])... };
        }
        void re_ctor_stack_from_heap() {
            re_ctor_stack_from_heap(std::make_index_sequence<N>{});
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
        size_type size_;
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
