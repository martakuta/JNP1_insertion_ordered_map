#ifndef INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H

#include <iostream>
#include <memory>
#include <functional>

class lookup_error : std::exception {
    char const* what() const noexcept {
        return "lookup_error";
    }
};

template <class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:

    struct field;
    using f_ptr = std::shared_ptr<field>;
    using map_ptr = std::shared_ptr<f_ptr[]>;

    struct field {
        std::pair<K, V> mapping;
        //before and after are pointers to fields with the same hash
        f_ptr before;
        f_ptr after;
        //prev i next are pointers to fields in order od insertion
        f_ptr prev;
        f_ptr next;
    };

    size_t capacity = 16;
    size_t inside = 0;
    map_ptr map;
    f_ptr first = nullptr;
    f_ptr last = nullptr;
    bool sb_has_ref;

    //Daje wskaźnik na pole przechowujące wartość z kluczem k lub null jeśli taki
    //klucz nie jest przechowywany
    f_ptr find(K const &k) const {
        size_t hash = Hash{}(k) % capacity;

        f_ptr current_ptr = map[hash];
        while(current_ptr) {
            if (current_ptr->mapping.first == k)
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
        else
            first = next;

        next->prev = prev;

        last->next = field_ptr;
        field_ptr->next = nullptr;
        field_ptr->prev = last;

        last = field_ptr;
    }

    void extend_map() {
        clear_map();

        f_ptr old = first;
        f_ptr help = old;
        inside = 0;
        capacity = 2*capacity;
        map_ptr m(new f_ptr[capacity]);
        map = m;
        first = nullptr;
        last = nullptr;

        while (old != nullptr) {
            insert(old->mapping.first, old->mapping.second);

            //nie dealokuję pamięci pod field, bo zrobi to shared_ptr
            help = old->next;
            disconnect(old);
            old = help;
        }
    }

    void copy_map(const f_ptr& other_first, bool merge) {
        f_ptr act = other_first;
        /*std::cout << "copy map, use_count=" << map.use_count() << "\n";
        char a;
        std::cin >> a;*/

        while (act != nullptr) {
            //std::cout << "-----" << act->mapping.first << "\n";
            if (merge) {
                //std::cout << "merge\n";
                if (!contains(act->mapping.first)) {
                    insert(act->mapping.first, act->mapping.second);
                    //std::cout << "dodaje " << act->mapping.first << "-" << act->mapping.second << "\n";
                }
                else {
                    //std::cout << "already contain key " << act->mapping.first << "\n";
                }
            }
            else {
                //std::cout << "not merge\n";
                insert(act->mapping.first, act->mapping.second);
                //std::cout << "dodaje " << act->mapping.first << "-" << act->mapping.second << "\n";
            }
            act = act->next;
        }
        //insert has already set "first" and "last" fields
    }

    void copy_yourself_if_needed() {
        if (map.use_count() > 1) {
            f_ptr other_first = first;
            map_ptr m(new f_ptr[capacity]);
            map = m;
            first = nullptr;
            last = nullptr;
            inside = 0;
            sb_has_ref = false;
            copy_map(other_first, false);
        }
    }

    void clear_map() {
        if (map == nullptr)
            return;
        for(size_t i=0; i<capacity; i++) {
            while (map[i] != nullptr) {
                f_ptr help = map[i]->after;
                map[i] = nullptr;
                map[i] = help;
            }
        }
        map = nullptr;
    }

    void clear_all() {
        clear_map();

        f_ptr act = first, help = first;
        first = nullptr;
        last = nullptr;
        inside = 0;

        while (act != nullptr) {
            help = act->next;
            disconnect(act);
            act = help;
        }
    }

    void disconnect(f_ptr field_ptr) {
        field_ptr->before = nullptr;
        field_ptr->after = nullptr;
        field_ptr->prev = nullptr;
        field_ptr->next = nullptr;
    }


public:

    void print_map(std::string name) {
        std::cout << name << ": ";
        f_ptr act = first;

        while (act != nullptr) {
            std::cout << act->mapping.first << "-" << act->mapping.second << "*";
            act = act->next;
        }
        std::cout << "\n";
    }

    //konstruktor bezparametrowy - tworzy pusty słownik
    insertion_ordered_map()
            : map(new f_ptr[16])
            , sb_has_ref(false)
            {}

    //konstruktor kopiujący z semantyką copy-on-write
    insertion_ordered_map(const insertion_ordered_map& other) {
        capacity = other.capacity;
        inside = other.inside;
        sb_has_ref = false;

        if (other.sb_has_ref) {
            map_ptr m(new f_ptr[capacity]);
            map = m;
            copy_map(other.first, false);
        }
        else {
            map = other.map;
            first = other.first;
            last = other.last;
        }
    };

    //konstruktor przenoszący
    insertion_ordered_map(insertion_ordered_map &&other) noexcept 
            : map(other.map)
    {
        capacity = other.capacity;
        first = other.first;
        last = other.last;
        sb_has_ref = other.sb_has_ref;
        inside = other.inside;
        other.map = nullptr;
    }

    void swap(insertion_ordered_map& iom) noexcept
    {
        std::swap(this->map, iom.map);
        std::swap(this->first, iom.first);
        std::swap(this->last, iom.last);
        std::swap(this->capacity, iom.capacity);
        std::swap(this->inside, iom.inside);
        std::swap(this->sb_has_ref, iom.sb_has_ref);
    }

    //operator przypisania, przujmujący argument przez wartość
    insertion_ordered_map &operator=(insertion_ordered_map other) {
        insertion_ordered_map temp(other);
        temp.swap(*this);
        return *this;
    }

    //wstawianie do słownika
    bool insert(K const &k, V const &v) {

        copy_yourself_if_needed();

        f_ptr found = find(k);
        if(found != nullptr) {
            move_to_end(found);
            return false;
        }

        size_t hash = Hash{}(k) % capacity;
        f_ptr field_ptr = std::make_shared<field>();

        field_ptr->mapping = std::make_pair(k, v);
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
        sb_has_ref = false;

        if (inside > capacity*3/4)
            extend_map();

        return true;
    }


    //usuwanie ze słownika
    void erase(K const &k) {
        f_ptr to_remove = find(k);
        if (to_remove == nullptr) {
            throw lookup_error();
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
        sb_has_ref = false;
        //Czy teraz inteligentny wskaźnik zwolni pamięć na ten obiekt?
    }

    //scalanie słowników
    void merge(insertion_ordered_map const &other) {

        copy_yourself_if_needed();
        copy_map(other.first, true);
        sb_has_ref = false;
    }

    //zwracanie referencji na wartość pod kluczem
    V &at(K const &k) {
        f_ptr found = find(k);
        if (found == nullptr) {
            throw lookup_error();

        }
        sb_has_ref = true;
        return found->mapping.second;
    }

    V const &at(K const &k) const {
        f_ptr found = find(k);
        if (found == nullptr) {
            throw lookup_error();

        }
        sb_has_ref = true;
        return found->mapping.second;
    }

    V &operator[](K const &k) {
        copy_yourself_if_needed();
        f_ptr found = find(k);
        //sb_has_ref = true;

        if (found == nullptr) {
            insert(k, V());
        }
        found = find(k);
        return found->mapping.second;
    }

    //rozmiar słownika
    size_t size() const {
        return inside;
    }

    //sprawdzanie pustości słownika
    bool empty() const {
        return inside == 0;
    }

    ~insertion_ordered_map() {
        clear_all();
    }

    //usuwanie wszystkiego ze słownika
    void clear() {
        clear_all();
        capacity = 16;
        map_ptr m(new f_ptr[capacity]);
        map = m;
    }

    //sprawdzanie czy słownik zawiera element
    bool contains(K const &k) const {
        return find(k) != nullptr;
    }

    class iterator;

    iterator begin() const {
        return iterator(first);
    }

    iterator end() const {
        return iterator(nullptr);
    }

    class iterator {
    private:
        std::shared_ptr<field> current_field;

    public:

        iterator() noexcept:
                current_field(nullptr) {}

        iterator(const iterator &iterator) noexcept:
                current_field(iterator.current_field) {}

        //nie jestem pewna co do tego ecplicit, ale IDE mi podpowiada że musi ono być tutaj
        explicit iterator(const std::shared_ptr<field> field_ptr) noexcept:
                current_field(field_ptr) {}

        iterator& operator++() {
            if (current_field)
                current_field = current_field->next;
            return *this;
        }

        bool operator!=(const iterator& iterator) {
            return current_field != iterator.current_field;
        }

        bool operator==(const iterator& iterator) {
            return current_field == iterator.current_field;
        }

        const std::pair<K, V>& operator*() {
            return current_field->mapping;
        }

        const std::pair<K, V>* operator->() {
            return &(current_field->mapping);
        }

        iterator& operator=(iterator other) {
            this = other;
            return *this;
        }
    };
};

#endif //INSERTION_ORDERED_MAP_INSERTION_ORDERED_MAP_H

