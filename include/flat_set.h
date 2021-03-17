#pragma once

#include <cassert>
#include <algorithm>
#include <concepts>
#include <type_traits>

namespace Ubpa::details {
    template<typename Compare, bool = std::is_empty_v<Compare> && !std::is_final_v<Compare>>
    class flat_set_base : private Compare {
    protected:
        template<typename T>
        constexpr flat_set_base(T&& t) noexcept(std::is_nothrow_constructible_v<Compare, T>) :
            Compare{ std::forward<T>(t) } {}

        constexpr Compare& GetCompare() noexcept { return *this; }
        constexpr const Compare& GetCompare() const noexcept { return *this; }
    };

    template<typename Compare>
    class flat_set_base<Compare, false> {
    protected:
        template<typename T>
        constexpr flat_set_base(T&& t) noexcept(std::is_nothrow_constructible_v<Compare, T>) :
            comp{ std::forward<T>(t) } {}

        constexpr Compare& GetCompare() noexcept { return comp; }
        constexpr const Compare& GetCompare() const noexcept { return comp; }
    private:
        Compare comp;
    };
}

namespace Ubpa {
    template <typename T, template<typename>class Vector, typename Compare = std::less<T>>
    class flat_set : private details::flat_set_base<Compare> {
        using mybase = details::flat_set_base<Compare>;
    public:
        //////////////////
        // Member types //
        //////////////////

        using container_type = Vector<T>;
        using key_type = typename container_type::value_type;
        using value_type = typename container_type::value_type;
        using size_type = typename container_type::size_type;
        using difference_type = typename container_type::difference_type;
        using key_compare = Compare;
        using value_compare = Compare;
        using iterator = typename container_type::iterator;
        using const_iterator = typename container_type::const_iterator;
        using reverse_iterator = typename container_type::reverse_iterator;
        using const_reverse_iterator = typename container_type::const_reverse_iterator;

        static_assert(std::random_access_iterator<iterator>);

        //////////////////////
        // Member functions //
        //////////////////////

        flat_set() : mybase(Compare()) {}

        explicit flat_set(const container_type& sorted_storage, const Compare& comp = Compare())
            : mybase(comp), storage(sorted_storage)
        { assert(std::is_sorted(begin(), end(), this->GetCompare())); }

        explicit flat_set(container_type&& sorted_storage, const Compare& comp = Compare())
            : mybase(comp), storage(std::move(sorted_storage))
        { assert(std::is_sorted(begin(), end(), this->GetCompare())); }

        explicit flat_set(const Compare& comp) : mybase(comp) {}

        template<typename Iter> requires std::input_iterator<Iter>
        flat_set(Iter first, Iter last, const Compare& comp = Compare()) : mybase(comp)
        { insert(first, last); }

        flat_set(const flat_set&) = default;

        flat_set(flat_set&&) noexcept = default;

        flat_set(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
            : flat_set(ilist.begin(), ilist.end(), comp) {}

        flat_set& operator=(const flat_set&) = default;

        flat_set& operator=(flat_set&&) noexcept = default;

        flat_set& operator=(std::initializer_list<value_type> ilist) {
            clear();
            insert(ilist.begin(), ilist.end());
            return *this;
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
        // Capacity
        /////////////

        bool empty() const noexcept { return storage.empty(); }

        size_type size() const noexcept { return storage.size(); }

        size_type max_size() const noexcept { return storage.max_size(); }

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
            auto iter = find(key);
            if (iter == end())
                return 0;
            else {
                erase(iter);
                return 1;
            }
        }

        //
        // Lookup
        ///////////

        size_type count(const key_type& key) const { return static_cast<size_type>(find(key) != end()); }

        iterator find(const key_type& key) { return t_find(key); }

        const_iterator find(const key_type& key) const { return const_cast<flat_set*>(this)->find(key); }

        bool contains(const key_type& key) const { return lower_bound(key) != end(); }

        std::pair<iterator, iterator> equal_range(const key_type& key)
        { return std::equal_range(begin(), end(), key); }
        std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        { return std::equal_range(begin(), end(), key); }

        iterator lower_bound(const key_type& key)
        { return std::lower_bound(begin(), end(), key, this->GetCompare()); }
        const_iterator lower_bound(const key_type& key) const
        { return std::lower_bound(begin(), end(), key, this->GetCompare()); }

        iterator upper_bound(const key_type& key) { return std::upper_bound(begin(), end(), key); }
        const_iterator upper_bound(const key_type& key) const
        { return std::upper_bound(begin(), end(), key, this->GetCompare()); }
        
        // -- is_transparent

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        iterator find(const K& key) { return t_find(key); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        const_iterator find(const K& key) const { return const_cast<flat_set*>(this)->find(key); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        size_type count(const K& key) const { return static_cast<size_type>(find(key) != end()); }

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
            if (lb == end())
                return lb;
            else if (this->GetCompare()(key, *lb)) // key < lb
                return end();
            else
                return lb;
        }

        template<typename T> requires std::is_same<value_type, std::remove_cvref_t<T>>::value
        iterator emplace_hint_impl(const_iterator hint, T&& value) {
            assert(begin() <= hint && hint <= end());
            if (hint > begin()) {
                if (this->GetCompare()(*std::prev(hint), value)) { // value > hint - 1
                    if (hint < end()) {
                        if (this->GetCompare()(value, *hint)) // value < hint
                            return storage.insert(hint, std::forward<T>(value));
                        else { // value >= hint
                            auto lb = std::lower_bound(hint, cend(), value, this->GetCompare()); // value <= lb
                            if (lb == end()) {
                                storage.push_back(std::forward<T>(value));
                                return std::prev(end());
                            }
                            else if (this->GetCompare()(value, *lb)) // value < lb
                                return storage.insert(lb, std::forward<T>(value));
                            else
                                return begin() + std::distance(cbegin(), lb); // value == lb
                        }
                    }
                    else { // hint == end()
                        storage.push_back(std::forward<T>(value));
                        return std::prev(end());
                    }
                }
                else { // value <= hint - 1
                    auto lb = std::lower_bound(cbegin(), hint, value, this->GetCompare()); // value <= lb
                    if (this->GetCompare()(value, *lb)) // value < lb
                        return storage.insert(lb, std::forward<T>(value));
                    else // value == lb
                        return begin() + std::distance(cbegin(), lb);
                }
            }
            else { // hint == begin()
                if (hint < end()) {
                    if (this->GetCompare()(value, *hint)) // value < hint
                        return storage.insert(hint, std::forward<T>(value));
                    else { // value >= hint
                        auto lb = std::lower_bound(hint, cend(), value, this->GetCompare()); // value <= lb
                        if (lb == end()) {
                            storage.push_back(std::forward<T>(value));
                            return std::prev(end());
                        }
                        else if (this->GetCompare()(value, *lb)) // value < lb
                            return storage.insert(lb, std::forward<T>(value));
                        else
                            return begin() + std::distance(cbegin(), lb); // value == lb
                    }
                }
                else { // empty
                    storage.push_back(std::forward<T>(value));
                    return std::prev(end());
                }
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
        
        template<typename T> requires std::is_same<value_type, std::remove_cvref_t<T>>::value
        std::pair<iterator, bool> emplace_impl(T&& value) {
            auto lb = lower_bound(value); // key <= lb
            if (lb == end()) {
                storage.push_back(std::forward<T>(value));
                return { std::prev(end()), true };
            }
            else if (this->GetCompare()(value, *lb)) // key < lb
                return { storage.insert(lb, std::forward<T>(value)), true };
            else
                return { lb, false }; // key == lb
        }

        std::pair<iterator, bool> emplace_impl() {
            return emplace_impl(value_type());
        }

        template<typename Arg, typename... Args> requires std::bool_constant<sizeof...(Args) != 0>::value
            || std::negation_v<std::is_same<value_type, std::remove_cvref_t<Arg>>>
        std::pair<iterator, bool> emplace_impl(Arg&& arg, Args&&... args) {
            return emplace_impl(value_type(std::forward<Arg>(arg), std::forward<Args>(args)...));
        }
    };

    template<typename T, template<typename>class Vector, typename Compare>
    bool operator==(const flat_set<T, Vector, Compare>& lhs, const flat_set<T, Vector, Compare>& rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<typename T, template<typename>class Vector, typename Compare>
    bool operator<(const flat_set<T, Vector, Compare>& lhs, const flat_set<T, Vector, Compare>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename T, template<typename>class Vector, typename Compare>
    bool operator!=(const flat_set<T, Vector, Compare>& lhs, const flat_set<T, Vector, Compare>& rhs) {
        return !(lhs == rhs);
    }

    template<typename T, template<typename>class Vector, typename Compare>
    bool operator>(const flat_set<T, Vector, Compare>& lhs, const flat_set<T, Vector, Compare>& rhs) {
        return rhs < lhs;
    }

    template<typename T, template<typename>class Vector, typename Compare>
    bool operator<=(const flat_set<T, Vector, Compare>& lhs, const flat_set<T, Vector, Compare>& rhs) {
        return !(rhs < lhs);
    }

    template<typename T, template<typename>class Vector, typename Compare>
    bool operator>=(const flat_set<T, Vector, Compare>& lhs, const flat_set<T, Vector, Compare>& rhs) {
        return !(lhs < rhs);
    }
}
