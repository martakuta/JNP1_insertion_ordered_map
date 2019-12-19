#ifndef INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H

#include <iostream>
#include <memory>

template <class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:
     
    struct field;
    using f_ptr = std::shared_ptr<field>;

    struct field {
        K key;
        V value;
        f_ptr before;
        f_ptr after;
        f_ptr prev;
        f_ptr next;
    };

    size_t capacity = 16;
    std::shared_ptr<field[]> map;

    

    f_ptr first = nullptr;
    f_ptr last = nullptr;

    //Daje wskaźnik pole przechowujące wartość z kluczek k lub null jeśli taki
    //klucz nie jest przechowywany
    f_ptr find(K const &k) const {
        size_t hash = Hash(k);

        f_ptr current_ptr = map[hash];
        while(current_ptr) {
            if (current_ptr->key == k)
                return current_ptr;
            current_ptr = current_ptr->after;
        }
    }

    //przesuwa pole na które wskazuje wskaźnik na koniec listy iteratora
    void move_to_end(f_ptr field_ptr) {
        if (field_ptr->next == nullptr)
            return;
        f_ptr prev = field_ptr->prev;
        f_ptr next = field_ptr->next;
        
        if (prev != nullptr)
            prev->next = next;
        next->prev = prev;

        last->next = field_ptr;
        field_ptr->next = nullptr;
        field_ptr->prev = last;

        last = field_ptr;
    }    
    
public:

    //konstruktor bezparametrowy - tworzy pusty słownik
    insertion_ordered_map() {
        map = std::make_shared<field[capacity]>(nullptr);
    };

    //konstruktor kopiujący z semantyką copy-on-write
    insertion_ordered_map(const insertion_ordered_map& other) {
        this->capacity = other.capacity;
        this->map = other.map;
        this->first = other.first;
        this->end = other.end;
    };

    //konstruktor przenoszący
    insertion_ordered_map(insertion_ordered_map &&other) noexcept {
        this = std::move(other);
    }
   /* //operator przypisania, przujmujący argument przez wartość
    insertion_ordered_map &operator=(insertion_ordered_map other) {
        //TODO
    }*/

    
    //wstawianie do słownika
    bool insert(K const &k, V const &v) {
          
        f_ptr found = find(k);
        if(found != nullptr) {
            move_to_end(found);
            return false;
        }

        size_t hash = Hash(k) % capacity;
        f_ptr field_ptr = std::make_shared<field>();
        
        field_ptr->key = k;
        field_ptr->value = v;
        field_ptr->after = map[hash];
        field_ptr->before = nullptr;
        field_ptr->next = nullptr;
        field_ptr->prev = last;
    
        last->next = field_ptr;
        map[hash]->before = field_ptr;

        last = field_ptr;
        if(first == nullptr)
            first = field_ptr;

        map[hash] = field_ptr;
        return true;    
    }
    
    /*
    void erase(K const &k) 
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
    */

    class Iterator;

    Iterator begin() {
        return Iterator(first);
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
                current_field = current_field->next; 
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

/*
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
} */

int main() {}

#endif //INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H
