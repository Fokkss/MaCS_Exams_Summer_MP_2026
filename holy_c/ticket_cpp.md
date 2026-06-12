## Ticket 0

### cpp build + compile:

main.cpp
  ↓
preprocessor
  ↓
translation unit
  ↓
compiler
  ↓
object file main.o
  ↓
linker
  ↓
executable program
  ↓
OS loader
  ↓
process in memory
  ↓
CPU executes machine instructions

### some commands:
compile:
```shell 
g++ -std=c++17 -Wall -Wextra -Iinclude -c src/main.cpp -o build/main.o
```

- g++              компилятор C++
- -std=c++17       стандарт языка
- -Wall -Wextra    предупреждения
- -Iinclude        где искать header-файлы
- -c               только скомпилировать, не линковать
- src/main.cpp     входной файл
- -o  build/main.o  выходной object file
linking:
```g++ build/main.o build/Matrix.o -o build/matrix_app```

cmake build:

```shell
cmake -S . -B build
cmake --build build
./build/matrix_app
```

проверка ошибок:
```shell
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -g main.cpp
```

```
-Wall -Wextra       предупреждения
-Wshadow            ловит затенение имён
-g                  debug-информация
-fsanitize=address  ловит use-after-free, out-of-bounds, double delete
-fsanitize=undefined ловит часть UB
valgrind            поиск утечек и ошибок памяти
gdb/lldb            отладка по шагам
```

### other reminders:

int x = 10;

- x       сам объект int
- &x      адрес объекта x
- int* p  указатель, хранит адрес int
- *p      объект, на который указывает p
- int& r  ссылка, другое имя объекта

---
## Ticket 1 – Classes and objects

Объект – конкретный экземпляр класса в памяти

Example:

```C++
class Matrix
{
private:
    size_t rows_;
    size_t cols_;
    double* data_;

public:
    Matrix(size_t rows, size_t cols);
    ~Matrix();

    double& at(size_t row, size_t col);
    double at(size_t row, size_t col) const;
};
```
### Encapsulation

Инкапсуляция — это идея: **внутреннее устройство класса спрятано, наружу дан контролируемый интерфейс**

### class vs struct

- class – private, struct – public
### constructor

если полей несколько, они инициализируются через список инициализации; если поле пропущено — вызывается дефолтный конструктор; в теле конструктора все поля уже созданы и проинициализированы.

Ининциализация происходит в порядке объявления полей в классе

overload:

```C++
class Matrix
{
private:
    size_t rows_;
    size_t cols_;
    double* data_;

public:
    Matrix()
        : rows_(0)
        , cols_(0)
        , data_(nullptr)
    {
    }

    Matrix(size_t rows, size_t cols)
        : rows_(rows)
        , cols_(cols)
        , data_(new double[rows * cols])
    {
        fill(0.0);
    }

    Matrix(size_t size)
        : Matrix(size, size)
    {
    }

    void fill(double value)
    {
        for (size_t i = 0; i < rows_ * cols_; ++i)
        {
            data_[i] = value;
        }
    }
};
```

С C++11 можно писать значения по умолчанию прямо у полей
поля класса можно инициализировать прямо при объявлении, и эта инициализация встроится в любой конструктор, который явно не инициализирует эти поля

```C++
class Matrix
{
private:
    size_t rows_ = 0;
    size_t cols_ = 0;
    double* data_ = nullptr;

public:
    Matrix() = default;

    Matrix(size_t rows, size_t cols)
        : rows_(rows)
        , cols_(cols)
        , data_(new double[rows * cols])
    {
        fill(0.0);
    }

    ~Matrix()
    {
        delete[] data_;
    }

    void fill(double value)
    {
        for (size_t i = 0; i < rows_ * cols_; ++i)
        {
            data_[i] = value;
        }
    }
};
```

При объявлении какого-либо конструктора нельзя вызвать по-умолчанию, придется перегружать его через `=default`

### Construct chaining / delegation constructors

До C++11 часто приходилось дублировать код:

```C++
class Matrix
{
public:
    Matrix(size_t rows, size_t cols)
    {
        // большая инициализация
    }

    Matrix(size_t size)
    {
        // почти такая же большая инициализация
    }
};
```
С C++11 можно вызвать один конструктор из другого:
```C++
class Matrix
{
private:
    size_t rows_ = 0;
    size_t cols_ = 0;
    double* data_ = nullptr;

public:
    Matrix() = default;

    Matrix(size_t rows, size_t cols)
        : rows_(rows)
        , cols_(cols)
        , data_(new double[rows * cols])
    {
        fill(0.0);
    }

    Matrix(size_t size)
        : Matrix(size, size)
    {
    }

    void fill(double value)
    {
        for (size_t i = 0; i < rows_ * cols_; ++i)
        {
            data_[i] = value;
        }
    }
};
```
`Matrix(size)` делегирует работу `Matrix(size, size)`, в нем (делегат) нельзя инициализировать дополнительно поля.

Формы инициализации:
- Type x;        default initialization
- Type x{};      value initialization
- Type x(a, b);  direct initialization
- Type x = a;    copy initialization
- Type x{a, b};  list initialization

```C++
int a;      // мусорное значение
int b{};    // 0

Matrix m1;      // default initialization
Matrix m2{};    // value initialization
Matrix m3(2, 3); // direct initialization
Matrix m4{2, 3}; // list initialization

vector<int> a(10);
vector<int> b{10};

// a — vector из 10 нулей
// b — vector из одного элемента 10
```

Внутри нестатического метода есть скрытый парметр this – указатель на текущий объект

Если метод не меняет объект – делаем const:
```C++
class Matrix
{
private:
    size_t rows_ = 0;
    size_t cols_ = 0;
    double* data_ = nullptr;

public:
    size_t rows() const
    {
        return rows_;
    }

    size_t cols() const
    {
        return cols_;
    }

    double& at(size_t row, size_t col)
    {
        return data_[row * cols_ + col];
    }

    double at(size_t row, size_t col) const
    {
        return data_[row * cols_ + col];
    }
};
```

Почему два at?
```C++
Matrix a(2, 2);
a.at(0, 0) = 5.0;

const Matrix b(2, 2);
b.at(0, 0);      // читать можно
// b.at(0, 0) = 5.0;  // менять нельзя
```

Если деструктор вызвался во время обработки другого исключения, второе исключение из деструктора может привести к аварийному завершению программы.

Правильная идея: деструктор освобождает ресурс тихо, а ошибки закрытия обрабатываются отдельным методом.

--- 
## Ticket 2 – Работа с кучей в C++

### new/delete

### heap

```C++
int main()
{
    Matrix a(2, 2);
    Matrix* b = new Matrix(3, 3);

    delete b;
}
```

```stack:
+----------------+
| a              | объект Matrix
+----------------+
| b              | указатель Matrix*
+----------------+

heap:
+----------------+
| Matrix object  | объект, созданный через new
+----------------+
```

`a` уничтожится автоматически в конце блока.

`b` — это только указатель. Сам объект `*b` живёт в куче, пока ты явно не сделаешь delete

```C++
Matrix* matrix = new Matrix(2, 3);

matrix->at(0, 0) = 5.0;

delete matrix;
```
new:
1. выделяет память под Matrix
2. вызывает конструктор Matrix(2, 3)

delete:
1. вызывает деструктор ~Matrix()
2. освобождает память объекта

`malloc` выделит память, но не вызовет конструктор, а `free` не вызовет деструктор; поэтому для объектов C++ нужны `new/delete`

`new Image[10]` вызывает default constructor для каждого элемента.

`delete[] images` вызывает destructor для каждого элемента.

Если неправильно юзать – UB

### copy constructor

если его не сделать – для для каждого поля будет копия, из-за чего объекты будут одними и теми же, тогда при вызове delete будет double delete.

```C++
Matrix::Matrix(const Matrix& other)
    : rows_(other.rows_)
    , cols_(other.cols_)
    , data_(new double[other.rows_ * other.cols_])
{
    for (size_t i = 0; i < rows_ * cols_; ++i)
    {
        data_[i] = other.data_[i];
    }
}
```

### def operator оператор присваивания

```C++
Matrix a(2, 2);
Matrix b(3, 3);

b = a;
```
copy assignment oprator:
```C++
Matrix& Matrix::operator=(const Matrix& other)
{
    if (this == &other)
    {
        return *this;
    }

    delete[] data_;

    rows_ = other.rows_;
    cols_ = other.cols_;
    data_ = new double[rows_ * cols_];

    for (size_t i = 0; i < rows_ * cols_; ++i)
    {
        data_[i] = other.data_[i];
    }

    return *this;
}
```
чтобы new не бросил исключение:
```C++
Matrix& Matrix::operator=(const Matrix& other)
{
    if (this == &other)
    {
        return *this;
    }

    double* newData = new double[other.rows_ * other.cols_];

    for (size_t i = 0; i < other.rows_ * other.cols_; ++i)
    {
        newData[i] = other.data_[i];
    }

    delete[] data_;

    rows_ = other.rows_;
    cols_ = other.cols_;
    data_ = newData;

    return *this;
}
```

### C++ `=delete`

```C++
class FileHandle
{
private:
    FILE* file_ = nullptr;

public:
    FileHandle(const char* path)
    {
        file_ = fopen(path, "r");
    }

    ~FileHandle()
    {
        if (file_ != nullptr)
        {
            fclose(file_);
        }
    }

    FileHandle(const FileHandle& other) = delete;
    FileHandle& operator=(const FileHandle& other) = delete;
};
```
– запрет копирования

Потому что если два `FileHandle` будут владеть одним `FILE*`, оба в деструкторе вызовут `fclose`

до `C++=11:`
```C++
class FileHandle
{
private:
    FileHandle(const FileHandle& other);
    FileHandle& operator=(const FileHandle& other);
};
```
после `C++=11:`
```C++
FileHandle(const FileHandle& other) = delete;
FileHandle& operator=(const FileHandle& other) = delete;
```

`delete nullptr:`
```C++
double* p = nullptr;
delete[] p; // OK
```

утечка памяти:
```C++
void bad()
{
    Matrix* matrix = new Matrix(2, 2);

    throw runtime_error("error");

    delete matrix;
}
```
delete не выполнится, лучше без него, воспользовавшись RAII

возврат ссылки на локальную переменную:
```C++
int& bad()
{
    int x = 10;
    return x;
}
```
– в локальной памяти уничтожится x, ссылка стала висячей

висячий указатель:
```C++
int* p = new int(10);
delete p;

cout << *p << endl; // UB
```
– указатель есть, но объекта нет

```C++
double* data = new double[1000000000000];
```
– bad_alloc
### Rule of three (four actually)

Класс в ручную управляет ресурсом + имеет что то из копирования, деструткор $\Rightarrow$ нужны все три

Потому что:

- деструктор освобождает `data_`;
- значит, копирование по умолчанию опасно;
- значит, надо определить copy constructor;
- значит, надо определить copy assignment.

А современная цель — **Rule of Zero**: использовать `vector`, `string`, `unique_ptr`, чтобы не писать эти функции руками.

### copy elision

компилятор убирает лишнее копирование/перемещение объекта и создает его в нужном месте

с C++17 в некоторых случаях вообще не создает лишний временный объект
```C++
Matrix makeMatrix()
{
    Matrix result(2, 2);
    return result;
}

int main()
{
    Matrix a = makeMatrix();
}
```
без copy elision:
```
1. создать result внутри makeMatrix
2. скопировать result во временный объект возврата
3. скопировать временный объект в a
4. уничтожить лишние временные объекты
```
с:
создать Matrix сразу в памяти объекта a


---
## Ticket 3 – Inheritance and polymorhism

### Inheritance and poly

```C++
void drawAll(vector<Shape*>& shapes)
{
    for (size_t i = 0; i < shapes.size(); ++i)
    {
        shapes[i]->draw();
    }
}
```
– наследованные shapes, а т.к. функция работает для всех – полиморфизм

### virtual

static dispatch:
```C++
#include <iostream>

using namespace std;

class Shape
{
public:
    void draw() const
    {
        cout << "Drawing generic shape" << endl;
    }
};

class Circle : public Shape
{
public:
    void draw() const
    {
        cout << "Drawing circle" << endl;
    }
};

int main()
{
    Circle circle;
    Shape* shape = &circle;

    shape->draw();

    return 0;
}
```
– связывает с статическим типом указателя на shape, поэтому выведет его метод
`virtual:`
```C++
#include <iostream>

using namespace std;

class Shape
{
public:
    virtual void draw() const
    {
        cout << "Drawing generic shape" << endl;
    }
};

class Circle : public Shape
{
public:
    void draw() const override
    {
        cout << "Drawing circle" << endl;
    }
};

int main()
{
    Circle circle;
    Shape* shape = &circle;

    shape->draw();

    return 0;
}
```
– dynamic dispatch, работает через таблицу виртуальных функций

### protected

```
public     доступно всем
private    доступно только самому классу
protected  доступно самому классу и наследникам
```
лучше делать защищенные методы:
```C++
class Shape
{
private:
    double x_ = 0.0;
    double y_ = 0.0;

protected:
    double x() const
    {
        return x_;
    }

    double y() const
    {
        return y_;
    }
};
```

Circle лежит внутри shape, поэтому по стеку вызовов – констуркторы от объемлющего множества, а деструкторы – из самого внутреннего

```C++
virtual double area() const = 0;
```
– abstract func

если хочется удалить наследника через указатель – делать виртуальный деструктор:
```C++
Shape* shape = new Circle(0, 0, 5);
delete shape;

virtual ~Shape()
{
}
```

### virtual table

```C++
class Shape
{
public:
    virtual double area() const = 0;
    virtual void print(ostream& out) const = 0;
    virtual ~Shape()
    {
    }
};
```
Shape:
```vtable for Shape:
+----------------------+
| pointer to area      |
+----------------------+
| pointer to print     |
+----------------------+
| pointer to destructor|
+----------------------+
```
Circle:
```vtable for Circle:
+---------------------------+
| pointer to Circle::area   |
+---------------------------+
| pointer to Circle::print  |
+---------------------------+
| pointer to Circle::~Circle|
+---------------------------+
```

```
Circle object:
+------------------+
| vptr             | ----> vtable for Circle
+------------------+
| Shape::x_        |
+------------------+
| Shape::y_        |
+------------------+
| Circle::radius_  |
+------------------+
```

```C++
Shape* shape = new Circle(0, 0, 5);
shape->print(cout);
```
```under_the_hood
1. взять указатель shape
2. по нему найти объект Circle
3. из объекта прочитать hidden vptr
4. по vptr найти vtable for Circle
5. взять из таблицы адрес print
6. вызвать Circle::print
```
virtual call дороже обычного вызова, но дает полиморфизм

### static/dynamic dispatch
static:
```C++
class Shape
{
public:
    void print() const
    {
        cout << "Shape" << endl;
    }
};

class Circle : public Shape
{
public:
    void print() const
    {
        cout << "Circle" << endl;
    }
};

Circle c;
Shape* p = &c;
p->print();
```
-> Shape

Dynamic
```C++
class Shape
{
public:
    virtual void print() const
    {
        cout << "Shape" << endl;
    }
};

class Circle : public Shape
{
public:
    void print() const override
    {
        cout << "Circle" << endl;
    }
};

Circle c;
Shape* p = &c;
p->print();
```
-> Circle

### override

позволяет компилятору перепроверить, не virtual ли метод

overload: 
```C++
class Shape
{
public:
    virtual void scale(double factor)
    {
    }
};

class Circle : public Shape
{
public:
    void scale(int factor)
    {
    }
};
```

– это перегрузка, а не переопределение $\Rightarrow$
```C++
class Circle : public Shape
{
public:
    void scale(double factor) override
    {
    }
};
```

### Object slicing

```C++
class Shape
{
public:
    virtual void print() const
    {
        cout << "Shape" << endl;
    }
};

class Circle : public Shape
{
private:
    double radius_ = 5.0;

public:
    void print() const override
    {
        cout << "Circle" << endl;
    }
};

int main()
{
    Circle c;
    Shape s = c;

    s.print();

    return 0;
}
```

Здесь `s` — отдельный объект типа `Shape`.

В него скопировалась только базовая часть `Circle`.

Часть `Circle` с `radius_` отрезалась.

Это называется **object slicing**.

Вывод будет:

```
Shape
```

Чтобы сохранить полиморфизм, работаем через указатели или ссылки:

```
Shape& s = c;
s.print(); // Circle
```

или:

```
Shape* s = &c;
s->print(); // Circle
```

### corner cases:
```C++
class Shape
{
public:
    Shape()
    {
        print();
    }

    virtual void print() const
    {
        cout << "Shape" << endl;
    }
};

class Circle : public Shape
{
public:
    void print() const override
    {
        cout << "Circle" << endl;
    }
};
```
– при Circle c будет Shape, т.к.
Во время выполнения конструктора `Shape` часть `Circle` ещё не построена. Поэтому виртуальный вызов внутри конструктора базового класса не dispatch-ится в наследника.

Аналогично в деструкторе: когда выполняется деструктор базового класса, часть наследника уже разрушена.

Правило:

> Не полагайся на virtual dispatch в конструкторах и деструкторах.

### type cast
```C++
Circle* circle = new Circle(0, 0, 5);
Shape* shape = circle;
```
– safe, as it's implicit – **upcast** 
при downcast если не делать явного преобразование типа, что компилятор может не понять, там circle или rectangle

---
## Ticket 4 – smart pointers

**Умный указатель** — это объект, который внутри хранит обычный указатель и автоматически освобождает ресурс в своём деструкторе.

### scoped_ptr



