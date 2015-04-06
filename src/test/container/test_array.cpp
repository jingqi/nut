
#include <nut/unittest/unittest.h>

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
        rc_ptr<RCArray<int> > rca = RC_NEW(NULL, RCArray<int>);
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

        rc_ptr<RCArray<int> > rcb = rca->clone();
        NUT_TA(rca->get_ref() == 1 && rcb->get_ref() == 1);
        NUT_TA(rcb->size() == 1);
        NUT_TA(rca->operator ==(*rcb));
        NUT_TA(!(rca->operator !=(*rcb)));

        rc_ptr<RCArray<int> > rcc = RC_NEW(NULL, RCArray<int>);
        NUT_TA(rcc->size() == 0);
        NUT_TA(!(rcc->operator ==(*rca)));
        NUT_TA(rcc->operator !=(*rca));
        rcc->operator =(*rca);
        NUT_TA(rcc->size() == 1);
        NUT_TA(rcc->operator ==(*rca));
    }

    void test_rcarray_insert_erase()
    {
        rc_ptr<RCArray<int> > rca = RC_NEW(NULL, RCArray<int>);
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

        rc_ptr<RCArray<int> > rcb = RC_NEW(NULL, RCArray<int>);
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
    }

    void test_rcarray_ma()
    {
        rc_ptr<sys_ma> ma = RC_NEW(NULL, sys_ma);
        rc_ptr<RCArray<int> > rca = RC_NEW(NULL, RCArray<int>, 0, ma.pointer());
        NUT_TA(ma->get_ref() == 2);
        rca->push_back(45);
        rca->push_back(46);

        rca.set_null();
        NUT_TA(ma->get_ref() == 1);
    }

    void test_array_cow() // copy on write
    {
        Array<int> a;
        a.push_back(12);

        Array<int> b(a), c;
        c = a;
        NUT_TA(static_cast<const Array<int>& >(a).data() == static_cast<const Array<int>& >(b).data());
        NUT_TA(static_cast<const Array<int>& >(a).data() == static_cast<const Array<int>& >(c).data());
        a.push_back(12);
        NUT_TA(static_cast<const Array<int>& >(a).data() != static_cast<const Array<int>& >(b).data());
        NUT_TA(static_cast<const Array<int>& >(b).data() == static_cast<const Array<int>& >(c).data());
    }
};

NUT_REGISTER_FIXTURE(TestArray, "container, quiet")
