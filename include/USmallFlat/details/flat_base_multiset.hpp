#pragma once

#include <cassert>
#include <algorithm>
#include <concepts>
#include <type_traits>

namespace Ubpa::details {
    template<typename Compare, bool = std::is_empty_v<Compare> && !std::is_final_v<Compare>>
    class flat_base_multiset_base : private Compare {
    protected:
        template<typename Key>
        constexpr flat_base_multiset_base(Key&& t) noexcept(std::is_nothrow_constructible_v<Compare, Key>) :
            Compare{ std::forward<Key>(t) } {}

        constexpr Compare& GetCompare() noexcept { return *this; }
        constexpr const Compare& GetCompare() const noexcept { return *this; }
    };

    template<typename Compare>
    class flat_base_multiset_base<Compare, false> {
    protected:
        template<typename Key>
        constexpr flat_base_multiset_base(Key&& t) noexcept(std::is_nothrow_constructible_v<Compare, Key>) :
            comp{ std::forward<Key>(t) } {}

        constexpr Compare& GetCompare() noexcept { return comp; }
        constexpr const Compare& GetCompare() const noexcept { return comp; }
    private:
        Compare comp;
    };

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename Compare, typename RKey = typename Vector<Key>::value_type>
    class flat_base_multiset : private flat_base_multiset_base<Compare> {
        using mybase = flat_base_multiset_base<Compare>;
    public:
        //////////////////
        // Member types //
        //////////////////

        static constexpr bool is_multi = IsMulti;

        using container_type = Vector<Key>;
        using key_type = RKey;
        using value_type = typename container_type::value_type;
        using size_type = typename container_type::size_type;
        using difference_type = typename container_type::difference_type;
        using key_compare = Compare;
        using value_compare = Compare;
        using pointer = typename container_type::pointer;
        using const_pointer = typename container_type::const_pointer;
        using reference = typename container_type::reference;
        using const_reference = typename container_type::const_reference;
        using iterator = typename container_type::iterator;
        using const_iterator = typename container_type::const_iterator;
        using reverse_iterator = typename container_type::reverse_iterator;
        using const_reverse_iterator = typename container_type::const_reverse_iterator;

        static_assert(std::random_access_iterator<iterator>);

        //////////////////////
        // Member functions //
        //////////////////////

        flat_base_multiset() : mybase(Compare()) {}

        explicit flat_base_multiset(const container_type& sorted_storage, const Compare& comp = Compare())
            : mybase(comp), storage(sorted_storage)
        { assert(std::is_sorted(begin(), end(), this->GetCompare())); }

        explicit flat_base_multiset(container_type&& sorted_storage, const Compare& comp = Compare())
            : mybase(comp), storage(std::move(sorted_storage))
        { assert(std::is_sorted(begin(), end(), this->GetCompare())); }

        explicit flat_base_multiset(const Compare& comp) : mybase(comp) {}

        template<typename Iter> requires std::input_iterator<Iter>
        flat_base_multiset(Iter first, Iter last, const Compare& comp = Compare()) : mybase(comp)
        { insert(first, last); }

        flat_base_multiset(const flat_base_multiset&) = default;

        flat_base_multiset(flat_base_multiset&&) noexcept = default;

        flat_base_multiset(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
            : flat_base_multiset(ilist.begin(), ilist.end(), comp) {}

        Impl& operator=(const flat_base_multiset& rhs) {
            storage = rhs.storage;
            return static_cast<Impl&>(*this);
        }

        Impl& operator=(flat_base_multiset&& rhs) noexcept {
            storage = std::move(rhs.storage);
            return static_cast<Impl&>(*this);
        }

        Impl& operator=(std::initializer_list<value_type> ilist) {
            clear();
            insert(ilist.begin(), ilist.end());
            return static_cast<Impl&>(*this);
        }

        //
        // Iterators
        //////////////

        iterator begin() noexcept { return storage.begin(); }
        const_iterator begin() const noexcept { return storage.begin(); }
        const_iterator cbegin() const noexcept { return storage.cbegin(); }

        iterator end() noexcept { return storage.end(); }
        const_iterator end() const noexcept { return storage.end(); }
        const_iterator cend() const noexcept { return storage.cend(); }

        reverse_iterator rbegin() noexcept { return storage.rbegin(); }
        const_reverse_iterator rbegin() const noexcept { return storage.rbegin(); }
        const_reverse_iterator crbegin() const noexcept { return storage.crbegin(); }

        reverse_iterator rend() noexcept { return storage.rend(); }
        const_reverse_iterator rend() const noexcept { return storage.rend(); }
        const_reverse_iterator crend() const noexcept { return storage.crend(); }

        //
        // Element access
        ///////////////////

        pointer data() noexcept { return storage.data(); }
        const_pointer data() const noexcept { return storage.data(); }

        reference front() { return storage.front(); }
        const_reference front() const { return storage.front(); }

        reference back() { return storage.back(); }
        const_reference back() const { return storage.back(); }

        //
        // Capacity
        /////////////

        bool empty() const noexcept { return storage.empty(); }

        size_type size() const noexcept { return storage.size(); }

        size_type max_size() const noexcept { return storage.max_size(); }

        size_type capacity() const noexcept { return storage.capacity(); }

        void shrink_to_fit() { return storage.shrink_to_fit(); }

        //
        // Modifiers
        //////////////

        void clear() noexcept { storage.clear(); }

        std::pair<iterator, bool> insert(const value_type& value) { return emplace(value); }

        std::pair<iterator, bool> insert(value_type&& value) { return emplace(std::move(value)); }

        iterator insert(const_iterator hint, const value_type& value) { return emplace_hint(hint, value); }

        iterator insert(const_iterator hint, value_type&& value) { return emplace_hint(hint, std::move(value)); }

        template<typename InputIt>
        void insert(InputIt first, InputIt last) {
            for (; first != last; ++first)
                insert(*first);
        }

        void insert(std::initializer_list<value_type> ilist) { insert(ilist.begin(), ilist.end()); }

        template<typename... Args>
        std::pair<iterator, bool> emplace(Args&&... args) {
            return emplace_impl(std::forward<Args>(args)...);
        }

        template<typename... Args>
        iterator emplace_hint(const_iterator hint, Args&&... args) {
            return emplace_hint_impl(hint, std::forward<Args>(args)...);
        }

        iterator erase(const_iterator pos) { return storage.erase(pos); }
        iterator erase(iterator pos) { return storage.erase(pos); }
        iterator erase(const_iterator first, const_iterator last) { return storage.erase(first, last); }
        size_type erase(const key_type& key) {
            if constexpr (is_multi) {
                auto [begin_iter, end_iter] = equal_range(key);
                size_type cnt = std::distance(begin_iter, end_iter);
                iterator cursor = begin_iter;
                for (size_type i = 0; i < cnt; ++i)
                    cursor = erase(cursor);
                return cnt;
            }
            else {
                auto iter = find(key);
                if (iter == end())
                    return 0;
                else {
                    erase(iter);
                    return 1;
                }
            }
        }

        //
        // Lookup
        ///////////

        size_type count(const key_type& key) const {
            if constexpr (is_multi) {
                auto [begin_iter, end_iter] = equal_range(key);
                return std::distance(begin_iter, end_iter);
            }
            else
                return static_cast<size_type>(find(key) != end());
        }

        iterator find(const key_type& key) { return t_find(key); }

        const_iterator find(const key_type& key) const { return const_cast<flat_base_multiset*>(this)->find(key); }

        bool contains(const key_type& key) const { return find(key) != end(); }

        std::pair<iterator, iterator> equal_range(const key_type& key)
        { return std::equal_range(begin(), end(), key, this->GetCompare()); }
        std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        { return std::equal_range(begin(), end(), key, this->GetCompare()); }

        iterator lower_bound(const key_type& key)
        { return std::lower_bound(begin(), end(), key, this->GetCompare()); }
        const_iterator lower_bound(const key_type& key) const
        { return std::lower_bound(begin(), end(), key, this->GetCompare()); }

        iterator upper_bound(const key_type& key)
        { return std::upper_bound(begin(), end(), key, this->GetCompare()); }
        const_iterator upper_bound(const key_type& key) const
        { return std::upper_bound(begin(), end(), key, this->GetCompare()); }
        
        // -- is_transparent

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        iterator find(const K& key) { return t_find(key); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        const_iterator find(const K& key) const { return const_cast<flat_base_multiset*>(this)->find(key); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        size_type count(const K& key) const {
            if constexpr (is_multi) {
                auto [begin_iter, end_iter] = equal_range(key);
                return std::distance(begin_iter, end_iter);
            }
            else
                return static_cast<size_type>(find(key) != end());
        }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        bool contains(const K& key) const { return find(key) != end(); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        iterator lower_bound(const K& key)
        { return std::lower_bound(begin(), end(), key, this->GetCompare()); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        const_iterator lower_bound(const K& key) const
        { return std::lower_bound(begin(), end(), key, this->GetCompare()); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        iterator upper_bound(const K& key)
        { return std::upper_bound(begin(), end(), key, this->GetCompare()); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        const_iterator upper_bound(const K& key) const
        { return std::upper_bound(begin(), end(), key, this->GetCompare()); }

        //
        // Observers
        //////////////

        key_compare key_comp() const { return this->GetCompare(); }
        value_compare value_comp() const { return this->GetCompare(); }

    protected:
        ////////////////////
        // Member Objects //
        ////////////////////

        container_type storage;

    private:
        template<typename K>
        iterator t_find(const K& key) {
            auto lb = lower_bound(key); // key <= lb
            auto e = end();
            if (lb == e || this->GetCompare()(key, *lb)) // key < lb
                return e;
            else
                return lb;
        }

        template<typename V> requires std::is_same<value_type, std::remove_cvref_t<V>>::value
        iterator emplace_hint_impl(const_iterator hint, V&& value) {
            assert(begin() <= hint && hint <= end());
            
            const_iterator first;
            const_iterator last;

            auto& comp = this->GetCompare();

            if (hint == begin() || comp(*std::prev(hint), value)) { // value > hint - 1
                if (hint < end()) {
                    if constexpr (is_multi) {
                        if (!comp(*hint, value)) // value <= hint
                            return storage.insert(hint, std::forward<V>(value));
                        else { // value > hint
                            auto lb = std::lower_bound(std::next(hint), cend(), value, comp); // value <= lb
                            return storage.insert(lb, std::forward<V>(value));
                        }
                    }
                    else {
                        if (comp(value, *hint)) // value < hint
                            return storage.insert(hint, std::forward<V>(value));
                        else { // value >= hint
                            first = hint;
                            last = cend();
                        }
                    }
                }
                else { // hint == end()
                    storage.push_back(std::forward<V>(value));
                    return std::prev(end());
                }
            }
            else { // value <= hint - 1
                first = cbegin();
                last = hint;
            }

            auto lb = std::lower_bound(first, last, value, comp); // value <= lb

            if constexpr (is_multi)
                return storage.insert(lb, std::forward<V>(value));
            else {
                if (lb == end() || comp(value, *lb)) // value < lb
                    return storage.insert(lb, std::forward<V>(value));
                else
                    return begin() + std::distance(cbegin(), lb); // value == lb
            }
        }
        
        iterator emplace_hint_impl(const_iterator hint) {
            return emplace_hint_impl(hint, value_type());
        }

        template<typename Arg, typename... Args> requires std::bool_constant<sizeof...(Args) != 0>::value
            || std::negation_v<std::is_same<value_type, std::remove_cvref_t<Arg>>>
        iterator emplace_hint_impl(const_iterator hint, Arg&& arg, Args&&... args) {
            return emplace_hint_impl(hint, value_type(std::forward<Arg>(arg), std::forward<Args>(args)...));
        }
        
        template<typename V> requires std::is_same<value_type, std::remove_cvref_t<V>>::value
        std::pair<iterator, bool> emplace_impl(V&& value) {
            auto lb = lower_bound(value); // key <= lb
            if (lb == end() || this->GetCompare()(value, *lb)) // key < lb
                return { storage.insert(lb, std::forward<V>(value)), true };
            else
                return { lb, false }; // key == lb
        }

        std::pair<iterator, bool> emplace_impl() { return emplace_impl(value_type()); }

        template<typename Arg, typename... Args> requires std::bool_constant<sizeof...(Args) != 0>::value
            || std::negation_v<std::is_same<value_type, std::remove_cvref_t<Arg>>>
        std::pair<iterator, bool> emplace_impl(Arg&& arg, Args&&... args) {
            return emplace_impl(value_type(std::forward<Arg>(arg), std::forward<Args>(args)...));
        }
    };

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename Compare>
    bool operator==(const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& lhs, const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename Compare>
    bool operator<(const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& lhs, const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename Compare>
    bool operator!=(const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& lhs, const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& rhs) {
        return !(lhs == rhs);
    }

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename Compare>
    bool operator>(const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& lhs, const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& rhs) {
        return rhs < lhs;
    }

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename Compare>
    bool operator<=(const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& lhs, const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& rhs) {
        return !(rhs < lhs);
    }

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename Compare>
    bool operator>=(const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& lhs, const flat_base_multiset<Impl, IsMulti, Vector, Key, Compare>& rhs) {
        return !(lhs < rhs);
    }
}
