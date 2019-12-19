#ifndef INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H

#include <iostream>

template <class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:

    std::pair<K, V> tab[16] = new std::pair<K, V>[16];

public:

    //konstruktor bezparametrowy - tworzy pusty słownik
    insertion_ordered_map() {
        //TODO
    };
    //konstruktor kopiujący z semantyką copy-on-write
    insertion_ordered_map(const insertion_ordered_map& other) {
        //TODO
    };
    //konstruktor przenoszący
    insertion_ordered_map(insertion_ordered_map &&other) {
        //TODO
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

    //doesn't work
    class iterator {

        iterator end();

    public:


        std::pair<V, K>* begin() {
            return &tab[0];
        }
        std::pair<V, K>* end() {
            return nullptr;
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