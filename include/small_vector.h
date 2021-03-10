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
            stack_(count <= N ? count : 0),
            size_{ count }
        {
            if (count > N)
                heap_ = heap_type(count);
        }

        small_vector(size_type count, const value_type& value) :
            stack_(count <= N ? count : 0, value),
            size_{ count }
        {
            if (count > N)
                heap_ = heap_type(count, value);
        }

        small_vector(size_type count,
            const value_type& value,
            const allocator_type& alloc) :
            stack_(count <= N ? 0 : count, value),
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

        small_vector(std::initializer_list<T> initlist) :
            stack_{ initlist.size() <= N ? initlist : std::initializer_list<T>{} },
            size_{ initlist.size() }
        {
            if (initlist.size() > N)
                heap_ = heap_type{ initlist };
        }

        small_vector(std::initializer_list<T> initlist, const allocator_type& alloc) :
            stack_{ initlist.size() <= N ? initlist : std::initializer_list<T>{} },
            heap_{ initlist.size() <= N ? heap_type{ alloc } : heap_type{ initlist } },
            size_{ initlist.size() }
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

        allocator_type get_allocator() const noexcept {
            return heap_.has_value() ? heap_->get_allocator() : allocator_type{};
        }

        //
        // Element access
        ///////////////////

        reference at(size_type pos) {
            if (size_ <= N)
                return stack_.at(pos);
            else
                return heap_->at(pos);
        }

        const_reference at(size_type pos) const {
            if (size_ <= N)
                return stack_.at(pos);
            else
                return heap_->at(pos);
        }

        reference operator[](size_type pos) noexcept {
            if (size_ <= N)
                return stack_[pos];
            else
                return (*heap_)[pos];
        }

        const_reference operator[](size_type pos) const noexcept {
            if (size_ <= N)
                return stack_[pos];
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

        void push_back(const value_type& value) {
            if (size_ < N)
                stack_.push_back(value);
            else {
                if (size_ == N) {
                    if (!heap_.has_value()) {
                        heap_ = heap_type{};
                        heap_->reserve(N);
                    }
                    // move everything to heap
                    std::move(stack_.begin(), stack_.end(), std::back_inserter(*heap_));
                    stack_.clear();
                }
                heap_->push_back(value);
            }
            size_++;
        }

        void push_back(T&& value) {
            if (size_ < N)
                stack_.push_back(std::move(value));
            else {
                if (size_ == N) {
                    if (!heap_.has_value()) {
                        heap_ = heap_type{};
                        heap_->reserve(N);
                    }
                    // move everything to heap
                    std::move(stack_.begin(), stack_.end(), std::back_inserter(*heap_));
                    stack_.clear();
                }
                heap_->push_back(std::move(value));
            }
            size_++;
        }

        template<typename... Args>
        void emplace_back(Args&&... args) {
            if (size_ < N)
                stack_.emplace_back(std::forward<Args>(args)...);
            else {
                if (size_ == N) {
                    if (!heap_.has_value()) {
                        heap_ = heap_type{};
                        heap_->reserve(N);
                    }
                    // move everything to heap
                    std::move(stack_.begin(), stack_.end(), std::back_inserter(*heap_));
                    stack_.clear();
                }
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
                // new `count` of elements completely fit on stack
                if (size_ > N) {
                    // currently, all data on heap
                    // move back to stack
                    re_ctor_stack_from_heap();
                    heap_->clear();
                }
                else {
                    // all data already on stack
                    stack_.resize(count);
                }
            }
            else {
                // new `count` of data is going to be on the heap
                // check if data is currently on the stack
                if (size_ <= N) {
                    // move to heap
                    std::move(stack_.begin(), stack_.begin() + size_, std::back_inserter(*heap_));
                    stack_.clear();
                }
                heap_->resize(count);
            }
            size_ = count;
        }

        void resize(size_type count, const T& value) {
            if (count <= N) {
                // new `count` of elements completely fit on stack
                if (size_ > N) {
                    // currently, all data on heap
                    // move back to stack
                    re_ctor_stack_from_heap();
                    heap_->clear();
                }
                else {
                    // all data already on stack
                    stack_.resize(count, value);
                }
            }
            else {
                // new `count` of data is going to be on the heap
                // check if data is currently on the stack
                if (size_ < N) {
                    // move to heap
                    std::move(stack_.begin(), stack_.begin() + size_, std::back_inserter(*heap_));
                    stack_.clear();
                }
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
