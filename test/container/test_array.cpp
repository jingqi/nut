
#include <nut/unittest/unittest.hpp>

#include <nut/container/array.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestArray)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_rcarray_smoking)
    NUT_CASE(test_rcarray_insert_erase)
    NUT_CASE(test_rcarray_ma)
    NUT_CASE(test_array_cow)
    NUT_CASES_END()

    void set_up() {}

    void tear_down() {}

    void test_rcarray_smoking()
    {
        RCArray<int> *rca = RCArray<int>::create();
        NUT_TA(rca->get_ref() == 1);
        NUT_TA(rca->size() == 0);

        rca->push_back(12);
        NUT_TA(rca->size() == 1);
        NUT_TA(rca->capacity() >= 1);
        NUT_TA(rca->at(0) == 12);

        rca->push_back(11);
        NUT_TA(rca->size() == 2);
        NUT_TA(rca->capacity() >= 2);
        NUT_TA(rca->at(0) == 12);
        NUT_TA(rca->at(1) == 11);

        NUT_TA(*rca->begin() == 12);
        NUT_TA(*(rca->end() - 1) == 11);
        NUT_TA(rca->data()[0] == 12);
        NUT_TA(rca->data()[1] == 11);

        rca->pop_back();
        NUT_TA(rca->size() == 1);
        NUT_TA(rca->at(0) == 12);

        RCArray<int> *rcb = rca->clone();
        NUT_TA(rca->get_ref() == 1 && rcb->get_ref() == 1);
        NUT_TA(rcb->size() == 1);
        NUT_TA(rca->operator ==(*rcb));
        NUT_TA(!(rca->operator !=(*rcb)));

        RCArray<int> *rcc = RCArray<int>::create();
        NUT_TA(rcc->size() == 0);
        NUT_TA(!(rcc->operator ==(*rca)));
        NUT_TA(rcc->operator !=(*rca));
        rcc->operator =(*rca);
        NUT_TA(rcc->size() == 1);
        NUT_TA(rcc->operator ==(*rca));

        rca->rls_ref();
        rcb->rls_ref();
        rcc->rls_ref();
    }

    void test_rcarray_insert_erase()
    {
        RCArray<int> *rca = RCArray<int>::create();
        rca->insert(0, 2);
        NUT_TA(rca->size() == 1);
        NUT_TA(rca->at(0) == 2);

        rca->insert(0,54);
        NUT_TA(rca->size() == 2);
        NUT_TA(rca->at(0) == 54 && rca->at(1) == 2);

        rca->insert(1, 85);
        NUT_TA(rca->size() == 3);
        NUT_TA(rca->at(0) == 54 && rca->at(1) == 85 && rca->at(2) == 2);

        rca->erase(1);
        NUT_TA(rca->size() == 2 && rca->at(0) == 54 && rca->at(1) == 2);

        rca->erase(0, 2);
        NUT_TA(rca->size() == 0);

        rca->push_back(12);
        rca->push_back(11);
        rca->clear();
        NUT_TA(rca->size() == 0 && rca->begin() == rca->end());

        RCArray<int> *rcb = RCArray<int>::create();
        rca->push_back(11);
        rca->push_back(12);
        rcb->push_back(21);
        rcb->push_back(22);
        rca->insert(1, rcb->begin(), rcb->end());
        NUT_TA(rca->size() == 4);
        NUT_TA(rca->at(0) == 11);
        NUT_TA(rca->at(1) == 21);
        NUT_TA(rca->at(2) == 22);
        NUT_TA(rca->at(3) == 12);

        rca->rls_ref();
        rcb->rls_ref();
    }

    void test_rcarray_ma()
    {
        sys_ma *ma = sys_ma::create();
        RCArray<int> *rca = RCArray<int>::create(0, ma);
        NUT_TA(ma->get_ref() == 2);
        rca->push_back(45);
        rca->push_back(46);

        rca->rls_ref();
        NUT_TA(ma->get_ref() == 1);
        ma->rls_ref();
    }

    void test_array_cow() // copy on write
    {
        Array<int> a;
        a.push_back(12);

        Array<int> b(a), c;
        c = a;
        NUT_TA(static_cast<const Array<int> >(a).data() == static_cast<const Array<int> >(b).data());
        NUT_TA(static_cast<const Array<int> >(a).data() == static_cast<const Array<int> >(c).data());
        a.push_back(12);
        NUT_TA(static_cast<const Array<int> >(a).data() != static_cast<const Array<int> >(b).data());
        NUT_TA(static_cast<const Array<int> >(b).data() == static_cast<const Array<int> >(c).data());
    }
};

NUT_REGISTER_FIXTURE(TestArray, "container, quiet")
