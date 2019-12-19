#ifndef INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H

#include <iostream>
#include <memory>

using std::shared_ptr;

template <class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:

    struct field {
        K key;
        V value;
        shared_ptr<field> before;
        shared_ptr<field> after;
        shared_ptr<field> prev;
        shared_ptr<field> next;
    };

    size_t capacity = 16;
    shared_ptr<field[]> map;
    shared_ptr<field> begin;
    shared_ptr<field> end;

public:

    //konstruktor bezparametrowy - tworzy pusty słownik
    insertion_ordered_map() {
        map = std::make_shared<field[capacity]>(nullptr);
    };

    //konstruktor kopiujący z semantyką copy-on-write
    insertion_ordered_map(const insertion_ordered_map& other) {
        this->capacity = other.capacity;
        this->map = other.map;
        this->begin = other.begin;
        this->end = other.end;
    };

    //konstruktor przenoszący
    insertion_ordered_map(insertion_ordered_map &&other) noexcept {
        this = std::move(other);
    }
    //operator przypisania, przujmujący argument przez wartość
    insertion_ordered_map &operator=(insertion_ordered_map other) {
        //TODO
    }
    //wstawianie do słownika
    bool insert(K const &k, V const &v);
    //usuwanie ze słownika
    void erase(K const &k);
    //scalanie słowników
    void merge(insertion_ordered_map const &other);
    //zwracanie referencji na wartość pod kluczem
    V &at(K const &k);
    V const &at(K const &k) const;
    V &operator[](K const &k);
    //rozmiar słownika
    size_t size() const {
        //TODO
    }
    //sprawdzanie pustości słownika
    bool empty() const {
        //TODO
    }
    //usuwanie wszystkiego ze słownika
    void clear();
    //sprawdzanie czy słownik zawiera element
    bool contains(K const &k) const;


    class Iterator;

    Iterator begin() {
        return Iterator(begin);
    }

    Iterator end() {
        return Iterator(nullptr);
    }

    class Iterator {
    private:
        std::shared_ptr<field> current_field;

    public:

        Iterator() noexcept:
                current_field(nullptr) {}

        Iterator(const Iterator &iterator) noexcept:
                current_field(iterator.current_field) {}

        Iterator(const std::shared_ptr<field> field_ptr) noexcept:
                current_field(field_ptr) {}

        Iterator& operator++() {
            if (current_field)
                current_field = currnet_field->next;
            return *this;
        }

        bool operator!=(const Iterator& iterator) {
            return current_field != iterator.current_field;
        }

        bool operator==(const Iterator& iterator) {
            return current_field == iterator.current_field;
        }

        const K& operator*() {
            return current_field->key;
        }

    };


};


template <class K, class V, class Hash>
bool insertion_ordered_map<K, V, Hash>::insert(K const& k, V const& v) {
    //TODO
}

template <class K, class V, class Hash>
void insertion_ordered_map<K, V, Hash>::erase(K const &k) {
    //TODO
}

template <class K, class V, class Hash>
void insertion_ordered_map<K, V, Hash>::merge(insertion_ordered_map const &other) {
    //TODO
}

template <class K, class V, class Hash>
V& insertion_ordered_map<K, V, Hash>::at(K const &k) {
    //TODO
}

template <class K, class V, class Hash>
V const& insertion_ordered_map<K, V, Hash>::at(K const &k) const {
    //TODO
}

template <class K, class V, class Hash>
V& insertion_ordered_map<K, V, Hash>::operator[](K const &k) {
    //TODO
}

template <class K, class V, class Hash>
void insertion_ordered_map<K, V, Hash>::clear() {
    //TODO
}

template <class K, class V, class Hash>
bool insertion_ordered_map<K, V, Hash>::contains(K const &k) const {
    //TODO
}

#endif //INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H