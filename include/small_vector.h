#pragma once
#pragma once

#include <array>
#include <vector>
#include <iterator>
#include <optional>

#include <cassert>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 26495 )
#endif

namespace Ubpa {
    template <class T, std::size_t N, class Allocator = std::allocator<T>>
    class small_vector {
        using raw_stack_type = std::aligned_storage_t<sizeof(T)* N, alignof(T)>;
        using value_stack_type = std::array<T, N>;
        using heap_type = std::vector<T, Allocator>;

        value_stack_type& get_value_stack() noexcept { return reinterpret_cast<value_stack_type&>(stack_); }
        const value_stack_type& get_value_stack() const noexcept { return const_cast<small_vector*>(this)->get_value_stack(); }

    public:
        using value_type = T;
        using size_type = std::size_t;
        using reference = typename std::vector<T, Allocator>::reference;
        using const_reference = typename std::vector<T, Allocator>::const_reference;
        using allocator_type = Allocator;
        using pointer = T*;
        using const_pointer = const T*;

        class iterator {
            pointer ptr_;

        public:
            using self_type = iterator;
            using value_type = T;
            using reference = typename std::vector<T, Allocator>::reference;
            using pointer = T*;
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;

            iterator(pointer ptr) : ptr_(ptr) {}
            self_type& operator++() {
                ptr_++;
                return *this;
            }
            self_type& operator--() {
                ptr_--;
                return *this;
            }
            self_type operator++(int) {
                self_type i = *this;
                ptr_++;
                return i;
            }
            self_type operator--(int) {
                self_type i = *this;
                ptr_--;
                return i;
            }
            reference operator*() { return *ptr_; }
            pointer operator->() { return ptr_; }
            friend bool operator==(const self_type& lhs, const self_type& rhs) { return lhs.ptr_ == rhs.ptr_; }
            friend bool operator!=(const self_type& lhs, const self_type& rhs) { return lhs.ptr_ != rhs.ptr_; }
        };

        class const_iterator {
            pointer ptr_;

        public:
            using self_type = iterator;
            using value_type = T;
            using reference = typename std::vector<T, Allocator>::reference;
            using pointer = T*;
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            
            const_iterator(pointer ptr) : ptr_(ptr) {}
            self_type& operator++() {
                ptr_++;
                return *this;
            }
            self_type& operator--() {
                ptr_--;
                return *this;
            }
            self_type operator++(int) {
                self_type i = *this;
                ptr_++;
                return i;
            }
            self_type operator--(int) {
                self_type i = *this;
                ptr_--;
                return i;
            }
            const value_type& operator*() { return *ptr_; }
            const pointer operator->() { return ptr_; }
            friend bool operator==(const self_type& lhs, const self_type& rhs) { return lhs.ptr_ == rhs.ptr_; }
            friend bool operator!=(const self_type& lhs, const self_type& rhs) { return lhs.ptr_ != rhs.ptr_; }
        };

        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr small_vector() noexcept = default;

        explicit small_vector(size_type count, const value_type& value = {}) : size_{ count } {
            if (count <= N) {
                for (size_type i = 0; i < count; i++)
                    new((value_type*)&stack_ + i)value_type{ value };
            }
            else {
                // use heap
                heap_ = heap_type(count, value);
            }
        }

        explicit small_vector(size_type count,
            const value_type& value,
            const allocator_type& alloc)
            : size_{ count }
        {
            if (count <= N) {
                for (size_type i = 0; i < count; i++)
                    new((value_type*)&stack_ + i)value_type{ value };
                heap_ = heap_type{ alloc };
            }
            else {
                // use heap
                heap_ = heap_type{ count, value, alloc };
            }
        }

        small_vector(const small_vector& other)
            : heap_{ other.heap_ }, size_(other.size_)
        {
            if (size_ <= N) {
                for (size_type i = 0; i < size_; i++)
                    new((value_type*)&stack_ + i)value_type{ other.get_value_stack()[i] };
            }
        }

        small_vector(const small_vector& other, const allocator_type& alloc)
            : heap_{ std::in_place_t{}, other.heap_.has_value() ? *other.heap_ : heap_type{}, alloc },
            size_{ other.size_ }
        {
            if (size_ <= N) {
                for (size_type i = 0; i < size_; i++)
                    new((value_type*)&stack_ + i)value_type{ other.get_value_stack()[i] };
            }
        }

        small_vector(small_vector&& other)
            : heap_(std::move(other.heap_)),
            size_(std::move(other.size_))
        {
            if (size_ <= N) {
                for (size_type i = 0; i < size_; i++)
                    new((value_type*)&stack_ + i)value_type{ std::move(other.get_value_stack()[i]) };
            }
        }

        small_vector(small_vector&& other, const allocator_type& alloc)
            : heap_{ std::in_place_t{}, std::move(other.heap_.has_value() ? *other.heap_ : heap_type{}), alloc },
            size_{ other.size_ }
        {
            if (size_ <= N) {
                for (size_type i = 0; i < size_; i++)
                    new((value_type*)&stack_ + i)value_type{ std::move(other.get_value_stack()[i]) };
            }
        }

        small_vector(std::initializer_list<T> initlist) : size_{ initlist.size() } {
            if (size_ <= N) {
                for (std::size_t i = 0; i < size_; i++)
                    new((value_type*)&stack_ + i)value_type{ std::move(*(initlist.begin() + i)) };
            }
            else
                heap_ = heap_type{ initlist };
        }

        small_vector(std::initializer_list<T> initlist, const allocator_type& alloc) : size_{ initlist.size() } {
            if (size_ <= N) {
                for (std::size_t i = 0; i < size_; i++)
                    new((value_type*)&stack_ + i)value_type{ std::move(*(initlist.begin() + i)) };
                heap_ = heap_type{ alloc };
            }
            else
                heap_ = heap_type{ initlist };
        }

        ~small_vector() {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                if (size_ <= N) {
                    for (size_type i = 0; i < size_; i++)
                        get_value_stack()[i].~T();
                }
            }
        }

        small_vector& operator=(const small_vector& rhs) {
            if (size_ <= N) {
                for (size_type i = 0; i < size_; i++)
                    get_value_stack()[i] = rhs.get_value_stack()[i];
            }
            heap_ = rhs.heap_;
            size_ = rhs.size_;
            return *this;
        }

        small_vector& operator=(small_vector&& rhs) {
            if (size_ <= N) {
                for (size_type i = 0; i < size_; i++)
                    get_value_stack()[i] = std::move(rhs.get_value_stack()[i]);
            }
            heap_ = std::move(rhs.heap_);
            size_ = std::move(rhs.size_);
            return *this;
        }

        small_vector& operator=(std::initializer_list<value_type> rhs) {
            if (rhs.size() <= N)
                for (size_type i = 0; i < size_; i++)
                    get_value_stack()[i] = std::move(*(rhs.begin() + i));
            else
                heap_ = rhs;
            size_ = rhs.size();
        }

        allocator_type get_allocator() const noexcept {
            return heap_.has_value() ? heap_->get_allocator() : allocator_type{};
        }

        reference at(size_type pos) {
            if (size_ <= N)
                return get_value_stack().at(pos);
            else
                return heap_->at(pos);
        }

        const_reference at(size_type pos) const {
            if (size_ <= N)
                return get_value_stack().at(pos);
            else
                return heap_->at(pos);
        }

        reference operator[](size_type pos) {
            if (size_ <= N)
                return get_value_stack()[pos];
            else
                return (*heap_)[pos];
        }

        const_reference operator[](size_type pos) const {
            if (size_ <= N)
                return get_value_stack()[pos];
            else
                return (*heap_)[pos];
        }

        reference front() {
            if (size_ <= N)
                return get_value_stack().front();
            else
                return heap_->front();
        }

        const_reference front() const {
            if (size_ <= N)
                return get_value_stack().front();
            else
                return heap_->front();
        }

        reference back() {
            if (size_ <= N)
                return get_value_stack()[size_ - 1];
            else
                return (*heap_)[size_ - 1];
        }

        const_reference back() const {
            if (size_ < N) {
                return get_value_stack()[size_ - 1];
            }
            else {
                return (*heap_)[size_ - 1];
            }
        }

        pointer data() noexcept {
            if (size_ < N)
                return get_value_stack().data();
            else
                return heap_->data();
        }

        const_pointer data() const noexcept {
            if (size_ < N)
                return get_value_stack().data();
            else
                return heap_->data();
        }

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
            if (heap_.has_value())
                return heap_->capacity();
            else
                return N;
        }

        void clear() noexcept {
            if (size_ > N)
                heap_->clear();
            else {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    for (size_type i = 0; i < size_; i++)
                        get_value_stack()[i].~T();
                }
            }
            size_ = 0;
        }

        void push_back(const value_type& value) {
            if (size_ < N)
                new((value_type*)&stack_ + size_)value_type{ value };
            else {
                if (size_ == N) {
                    if (!heap_.has_value()) {
                        heap_ = heap_type{};
                        heap_->reserve(N);
                    }
                    // move everything to heap
                    std::move(get_value_stack().begin(), get_value_stack().end(), std::back_inserter(*heap_));
                    if constexpr (!std::is_trivially_destructible_v<value_type>) {
                        for (std::size_t i = 0; i < N; i++)
                            get_value_stack()[i].~T();
                    }
                }
                heap_->push_back(value);
            }
            size_ += 1;
        }

        void push_back(T&& value) {
            if (size_ < N)
                new((value_type*)&stack_ + size_)value_type{ std::move(value) };
            else {
                if (size_ == N) {
                    if (!heap_.has_value()) {
                        heap_ = heap_type{};
                        heap_->reserve(N);
                    }
                    // move everything to heap
                    std::move(get_value_stack().begin(), get_value_stack().end(), std::back_inserter(*heap_));
                    if constexpr (!std::is_trivially_destructible_v<value_type>) {
                        for (std::size_t i = 0; i < N; i++)
                            get_value_stack()[i].~T();
                    }
                }
                heap_->push_back(std::move(value));
            }
            size_ += 1;
        }

        template<typename... Args>
        void emplace_back(Args&&... args) {
            if (size_ < N)
                new((value_type*)&stack_ + size_)value_type{ std::forward<Args>(args)... };
            else {
                if (size_ == N) {
                    if (!heap_.has_value()) {
                        heap_ = heap_type{};
                        heap_->reserve(N);
                    }
                    // move everything to heap
                    std::move(get_value_stack().begin(), get_value_stack().end(), std::back_inserter(*heap_));
                    if constexpr (!std::is_trivially_destructible_v<value_type>) {
                        for (std::size_t i = 0; i < N; i++)
                            get_value_stack()[i].~T();
                    }
                }
                heap_->emplace_back(std::forward<Args>(args)...);
            }
            size_ += 1;
        }

        void pop_back() {
            if (size_ == 0)
                return; // do nothing

            if (size_ <= N) {
                get_value_stack()[size_ - 1].~T();
                size_ -= 1;
            }
            else if (size_ == N + 1) {
                // move back to stack
                for (size_type i = 0; i < N; i++)
                    new((value_type*)&stack_ + i)value_type{ std::move((*heap_)[i]) };
                heap_->clear();
                size_ = N;
            }
            else {
                // currently using heap
                heap_->pop_back();
                size_ -= 1;
            }
        }

        // Resizes the container to contain count elements.
        void resize(size_type count) {
            if (count <= N) {
                // new `count` of elements completely fit on stack
                if (size_ > N) {
                    // currently, all data on heap
                    // move back to stack
                    for (size_type i = 0; i < count; i++)
                        new((value_type*)&stack_ + i)value_type{ std::move((*heap_)[i]) };
                    heap_->clear();
                }
                else {
                    // all data already on stack

                    if constexpr (!std::is_trivially_default_constructible_v<T>) {
                        for (size_type i = size_; i < count; i++)
                            new((value_type*)&stack_ + i)value_type{};
                    }

                    if constexpr (!std::is_trivially_destructible_v<T>) {
                        for (size_type i = count; i < size_; i++)
                            get_value_stack()[i].~T();
                    }
                }
            }
            else {
                // new `count` of data is going to be on the heap
                // check if data is currently on the stack
                if (size_ <= N) {
                    // move to heap
                    std::move(get_value_stack().begin(), get_value_stack().begin() + size_, std::back_inserter(*heap_));
                    if constexpr (!std::is_trivially_destructible_v<value_type>) {
                        for (std::size_t i = 0; i < size_; i++)
                            get_value_stack()[i].~T();
                    }
                }
                heap_->resize(count);
            }
            size_ = count;
        }

        // Resizes the container to contain count elements.
        void resize(size_type count, const T& value) {
            if (count <= N) {
                // new `count` of elements completely fit on stack
                if (size_ > N) {
                    // currently, all data on heap
                    // move back to stack
                    for (size_type i = 0; i < count; i++)
                        new((value_type*)&stack_ + i)value_type{ std::move((*heap_)[i]) };
                    heap_->clear();
                }
                else {
                    // all data already on stack

                    if constexpr (!std::is_trivially_copy_constructible_v<T>) {
                        for (size_type i = size_; i < count; i++)
                            new((value_type*)&stack_ + i)value_type{ value };
                    }

                    if constexpr (!std::is_trivially_destructible_v<T>) {
                        for (size_type i = count; i < size_; i++)
                            get_value_stack()[i].~T();
                    }
                }
            }
            else {
                // new `count` of data is going to be on the heap
                // check if data is currently on the stack
                if (size_ < N) {
                    // move to heap
                    std::move(get_value_stack().begin(), get_value_stack().end(), std::back_inserter(*heap_));
                    if constexpr (!std::is_trivially_destructible_v<value_type>) {
                        for (std::size_t i = 0; i < size_; i++)
                            get_value_stack()[i].~T();
                    }
                }
                heap_->resize(count, value);
            }
            size_ = count;
        }

        void swap(small_vector& other) noexcept {
            if constexpr (std::is_trivially_move_constructible_v<T> && std::is_trivially_move_assignable_v<T>)
                std::swap(stack_, other.stack_);

            raw_stack_type* from, * to;
            size_type swap_cnt, move_cnt;
            if (size_ > N) {
                from = &other.stack_;
                to = &stack_;

                swap_cnt = 0;
                move_cnt = other.size_ > N ? 0 : other.size_;
            }
            else if (other.size_ > N) {
                from = &stack_;
                to = &other.stack_;

                swap_cnt = 0;
                move_cnt = size_ > N ? 0 : size_;
            }
            else if (size_ > other.size_) {
                from = &stack_;
                to = &other.stack_;
                swap_cnt = other.size_;
                move_cnt = size_ - swap_cnt;
            }
            else {
                from = &other.stack_;
                to = &stack_;
                swap_cnt = size_;
                move_cnt = other.size_ - swap_cnt;
            }
            for (std::size_t i = 0; i < swap_cnt; i++)
                std::swap(get_value_stack()[i], other.get_value_stack()[i]);
            for (std::size_t i = swap_cnt; i < move_cnt; i++) {
                auto& v = reinterpret_cast<value_type&>(from[i]);
                new(&to[i])value_type(std::move(v));
                if constexpr (!std::is_trivially_destructible_v<T>)
                    v.~T();
            }

            std::swap(heap_, other.heap_);
            std::swap(size_, other.size_);
        };

        iterator begin() noexcept {
            if (size_ <= N)
                return iterator(get_value_stack().data());
            else
                return iterator(heap_->data());
        }

        iterator end() noexcept {
            if (size_ <= N)
                return iterator(get_value_stack().data() + size_);
            else
                return iterator(heap_->data() + size_);
        }

        const_iterator begin() const noexcept {
            if (size_ <= N)
                return const_iterator(get_value_stack().data());
            else
                return const_iterator(heap_->data());
        }

        const_iterator end() const noexcept {
            if (size_ <= N)
                return const_iterator(get_value_stack().data() + size_);
            else
                return const_iterator(heap_->data() + size_);
        }

        const_iterator cbegin() const noexcept { return begin(); }
        const_iterator cend() const noexcept { return end(); }

        reverse_iterator rbegin() noexcept {
            if (size_ <= N)
                return reverse_iterator(get_value_stack().data() + size_);
            else
                return reverse_iterator(heap_->data() + size_);
        }

        reverse_iterator rend() noexcept {
            if (size_ <= N)
                return reverse_iterator(get_value_stack().data());
            else
                return reverse_iterator(heap_->data());
        }

        const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        const_reverse_iterator crend() const noexcept { return rend(); }

    private:
        raw_stack_type stack_;
        std::optional<heap_type> heap_;
        size_type size_{ 0 };
    };

}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
