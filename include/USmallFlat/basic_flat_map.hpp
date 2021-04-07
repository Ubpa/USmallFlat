#pragma once

#include "details/flat_base_multimap.hpp"

namespace Ubpa {
    // require
    // - Vector<std::pair<const Key, T>>::iterator <=> Vector<std::pair<Key, T>>::iterator
    // - Vector<std::pair<const Key, T>>::const_iterator <=> Vector<std::pair<Key, T>>::const_iterator
    template <template<typename>class Vector, typename Key, typename T, typename Compare>
    class basic_flat_map : public details::flat_base_multimap<basic_flat_map<Vector, Key, T, Compare>, false, Vector, Key, T, Compare> {
        using mybase = details::flat_base_multimap<basic_flat_map<Vector, Key, T, Compare>, false, Vector, Key, T, Compare>;
    public:
        //////////////////
        // Member types //
        //////////////////

        using typename mybase::key_type;
        using typename mybase::mapped_type;
        using typename mybase::value_type;

        using typename mybase::iterator;
        using typename mybase::const_iterator;

        //////////////////////
        // Member functions //
        //////////////////////

        using mybase::mybase;

        //
        // Element access
        ///////////////////

        mapped_type& at(const key_type& key) {
            auto target = mybase::find(key);
            if (target == mybase::end())
                throw_out_of_range();
            return target->second;
        }

        const mapped_type& at(const key_type& key) const { return const_cast<basic_flat_map*>(this)->at(key); }

        mapped_type& operator[](const key_type& key) { return try_emplace(key).first->second; }
        mapped_type& operator[](key_type&& key) { return try_emplace(std::move(key)).first->second; }

        //
        // Modifiers
        //////////////

        template<typename M>
        std::pair<iterator, bool> insert_or_assign(const key_type& k, M&& m) { return insert_or_assign_impl(k, std::forward<M>(m)); }

        template<typename M>
        std::pair<iterator, bool> insert_or_assign(key_type&& k, M&& m) { return insert_or_assign_impl(std::move(k), std::forward<M>(m)); }

        template<typename M>
        iterator insert_or_assign(const_iterator hint, const key_type& k, M&& m) { return insert_or_assign_hint_impl(hint, k, std::forward<M>(m)); }

        template<typename M>
        iterator insert_or_assign(const_iterator hint, key_type&& k, M&& m) { return insert_or_assign_hint_impl(hint, std::move(k), std::forward<M>(m)); }

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

    private:
        [[noreturn]] void throw_out_of_range() const { throw std::out_of_range("invalid basic_flat_map subscript"); }

        template<typename K, typename... Args>
        void storage_emplace_back(K&& k, Args&&... args) {
            if constexpr (std::is_constructible_v<value_type, std::piecewise_construct_t, std::tuple<K&&>, std::tuple<Args&&...>>) {
                mybase::storage.emplace_back(std::piecewise_construct_t{},
                    std::forward_as_tuple(std::forward<K>(k)),
                    std::forward_as_tuple(std::forward<Args>(args)...));
            }
            else
                mybase::storage.emplace_back(std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
        }

        template<typename K, typename... Args>
        iterator storage_emplace(const_iterator hint, K&& k, Args&&... args) {
            if constexpr (std::is_constructible_v<value_type, std::piecewise_construct_t, std::tuple<K&&>, std::tuple<Args&&...>>) {
                return mybase::cast_iterator(mybase::storage.emplace(mybase::cast_iterator(hint), std::piecewise_construct_t{},
                    std::forward_as_tuple(std::forward<K>(k)),
                    std::forward_as_tuple(std::forward<Args>(args)...)));
            }
            else {
                return mybase::cast_iterator(mybase::storage.emplace(mybase::cast_iterator(hint),
                    std::forward<K>(k),
                    mapped_type(std::forward<Args>(args)...)));
            }
        }

        template<typename K, typename M>
        std::pair<iterator, bool> insert_or_assign_impl(K&& k, M&& m) {
            auto lb = mybase::lower_bound(k); // k <= lb
            if (lb == mybase::end() || mybase::key_comp()(k, *lb)) // k < lb
                return { mybase::cast_iterator(mybase::storage.insert(mybase::cast_iterator(lb), value_type(std::forward<K>(k), std::forward<M>(m)))), true };
            else { // k == lb
                lb->second = std::forward<M>(m);
                return { lb, false };
            }
        }

        template<typename K, typename M>
        iterator insert_or_assign_hint_impl(const_iterator hint, K&& k, M&& m) {
            assert(mybase::begin() <= hint && hint <= mybase::end());

            const_iterator first;
            const_iterator last;

            if (hint == mybase::begin() || mybase::key_comp()(*std::prev(hint), k)) { // k > hint - 1
                if (hint < mybase::end()) {
                    if (mybase::key_comp()(k, *hint)) // k < hint
                        return storage_emplace(hint, std::forward<K>(k), mapped_type(std::forward<M>(m)));
                    else { // k >= hint
                        first = hint;
                        last = mybase::cend();
                    }
                }
                else { // hint == end()
                    storage_emplace_back(std::forward<K>(k), mapped_type(std::forward<M>(m)));
                    return std::prev(mybase::end());
                }
            }
            else { // k <= hint - 1
                first = mybase::cbegin();
                last = hint;
            }

            auto lb = std::lower_bound(first, last, k, mybase::key_comp()); // value <= lb


            if (lb == mybase::end() || mybase::key_comp()(k, *lb)) // value < lb
                return storage_emplace(lb, std::forward<K>(k), mapped_type(std::forward<M>(m)));
            else { // value == lb
                auto iter = mybase::begin() + std::distance(mybase::cbegin(), lb);
                iter->second = std::forward<M>(m);
                return iter;
            }
        }

        template<typename K, typename... Args>
        std::pair<iterator, bool> try_emplace_impl(K&& k, Args&&... args) {
            auto lb = mybase::lower_bound(k); // key <= lb
            if (lb == mybase::end() || mybase::key_comp()(k, *lb)) // key < lb
                return { storage_emplace(lb, std::forward<K>(k), mapped_type(std::forward<Args>(args)...)), true };
            else
                return { lb, false }; // key == lb
        }

        template<typename K, typename... Args>
        iterator try_emplace_hint_impl(const_iterator hint, K&& k, Args&&... args) {
            assert(mybase::begin() <= hint && hint <= mybase::end());

            const_iterator first;
            const_iterator last;

            if (hint == mybase::begin() || mybase::key_comp()(*std::prev(hint), k)) { // k > hint - 1
                if (hint < mybase::end()) {
                    if (mybase::key_comp()(k, *hint)) // k < hint
                        return storage_emplace(hint, std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                    else { // k >= hint
                        first = hint;
                        last = mybase::cend();
                    }
                }
                else { // hint == end()
                    storage_emplace_back(std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
                    return std::prev(mybase::end());
                }
            }
            else { // k <= hint - 1
                first = mybase::cbegin();
                last = hint;
            }

            auto lb = std::lower_bound(first, last, k, mybase::key_comp()); // value <= lb


            if (lb == mybase::end() || mybase::key_comp()(k, *lb)) // value < lb
                return storage_emplace(lb, std::forward<K>(k), mapped_type(std::forward<Args>(args)...));
            else
                return mybase::begin() + std::distance(mybase::cbegin(), lb); // value == lb
        }
    };
}
