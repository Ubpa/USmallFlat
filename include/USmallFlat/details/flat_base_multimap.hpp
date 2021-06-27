#pragma once

#include "flat_base_multiset.hpp"

#include <utility>
#include <tuple>
#include <stdexcept>

namespace Ubpa::details {
    template<typename Compare, bool = std::is_empty_v<Compare> && !std::is_final_v<Compare>>
    class flat_base_multimap_comp_storage : private Compare {
    public:
        template<typename T>
        constexpr flat_base_multimap_comp_storage(T&& t) noexcept(std::is_nothrow_constructible_v<Compare, T>) :
            Compare{ std::forward<T>(t) } {}

    protected:
        constexpr Compare& GetCompare() noexcept { return *this; }
        constexpr const Compare& GetCompare() const noexcept { return *this; }
    };

    template<typename Compare>
    class flat_base_multimap_comp_storage<Compare, false> {
    public:
        template<typename T>
        constexpr flat_base_multimap_comp_storage(T&& t) noexcept(std::is_nothrow_constructible_v<Compare, T>) :
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

    template<typename Key, typename T, typename Compare,
        bool value_as_base = std::is_empty_v<Compare> && !std::is_final_v<Compare>,
        bool = contain_is_transparent<Compare>>
    class flat_base_multimap_comp : public flat_base_multimap_comp_storage<Compare, value_as_base> {
        using storage = flat_base_multimap_comp_storage<Compare, value_as_base>;
        using MapValue = std::pair<const Key, T>;
        using SetValue = std::pair<Key, T>;
    public:
        using storage::storage;

        constexpr bool operator()(const MapValue& lhs, const MapValue& rhs) const {
            return this->GetCompare()(std::get<0>(lhs), std::get<0>(rhs));
        }
        constexpr bool operator()(const SetValue& lhs, const SetValue& rhs) const {
            return this->GetCompare()(std::get<0>(lhs), std::get<0>(rhs));
        }
        template <class K>
        constexpr bool operator()(const K& lhs, const MapValue& rhs) const {
            return this->GetCompare()(lhs, std::get<0>(rhs));
        }
        template <class K>
        constexpr bool operator()(const MapValue& lhs, const K& rhs) const {
            return this->GetCompare()(std::get<0>(lhs), rhs);
        }
        template <class K>
        constexpr bool operator()(const K& lhs, const SetValue& rhs) const {
            return this->GetCompare()(lhs, std::get<0>(rhs));
        }
        template <class K>
        constexpr bool operator()(const SetValue& lhs, const K& rhs) const {
            return this->GetCompare()(std::get<0>(lhs), rhs);
        }

        using is_transparent = int;
    };

    template<typename Key, typename T, typename Compare, bool value_as_base>
    class flat_base_multimap_comp<Key, T, Compare, value_as_base, false> : public flat_base_multimap_comp_storage<Compare, value_as_base> {
        using storage = flat_base_multimap_comp_storage<Compare, value_as_base>;
        using MapValue = std::pair<const Key, T>;
        using SetValue = std::pair<Key, T>;
    public:
        using storage::storage;

        constexpr bool operator()(const MapValue& lhs, const MapValue& rhs) const {
            return this->GetCompare()(std::get<0>(lhs), std::get<0>(rhs));
        }
        constexpr bool operator()(const SetValue& lhs, const SetValue& rhs) const {
            return this->GetCompare()(std::get<0>(lhs), std::get<0>(rhs));
        }
        constexpr bool operator()(const Key& lhs, const MapValue& rhs) const {
            return this->GetCompare()(lhs, std::get<0>(rhs));
        }
        constexpr bool operator()(const MapValue& lhs, const Key& rhs) const {
            return this->GetCompare()(std::get<0>(lhs), rhs);
        }
        constexpr bool operator()(const Key& lhs, const SetValue& rhs) const {
            return this->GetCompare()(lhs, std::get<0>(rhs));
        }
        constexpr bool operator()(const SetValue& lhs, const Key& rhs) const {
            return this->GetCompare()(std::get<0>(lhs), rhs);
        }

        using is_transparent = int;
    };

    // require
    // - Vector<std::pair<const Key, T>>::iterator <=> Vector<std::pair<Key, T>>::iterator
    // - Vector<std::pair<const Key, T>>::const_iterator <=> Vector<std::pair<Key, T>>::const_iterator
    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename T, typename Compare>
    class flat_base_multimap : protected flat_base_multiset<Impl, IsMulti, Vector, std::pair<Key, T>, flat_base_multimap_comp<Key, T, Compare>, Key> {
        friend class flat_base_multiset<Impl, IsMulti, Vector, std::pair<Key, T>, flat_base_multimap_comp<Key, T, Compare>, Key>;
        using mybase = flat_base_multiset<Impl, IsMulti, Vector, std::pair<Key, T>, flat_base_multimap_comp<Key, T, Compare>, Key>;
    public:
        //////////////////
        // Member types //
        //////////////////

        static constexpr bool is_multi = IsMulti;

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

        flat_base_multimap() : mybase(Compare()) {}

        explicit flat_base_multimap(const container_type& sorted_storage, const Compare& comp = Compare())
            : mybase(sorted_storage, comp) {}

        explicit flat_base_multimap(container_type&& sorted_storage, const Compare& comp = Compare())
            : mybase(std::move(sorted_storage), comp) {}

        explicit flat_base_multimap(const Compare& comp) : mybase(comp) {}

        template<typename Iter> requires std::input_iterator<Iter>
        flat_base_multimap(Iter first, Iter last, const Compare& comp = Compare()) : mybase(first, last, comp) {}

        flat_base_multimap(const flat_base_multimap&) = default;

        flat_base_multimap(flat_base_multimap&&) noexcept = default;

        flat_base_multimap(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
            : flat_base_multimap(ilist.begin(), ilist.end(), comp) {}

        flat_base_multimap& operator=(const flat_base_multimap&) = default;

        flat_base_multimap& operator=(flat_base_multimap&&) noexcept = default;

        flat_base_multimap& operator=(std::initializer_list<value_type> ilist) {
            mybase::operator=(ilist);
            return *this;
        }

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
        // Element access
        ///////////////////

        using mybase::data;
        using mybase::front;
        using mybase::back;

        //
        // Capacity
        /////////////

        using mybase::empty;
        using mybase::size;
        using mybase::max_size;
        using mybase::capacity;
        using mybase::shrink_to_fit;

        //
        // Modifiers
        //////////////

        using mybase::clear;

        std::pair<iterator, bool> insert(const value_type& value) { return cast_iterator(mybase::insert(value)); }

        std::pair<iterator, bool> insert(value_type&& value) { return cast_iterator(mybase::insert(std::move(value))); }

        template<typename P> requires std::is_constructible_v<value_type, P&&>
        std::pair<iterator, bool> insert(P&& value) { return emplace(std::forward<P>(value)); }

        iterator insert(const_iterator hint, const value_type& value) { return cast_iterator(mybase::insert(cast_iterator(hint), value)); }

        iterator insert(const_iterator hint, value_type&& value) { return cast_iterator(mybase::insert(cast_iterator(hint), std::move(value))); }

        template<typename P> requires std::is_constructible_v<value_type, P&&>
        std::pair<iterator, bool> insert(const_iterator hint, P&& value) { return emplace_hint(hint, std::forward<P>(value)); }

        // FIXME
        template<typename InputIt>
        void insert(InputIt first, InputIt last) { mybase::insert(first, last); }

        template<typename... Args>
        iterator emplace(Args&&... args) { return cast_iterator(mybase::emplace(std::forward<Args>(args)...).first); }

        template<typename... Args>
        iterator emplace_hint(const_iterator hint, Args&&... args)
        { return cast_iterator(mybase::emplace_hint(cast_iterator(hint), std::forward<Args>(args)...)); }

        iterator erase(const_iterator pos) { return cast_iterator(mybase::erase(cast_iterator(pos))); }
        iterator erase(iterator pos) { return cast_iterator(mybase::erase(cast_iterator(pos))); }
        iterator erase(const_iterator first, const_iterator last) { return cast_iterator(mybase::erase(cast_iterator(first), cast_iterator(last))); }
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
        size_type count(const K& key) const { return mybase::count(key); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        bool contains(const K& key) const { return mybase::contains(key); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        iterator lower_bound(const K& key)
        { return cast_iterator(mybase::lower_bound(key)); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        const_iterator lower_bound(const K& key) const
        { return cast_iterator(mybase::lower_bound(key)); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        iterator upper_bound(const K& key)
        { return cast_iterator(mybase::upper_bound(key)); }

        template<typename K,
            typename Comp_ = Compare, typename = std::enable_if_t<std::is_same_v<Comp_, Compare>, typename Comp_::is_transparent>>
        const_iterator upper_bound(const K& key) const
        { return cast_iterator(mybase::upper_bound(key)); }

        //
        // Observers
        //////////////

        using mybase::key_comp;

    protected:
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
    };

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename T, typename Compare>
    bool operator==(const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& lhs, const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& rhs) {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename T, typename Compare>
    bool operator<(const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& lhs, const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename T, typename Compare>
    bool operator!=(const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& lhs, const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& rhs) {
        return !(lhs == rhs);
    }

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename T, typename Compare>
    bool operator>(const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& lhs, const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& rhs) {
        return rhs < lhs;
    }

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename T, typename Compare>
    bool operator<=(const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& lhs, const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& rhs) {
        return !(rhs < lhs);
    }

    template <typename Impl, bool IsMulti, template<typename>class Vector, typename Key, typename T, typename Compare>
    bool operator>=(const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& lhs, const flat_base_multimap<Impl, IsMulti, Vector, Key, T, Compare>& rhs) {
        return !(lhs < rhs);
    }
}
