#include "ticket_lib9.hpp"

int main()
{
    Matrix3x3<int> a;
    Matrix3x3<int> b;

    a.fill(1);
    b.fill(2);

    a(0, 0) = 10;
    b(1, 1) = 20;

    Matrix3x3<int> c = a + b;

    c.print(cout);

    cout << "trace = " << trace(c) << endl;

    StaticMatrix<int, 2, 3> notSquare;

    // cout << trace(notSquare) << endl; // compile-time error

    return 0;
}
