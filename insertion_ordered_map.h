#ifndef INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H

#include <iostream>
#include <memory>
#include <functional>

template <class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:
     
    struct field;
    using f_ptr = std::shared_ptr<field>;
    using tab_ptr = std::shared_ptr<f_ptr[]>;

    struct field {
        K key;
        V value;
        //before and after are pointers to fields with the same hash
        f_ptr before;
        f_ptr after;
        //prev i next are pointers to fields in order od insertion
        f_ptr prev;
        f_ptr next;
    };

    size_t capacity = 16;
    size_t inside = 0;
    tab_ptr map;
    f_ptr first = nullptr;
    f_ptr last = nullptr;
    bool sb_has_ref;

    //Daje wskaźnik na pole przechowujące wartość z kluczem k lub null jeśli taki
    //klucz nie jest przechowywany
    f_ptr find(K const &k) const {
        size_t hash = Hash{}(k) % capacity;

        f_ptr current_ptr = map[hash];
        while(current_ptr) {
            if (current_ptr->key == k)
                return current_ptr;
            current_ptr = current_ptr->after;
        }
        return nullptr;
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

    

    void extend_map() {
        f_ptr act = first, help = first;
        first = nullptr;
        last = nullptr;
        inside = 0;
        capacity = 2*capacity;
        std::shared_ptr<f_ptr[]> map(new f_ptr[capacity]);

        while (act != nullptr) {
            insert(act->key, act->value);

            //nie dealokuję pamięci pod field, bo zrobi to shared_ptr
            act->before = nullptr;
            act->after = nullptr;
            act->prev = nullptr;
            help = act->next;
            act->next = nullptr;
            act = nullptr;
            act = help;
        }
    }

    void copy_map(const insertion_ordered_map& other) {
        f_ptr act = other.first;

        while (act != nullptr) {
            insert(act->key, act->value);
            act = act->next;
        }
        //insert has already set "first" and "last" fields
    }

public:
/*
    using node_ptr = std::shared_ptr<node>;
    using tab_ptr = std::shared_ptr<node_ptr[]>;
    , tab(new node_ptr[16])
*/
    //konstruktor bezparametrowy - tworzy pusty słownik
    insertion_ordered_map()
        : map(new f_ptr[16])
        , sb_has_ref(false)
    {}

    //konstruktor kopiujący z semantyką copy-on-write
    insertion_ordered_map(const insertion_ordered_map& other)
        : capacity(other.capacity)
        , inside(other.inside)
        , sb_has_ref(false)
    {
        if (other.sb_has_ref) {
            std::shared_ptr<f_ptr[]> map(new f_ptr[capacity]);
            copy_map(other);
        } else {
            map = other.map;
            first = other.first;
            last = other.last;
        }
    };

    //konstruktor przenoszący
    insertion_ordered_map(insertion_ordered_map &&other) noexcept
        : capacity(other.capacity)
        , map(other.map)
        , first(other.first)
        , last(other.last)
        , sb_has_ref(other.sb_has_ref)
        , inside(other.inside)
    {
        other.map = nullptr;
    }
    //operator przypisania, przujmujący argument przez wartość
    insertion_ordered_map &operator=(insertion_ordered_map other) {
        this = other;
        return *this;
    }
    
    //wstawianie do słownika
    bool insert(K const &k, V const &v) {
          
        f_ptr found = find(k);
        if(found != nullptr) {
            move_to_end(found);
            return false;
        }

        size_t hash = Hash{}(k) % capacity;
        f_ptr field_ptr = std::make_shared<field>();
        
        field_ptr->key = k;
        field_ptr->value = v;
        field_ptr->after = map[hash];
        field_ptr->before = nullptr;
        field_ptr->next = nullptr;
        field_ptr->prev = last;
    
        if (last != nullptr)
        last->next = field_ptr;
        if (map[hash] != nullptr)
        map[hash]->before = field_ptr;

        last = field_ptr;
        if(first == nullptr)
            first = field_ptr;

        map[hash] = field_ptr;
        inside++;

        if (inside > capacity*3/4)
            extend_map();

        return true;    
    }
    
    
    //usuwanie ze słownika
    void erase(K const &k) {
        f_ptr to_remove = find(k);
        if (to_remove == nullptr) {
            //Podnieś wyjątek TODO
        }
        
        size_t hash = Hash{}(k) % capacity;
        f_ptr before = to_remove->before;
        f_ptr after = to_remove->after;
        f_ptr next = to_remove->next;
        f_ptr prev = to_remove->prev;
      
        if (last == to_remove)
            last = prev;
        if (first == to_remove)
            first = next;
        if (map[hash] == to_remove)
            map[hash] = after;
            
        if (before != nullptr)
            before->after = after;
        if (after != nullptr)
            after->before = before;
        if (next != nullptr)
            next->prev = prev;
        if (prev != nullptr)
            prev->next = next;

        inside--;
        //Czy teraz inteligętny wskaźnik zwolni pamięć na ten obiekt?
    }
          
    /* 
    //scalanie słowników
    void merge(insertion_ordered_map const &other);
    //zwracanie referencji na wartość pod kluczem
     */

    V &at(K const &k) {
        f_ptr found = find(k);
        if (found == nullptr) {
            //Rzuć wyjątek TODO
            
        }

        return found->value;
    }
     /*
    V const &at(K const &k) const;
    V &operator[](K const &k);
    */
    //rozmiar słownika
    size_t size() const {
        return inside;
    }
    
    //sprawdzanie pustości słownika
    bool empty() const {
        return inside == 0;
    }
    /*
    //usuwanie wszystkiego ze słownika
    void clear();
    */

    //sprawdzanie czy słownik zawiera element
    bool contains(K const &k) const {
        return find(k) != nullptr;
    }

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

        //nie jestem pewna co do tego ecplicit, ale IDE mi podpowiada że musi ono być tutaj
        explicit Iterator(const std::shared_ptr<field> field_ptr) noexcept:
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

        Iterator& operator=(Iterator other) {
            this = other;
            return *this;
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

#endif //INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H


