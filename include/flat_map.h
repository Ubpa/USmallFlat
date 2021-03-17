#pragma once

#include "flat_set.h"
#include <utility>
#include <tuple>
#include <stdexcept>

namespace Ubpa::details {
    template<typename Compare, bool = std::is_empty_v<Compare> && !std::is_final_v<Compare>>
    class flat_map_comp_storage : private Compare {
    public:
        template<typename T>
        constexpr flat_map_comp_storage(T&& t) noexcept(std::is_nothrow_constructible_v<Compare, T>) :
            Compare{ std::forward<T>(t) } {}

    protected:
        constexpr Compare& GetCompare() noexcept { return *this; }
        constexpr const Compare& GetCompare() const noexcept { return *this; }
    };

    template<typename Compare>
    class flat_map_comp_storage<Compare, false> {
    public:
        template<typename T>
        constexpr flat_map_comp_storage(T&& t) noexcept(std::is_nothrow_constructible_v<Compare, T>) :
            comp{ std::forward<T>(t) } {}

    protected:
        constexpr Compare& GetCompare() noexcept { return comp; }
        constexpr const Compare& GetCompare() const noexcept { return comp; }

    private:
        Compare comp;
    };

    template<typename T>
    concept contain_is_transparent = requires {
        typename T::is_transparent;
    };

    template<typename Value, typename Compare,
        bool value_as_base = std::is_empty_v<Compare> && !std::is_final_v<Compare>,
        bool = contain_is_transparent<Compare>>
    class flat_map_comp : public flat_map_comp_storage<Compare, value_as_base> {
        using storage = flat_map_comp_storage<Compare, value_as_base>;
    public:
        using storage::storage;

        template <class Ty1, class Ty2>
        constexpr decltype(auto) operator()(Ty1&& lhs, Ty2&& rhs) const
            noexcept(noexcept(static_cast<Ty1&&>(lhs) < static_cast<Ty2&&>(rhs))) // strengthened
        { return this->GetCompare()(static_cast<Ty1&&>(lhs), static_cast<Ty2&&>(rhs)); }

        using is_transparent = int;
    };

    template<typename Value, typename Compare, bool value_as_base>
    class flat_map_comp<Value, Compare, value_as_base, false> : public flat_map_comp_storage<Compare, value_as_base> {
        using storage = flat_map_comp_storage<Compare, value_as_base>;
        using Key = std::remove_const_t<std::tuple_element_t<0, Value>>;
    public:
        using storage::storage;

        constexpr bool operator()(const Value& lhs, const Value& rhs) const {
            return this->GetCompare()(std::get<0>(lhs), std::get<0>(rhs));
        }
        constexpr bool operator()(const Key& lhs, const Value& rhs) const {
            return this->GetCompare()(lhs, std::get<0>(rhs));
        }
        constexpr bool operator()(const Value& lhs, const Key& rhs) const {
            return this->GetCompare()(std::get<0>(lhs), rhs);
        }

        using is_transparent = int;
    };

}

namespace Ubpa {
    // require
    // - Vector<std::pair<const Key, T>>::iterator <=> Vector<std::pair<Key, T>>::iterator
    // - Vector<std::pair<const Key, T>>::const_iterator <=> Vector<std::pair<Key, T>>::const_iterator
    template <typename Key, typename T, template<typename>class Vector, typename Compare = std::less<Key>>
    class flat_map : protected flat_set<std::pair<Key, T>, Vector, details::flat_map_comp<std::pair<Key, T>, Compare>>  {
        using mybase = flat_set<std::pair<Key, T>, Vector, details::flat_map_comp<std::pair<Key, T>, Compare>>;
    public:
        //////////////////
        // Member types //
        //////////////////

        using key_type = Key;
        using mapped_type = T;
        using value_type = std::pair<const Key, T>;
        using size_type = typename mybase::size_type;
        using difference_type = typename mybase::difference_type;
        using key_compare = typename mybase::key_compare;

        using container_type = typename mybase::container_type;

        using iterator = typename Vector<std::pair<const key_type, mapped_type>>::iterator;
        using const_iterator = typename Vector<std::pair<const key_type, mapped_type>>::const_iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        static_assert(std::random_access_iterator<iterator>);

        //////////////////////
        // Member functions //
        //////////////////////

        flat_map() = default;

        explicit flat_map(const container_type& sorted_storage, const Compare& comp = Compare())
            : mybase(sorted_storage, comp) {}

        explicit flat_map(container_type&& sorted_storage, const Compare& comp = Compare())
            : mybase(std::move(sorted_storage), comp) {}

        explicit flat_map(const Compare& comp) : mybase(comp) {}

        template<typename Iter> requires std::input_iterator<Iter>
        flat_map(Iter first, Iter last, const Compare& comp = Compare()) : mybase(first, last, comp) {}

        flat_map(const flat_map&) = default;

        flat_map(flat_map&&) noexcept = default;

        flat_map(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
            : flat_map(ilist.begin(), ilist.end(), comp) {}

        flat_map& operator=(const flat_map&) = default;

        flat_map& operator=(flat_map&&) noexcept = default;

        flat_map& operator=(std::initializer_list<value_type> ilist) {
            mybase::opeator=(ilist);
            return *this;
        }

        //
        // Element access
        ///////////////////

        mapped_type& at(const key_type& key) {
            auto target = find(key);
            if (target == end())
                throw_out_of_range();
            return std::get<1>(*target);
        }

        const mapped_type& at(const key_type& key) const { return const_cast<flat_map*>(this)->at(key); }

        mapped_type& operator[](const key_type& key) { return std::get<1>(*try_emplace(key)->first); }
        mapped_type& operator[](key_type&& key) { return std::get<1>(*try_emplace(std::move(key))->first); }

        //
        // Iterators
        //////////////

        iterator begin() noexcept { return cast_iterator(mybase::begin()); }
        const_iterator begin() const noexcept { return cast_iterator(mybase::begin()); }
        const_iterator cbegin() const noexcept { return cast_iterator(mybase::cbegin()); }

        iterator end() noexcept { return cast_iterator(mybase::end()); }
        const_iterator end() const noexcept { return cast_iterator(mybase::end()); }
        const_iterator cend() const noexcept { return cast_iterator(mybase::cend()); }

        reverse_iterator rbegin() noexcept { return cast_iterator(mybase::rbegin()); }
        const_reverse_iterator rbegin() const noexcept { return cast_iterator(mybase::rbegin()); }
        const_reverse_iterator crbegin() const noexcept { return cast_iterator(mybase::crbegin()); }

        reverse_iterator rend() noexcept { return cast_iterator(mybase::rend()); }
        const_reverse_iterator rend() const noexcept { return cast_iterator(mybase::rend()); }
        const_reverse_iterator crend() const noexcept { return cast_iterator(mybase::crend()); }

        //
        // Capacity
        /////////////

        using mybase::empty;
        using mybase::size;
        using mybase::max_size;

        //
        // Modifiers
        //////////////

        using mybase::clear;

        std::pair<iterator, bool> insert(const value_type& value) { return cast_iterator(mybase::insert(value)); }

        std::pair<iterator, bool> insert(value_type&& value) { return cast_iterator(mybase::insert(std::move(value))); }

        template<typename P> requires std::is_constructible_v<value_type, P&&>
        std::pair<iterator, bool> insert(P&& value) { return emplace(std::forward<P>(value)); }

        iterator insert(const_iterator hint, const value_type& value) { return cast_iterator(mybase::insert(hint, value)); }

        iterator insert(const_iterator hint, value_type&& value) { return cast_iterator(mybase::insert(hint, std::move(value))); }

        template<typename P> requires std::is_constructible_v<value_type, P&&>
        std::pair<iterator, bool> insert(const_iterator hint, P&& value) { return emplace_hint(hint, std::forward<P>(value)); }

        template<typename InputIt>
        void insert(InputIt first, InputIt last) { mybase::insert(first, last); }

        template<typename... Args>
        std::pair<iterator, bool> try_emplace(const key_type& k, Args&&... args)
        { return try_emplace_impl(k, std::forward<Args>(args)...); }

        template<typename... Args>
        std::pair<iterator, bool> try_emplace(key_type&& k, Args&&... args)
        { return try_emplace_impl(std::move(k), std::forward<Args>(args)...); }

        template<typename... Args>
        iterator try_emplace(const_iterator hint, const key_type& k, Args&&... args)
        { return try_emplace_hint_impl(hint, k, std::forward<Args>(args)...); }

        template<typename... Args>
        iterator try_emplace(const_iterator hint, key_type&& k, Args&&... args)
        { return try_emplace_hint_impl(hint, std::move(k), std::forward<Args>(args)...); }

        iterator erase(const_iterator pos) { return cast_iterator(mybase::erase(pos)); }
        iterator erase(iterator pos) { return cast_iterator(mybase::erase(pos)); }
        iterator erase(const_iterator first, const_iterator last) { return cast_iterator(mybase::erase(first, last)); }
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

        size_type count(const key_type& key) const { return mybase::count(key); }

        iterator find(const key_type& key) { return cast_iterator(mybase::find(key)); }
        const_iterator find(const key_type& key) const { return cast_iterator(mybase::find(key)); }

        bool contains(const key_type& key) const { return mybase::contains(key); }

        std::pair<iterator, iterator> equal_range(const key_type& key) { return cast_iterator(mybase::equal_range(key)); }
        std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const { return cast_iterator(mybase::equal_range(key)); }

        iterator lower_bound(const key_type& key) { return cast_iterator(mybase::lower_bound(key)); }
        const_iterator lower_bound(const key_type& key) const { return cast_iterator(mybase::lower_bound(key)); }

        iterator upper_bound(const key_type& key) { return cast_iterator(mybase::upper_bound(key)); }
        const_iterator upper_bound(const key_type& key) const { return cast_iterator(mybase::upper_bound(key)); }
        
        // -- is_transparent

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        iterator find(const K& key) { return cast_iterator(mybase::find(key)); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        const_iterator find(const K& key) const { return cast_iterator(mybase::find(key)); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        size_type count(const K& key) const { return cast_iterator(mybase::count(key)); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        bool contains(const K& key) const { return cast_iterator(mybase::contains(key)); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        iterator lower_bound(const K& key) const
        { return cast_iterator(mybase::lower_bound(key)); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        const_iterator lower_bound(const K& key) const
        { return cast_iterator(mybase::lower_bound(key)); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        iterator upper_bound(const K& key) const
        { return cast_iterator(mybase::upper_bound(key)); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        const_iterator upper_bound(const K& key) const
        { return cast_iterator(mybase::upper_bound(key)); }

        //
        // Observers
        //////////////

        using mybase::key_comp;

    private:
        [[noreturn]] void throw_out_of_range() const { throw std::out_of_range("invalid flat_map subscript"); }
        static iterator cast_iterator(typename container_type::iterator iter) noexcept
        { return reinterpret_cast<const iterator&>(iter); }
        static const_iterator cast_iterator(typename container_type::const_iterator iter) noexcept
        { return reinterpret_cast<const const_iterator&>(iter); }
        static typename container_type::iterator cast_iterator(iterator iter) noexcept
        { return reinterpret_cast<const typename container_type::iterator&>(iter); }
        static typename container_type::const_iterator cast_iterator(const_iterator iter) noexcept
        { return reinterpret_cast<const typename container_type::const_iterator&>(iter); }
        static std::pair<iterator, iterator> cast_iterator(std::pair<typename container_type::iterator, typename container_type::iterator> iter) noexcept
        { return reinterpret_cast<const std::pair<iterator, iterator>&>(iter); }
        static std::pair<const_iterator, const_iterator> cast_iterator(std::pair<typename container_type::const_iterator, typename container_type::const_iterator> iter) noexcept
        { return reinterpret_cast<const std::pair<const_iterator, const_iterator>&>(iter); }
        static std::pair<iterator, bool> cast_iterator(std::pair<typename container_type::iterator, bool> iter) noexcept
        { return reinterpret_cast<const std::pair<iterator, bool>&>(iter); }

        template<typename K, typename... Args>
        void storage_emplace_back(K&& k, Args&&... args) {
            if constexpr (std::is_constructible_v<value_type, std::piecewise_construct_t, std::tuple<K&&>, std::tuple<Args&&...>>) {
                this->storage.emplace_back(std::piecewise_construct_t{},
                    std::forward_as_tuple(std::forward<K>(k)),
                    std::forward_as_tuple(std::forward<Args>(args)...));
            }
            else
                this->storage.emplace_back(std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
        }
        template<typename K, typename... Args>
        iterator storage_emplace_hint(const_iterator hint, K&& k, Args&&... args) {
            if constexpr (std::is_constructible_v<value_type, std::piecewise_construct_t, std::tuple<K&&>, std::tuple<Args&&...>>) {
                return cast_iterator(mybase::template emplace_hint(cast_iterator(hint), std::piecewise_construct_t{},
                    std::forward_as_tuple(std::forward<K>(k)),
                    std::forward_as_tuple(std::forward<Args>(args)...)));
            }
            else {
                return cast_iterator(mybase::template emplace_hint(cast_iterator(hint),
                    std::forward<K>(k),
                    mapped_type(std::forward<Args>(args)...)));
            }
        }

        template<typename K, typename... Args>
        std::pair<iterator, bool> try_emplace_impl(K&& k, Args&&... args) {
            auto lb = lower_bound(k); // key <= lb
            if (lb == end()) {
                storage_emplace_back(std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                return { std::prev(end()), true };
            }
            else if (key_comp()(k, *lb)) // key < lb
                return { storage_emplace_hint(lb, std::forward<K>(k), mapped_type(std::forward<Args>(args)...)), true };
            else
                return { lb, false }; // key == lb
        }

        template<typename K, typename... Args>
        iterator try_emplace_hint_impl(const_iterator hint, K&& k, Args&&... args) {
            assert(begin() <= hint && hint <= end());
            if (hint > begin()) {
                if (key_comp()(*std::prev(hint), k)) { // k > hint - 1
                    if (hint < end()) {
                        if (key_comp()(k, *hint)) // k < hint
                            return storage_emplace_hint(hint, std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                        else { // k >= hint
                            auto lb = std::lower_bound(hint, cend(), k, key_comp()); // k <= lb
                            if (lb == end()) {
                                storage_emplace_back(std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                                return std::prev(end());
                            }
                            else if (key_comp()(k, *lb)) // k < lb
                                return storage_emplace_hint(lb, std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                            else
                                return begin() + std::distance(cbegin(), lb); // k == lb
                        }
                    }
                    else { // hint == end()
                        storage_emplace_back(std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                        return std::prev(end());
                    }
                }
                else { // k <= hint - 1
                    auto lb = std::lower_bound(cbegin(), hint, k, key_comp()); // k <= lb
                    if (key_comp()(k, *lb)) // k < lb
                        return storage_emplace_hint(lb, std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                    else // k == lb
                        return begin() + std::distance(cbegin(), lb);
                }
            }
            else { // hint == begin()
                if (hint < end()) {
                    if (key_comp()(k, *hint)) // k < hint
                        return storage_emplace_hint(hint, std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                    else { // k >= hint
                        auto lb = std::lower_bound(hint, cend(), k, key_comp()); // k <= lb
                        if (lb == end()) {
                            storage_emplace_back(std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                            return std::prev(end());
                        }
                        else if (key_comp()(k, *lb)) // k < lb
                            return storage_emplace_hint(lb, std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                        else
                            return begin() + std::distance(cbegin(), lb); // k == lb
                    }
                }
                else { // empty
                    storage_emplace_back(std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                    return std::prev(end());
                }
            }
        }
    };

    template <typename Key, typename T, template<typename>class Vector, typename Compare>
    bool operator==(const flat_map<Key, T, Vector, Compare>& lhs, const flat_map<Key, T, Vector, Compare>& rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename Key, typename T, template<typename>class Vector, typename Compare>
    bool operator<(const flat_map<Key, T, Vector, Compare>& lhs, const flat_map<Key, T, Vector, Compare>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <typename Key, typename T, template<typename>class Vector, typename Compare>
    bool operator!=(const flat_map<Key, T, Vector, Compare>& lhs, const flat_map<Key, T, Vector, Compare>& rhs) {
        return !(lhs == rhs);
    }

    template <typename Key, typename T, template<typename>class Vector, typename Compare>
    bool operator>(const flat_map<Key, T, Vector, Compare>& lhs, const flat_map<Key, T, Vector, Compare>& rhs) {
        return rhs < lhs;
    }

    template <typename Key, typename T, template<typename>class Vector, typename Compare>
    bool operator<=(const flat_map<Key, T, Vector, Compare>& lhs, const flat_map<Key, T, Vector, Compare>& rhs) {
        return !(rhs < lhs);
    }

    template <typename Key, typename T, template<typename>class Vector, typename Compare>
    bool operator>=(const flat_map<Key, T, Vector, Compare>& lhs, const flat_map<Key, T, Vector, Compare>& rhs) {
        return !(lhs < rhs);
    }
}
