#include "insertion_ordered_map.h"
#include <cassert>
#include <vector>

namespace {
    auto f(insertion_ordered_map<int, int> q)
    {
        return q;
    }
}

int main()
{

    int keys[] = {3, 1, 2};

    insertion_ordered_map<int, int> iom1 = f({});

    for (int i = 0; i < 3; ++i) {
        iom1[keys[i]] = i;
    }
    auto &ref = iom1[3];

    insertion_ordered_map<int, int> iom2(iom1); // Wykonuje się pełna kopia. Dlaczego?
    insertion_ordered_map<int, int> iom3;
    iom3 = iom2;

    ref = 10;
    assert(iom1[3] == 10);
    assert(iom2[3] != 10);

    iom2.erase(3); // Przy wyłączonych asercjach obiekt iom2 tutaj dokonuje kopii i przestaje współdzielić dane z iom3.
    assert(iom2.size() == 2);
    assert(!iom2.contains(3));
    assert(iom2.contains(2));

    assert(iom3.size() == 3);
    assert(iom3.contains(3));

    iom2.insert(4, 10);
    iom2.insert(1, 10);
    assert(iom2.size() == 3);
    insertion_ordered_map<int, int> const iom4 = iom2;
    {
        int order[] = {2, 4, 1};
        int values[] = {2, 10, 1};
        int i = 0;
        for (auto it = iom2.begin(), end = iom2.end(); it != end; ++it, ++i)
            assert(it->first == order[i] && it->second == values[i]);
        i = 0;
        for (auto it = iom4.begin(), end = iom4.end(); it != end; ++it, ++i)
            assert(it->first == order[i] && it->second == values[i]);
    }

    auto piom5 = std::make_unique<insertion_ordered_map<int, int>>();
    piom5->insert(4, 0);
    assert(piom5->at(4) == 0);
    auto iom6(*piom5);
    piom5.reset();
    assert(iom6.at(4) == 0);
    iom6[5] = 5;
    iom6[6] = 6;

    iom2.merge(iom6);
    {
        int order[] = {2, 4, 1, 5, 6};
        int values[] = {2, 10, 1, 5, 6};
        int i = 0;
        for (auto it = iom4.begin(), end = iom4.end(); it != end; ++it, ++i)
            assert(it->first == order[i] && it->second == values[i]);
    }

    std::swap(iom1, iom2);
    std::vector<insertion_ordered_map<int, int>> vec;
    for (int i = 0; i < 100000; i++) {
        iom1.insert(i, i);
    }
    for (int i = 0; i < 1000000; i++) {
        vec.push_back(iom1);  // Wszystkie obiekty w vec współdzielą dane.
    }

    //==========================================================================

    insertion_ordered_map<int, int> n;
    for(int i = 1; i <=100; i++) {
        n.insert(i, 2 * i);
    }

    n.print_map("n");

    for(auto i = n.begin(); i != n.end(); ++i)
        std::cout<<n.at(i->first)<<" ";


    insertion_ordered_map<int, int> moje_iom = f({});
    insertion_ordered_map<int, int> moje_iom2(moje_iom);
    assert(moje_iom.insert(2,12));
    assert(moje_iom.insert(1,11));
    assert(moje_iom2.insert(5,15));
    assert(moje_iom2.insert(1,13));
    moje_iom.merge(moje_iom2); // moje_iom: 1-11, 2-12 + moje_iom2: 1-13, 5-15
    insertion_ordered_map<int, int> moje_iom3 = {};
    assert(moje_iom3.insert(1,1));
    //moje_iom3.merge(moje_iom); //moje_iom3: 1-1 + moje_iom: 1-11, 2-12, 5-15
    insertion_ordered_map<int, int> moje_iom4(moje_iom3);

    moje_iom.print_map("moje_iom");
    moje_iom2.print_map("moje_iom2");
    moje_iom3.print_map("moje_iom3");
    moje_iom4.print_map("moje_iom4");

    //insertion_ordered_map<int, int> moje_iom5(std::move(moje_iom4));
    //moje_iom5.print_map("moje_iom5");
    //moje_iom4.print_map("moje_iom4");

    moje_iom3.merge(moje_iom);
    moje_iom.print_map("moje_iom");
    moje_iom3.print_map("moje_iom3");
    moje_iom4.print_map("moje_iom4");

    insertion_ordered_map<double, std::string> moje_iom_napisy = {};
    assert(moje_iom_napisy.insert(1.5,"abrakadabra"));
    std::cout << moje_iom_napisy[1.5] << "*" << moje_iom_napisy[3.14] << "*" << moje_iom_napisy[1.5] << "*\n";

    return 0;
}

