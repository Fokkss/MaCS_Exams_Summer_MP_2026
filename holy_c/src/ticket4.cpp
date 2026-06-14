#include "ticket_lib4.hpp"

#include <iostream>
#include <memory>

using namespace std;

class File
{
private:
    string name_;

public:
    File(const string& name)
        : name_(name)
    {
        cout << "open " << name_ << endl;
    }

    ~File()
    {
        cout << "close " << name_ << endl;
    }

    void read() const
    {
        cout << "read from " << name_ << endl;
    }
};

int main()
{
    unique_ptr<File> first(new File("data.txt"));

    unique_ptr<File> second = std::move(first);

    if (!first)
    {
        cout << "first is empty" << endl;
    }

    second->read();

    return 0;
}
