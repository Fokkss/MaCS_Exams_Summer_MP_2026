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
– safe, as it's explicit – **upcast** 
при downcast если не делать явного преобразование типа, что компилятор может не понять, там circle или rectangle

---
## Ticket 4 – smart pointers

**Умный указатель** — это объект, который внутри хранит обычный указатель и автоматически освобождает ресурс в своём деструкторе.

```
unique_ptr<T>  ровно один владелец
shared_ptr<T>  несколько совместных владельцев
scoped_ptr<T>  один владелец внутри одного scope
```

### RAII

RAII — Resource Acquisition Is Initialization.

without:
```C++
void process()
{
    Matrix* matrix = new Matrix(100, 100);

    riskyFunction();

    delete matrix;
}
```

with:
```C++
void process()
{
    unique_ptr<Matrix> matrix(new Matrix(100, 100));

    riskyFunction();
}
```




### scoped_ptr

`scoped_ptr` нельзя копировать и нельзя передать владение
```
объект живёт ровно внутри одного scope
копировать нельзя
передавать владение нельзя
в деструкторе вызывает delete
```
похож на старый `boost::scoped_ptr`, не является стандартом, введен в качестве примера

```C++
#include <iostream>

using namespace std;

class Matrix
{
private:
    size_t rows_ = 0;
    size_t cols_ = 0;

public:
    Matrix(size_t rows, size_t cols)
        : rows_(rows)
        , cols_(cols)
    {
        cout << "Matrix constructor" << endl;
    }

    ~Matrix()
    {
        cout << "Matrix destructor" << endl;
    }

    void print() const
    {
        cout << "Matrix " << rows_ << "x" << cols_ << endl;
    }
};

class ScopedMatrixPtr
{
private:
    Matrix* ptr_ = nullptr;

public:
    explicit ScopedMatrixPtr(Matrix* ptr)
        : ptr_(ptr)
    {
    }

    ~ScopedMatrixPtr()
    {
        delete ptr_;
    }

    Matrix* get() const
    {
        return ptr_;
    }

    Matrix& operator*() const
    {
        return *ptr_;
    }

    Matrix* operator->() const
    {
        return ptr_;
    }

    explicit operator bool() const
    {
        return ptr_ != nullptr;
    }

    ScopedMatrixPtr(const ScopedMatrixPtr& other) = delete;
    ScopedMatrixPtr& operator=(const ScopedMatrixPtr& other) = delete;
};

int main()
{
    ScopedMatrixPtr matrix(new Matrix(2, 3));

    matrix->print();

    if (matrix)
    {
        cout << "matrix exists" << endl;
    }

    return 0;
}
```
```output
Matrix constructor
Matrix 2x3
matrix exists
Matrix destructor
```

разрешим копирование:
```
a.ptr_ ----+
           +----> один Matrix
b.ptr_ ----+
```

### unique_ptr

`unique_ptr<T>` — стандартный умный указатель с уникальным владением.

```
ровно один unique_ptr владеет объектом
копировать нельзя
перемещать можно
при уничтожении unique_ptr удаляет объект
```

```C++
#include <iostream>
#include <memory>

using namespace std;

class Matrix
{
public:
    Matrix()
    {
        cout << "Matrix constructor" << endl;
    }

    ~Matrix()
    {
        cout << "Matrix destructor" << endl;
    }

    void print() const
    {
        cout << "Matrix" << endl;
    }
};

int main()
{
    unique_ptr<Matrix> matrix(new Matrix());

    matrix->print();

    return 0;
}
```

Если надо передать владение – в сигнатуре пишем уникальный указатель, а при передаче – std::move, тогда после выполнения объект будет пустым:
```C++
void consumeMatrix(unique_ptr<Matrix> matrix)
{
    matrix->print();
}

unique_ptr<Matrix> matrix(new Matrix());

consumeMatrix(move(matrix));
```

C++<14:
```C++
unique_ptr<Matrix> createMatrix()
{
    unique_ptr<Matrix> matrix(new Matrix());

    return matrix;
}
```
C++14:
```C++
unique_ptr<Matrix> createMatrix()
{
    return make_unique<Matrix>();
}
```

`get()`
```C++
unique_ptr<Matrix> matrix(new Matrix(2, 2));

Matrix* raw = matrix.get();
```
`get()` возвращает сырой указатель, но **не отдаёт владение**.


```C++
unique_ptr<Matrix> matrix(new Matrix(2, 2));

Matrix* raw = matrix.release();
```
`release()` отдаёт сырой указатель и прекращает владение, придется самому delete для raw

```C++
unique_ptr<Matrix> matrix(new Matrix(2, 2));

matrix.reset(new Matrix(3, 3));
```
удалит объект и сделает указатель nullptr

### shared_ptr

`shared_ptr<T>` — умный указатель с совместным владением.

```
несколько shared_ptr могут владеть одним объектом
объект удаляется, когда исчезает последний shared_ptr
```

realization:
```C++
class SharedMatrixPtr
{
private:
    Matrix* ptr_ = nullptr;
    size_t* count_ = nullptr;

public:
    explicit SharedMatrixPtr(Matrix* ptr)
        : ptr_(ptr)
        , count_(new size_t(1))
    {
    }

    SharedMatrixPtr(const SharedMatrixPtr& other)
        : ptr_(other.ptr_)
        , count_(other.count_)
    {
        ++(*count_);
    }

    SharedMatrixPtr& operator=(const SharedMatrixPtr& other)
    {
        if (this == &other)
        {
            return *this;
        }

        release();

        ptr_ = other.ptr_;
        count_ = other.count_;
        ++(*count_);

        return *this;
    }

    ~SharedMatrixPtr()
    {
        release();
    }

    void release()
    {
        if (count_ == nullptr)
        {
            return;
        }

        --(*count_);

        if (*count_ == 0)
        {
            delete ptr_;
            delete count_;
        }

        ptr_ = nullptr;
        count_ = nullptr;
    }

    Matrix& operator*() const
    {
        return *ptr_;
    }

    Matrix* operator->() const
    {
        return ptr_;
    }

    size_t use_count() const
    {
        if (count_ == nullptr)
        {
            return 0;
        }

        return *count_;
    }
};
```

но настоящая сложнее:
```
поддерживает custom deleter
поддерживает weak_ptr
control block устроен сложнее
счётчики атомарны для многопоточности
есть aliasing constructor
```

```C++
#include <iostream>
#include <memory>

using namespace std;

class Texture
{
private:
    string name_;

public:
    Texture(const string& name)
        : name_(name)
    {
        cout << "load texture " << name_ << endl;
    }

    ~Texture()
    {
        cout << "destroy texture " << name_ << endl;
    }

    void bind() const
    {
        cout << "bind texture " << name_ << endl;
    }
};

int main()
{
    shared_ptr<Texture> grass(new Texture("grass.png"));

    {
        shared_ptr<Texture> sameGrass = grass;

        cout << "use_count = " << grass.use_count() << endl;

        sameGrass->bind();
    }

    cout << "use_count = " << grass.use_count() << endl;

    return 0;
}
```
```output
load texture grass.png
use_count = 2
bind texture grass.png
use_count = 1
destroy texture grass.png
```

```under_the_hood
shared_ptr object:
+--------------------------+
| pointer to T             |
+--------------------------+
| pointer to control block |
+--------------------------+

control block:
+------------------------+
| strong reference count |
+------------------------+
| weak reference count   |
+------------------------+
| deleter / allocator    |
+------------------------+
```

Лучше писать:
```C++
shared_ptr<Texture> texture = make_shared<Texture>("grass.png");
```

```
1. короче
2. exception-safe
3. часто делает одну аллокацию вместо двух:
   объект + control block рядом
```

Но у `shared_ptr` есть главная проблема: циклические ссылки.
### weak_ptr

```C++
class Node
{
public:
    shared_ptr<Node> next;
    shared_ptr<Node> prev;

    ~Node()
    {
        cout << "Node destructor" << endl;
    }
};

int main()
{
    shared_ptr<Node> a = make_shared<Node>();
    shared_ptr<Node> b = make_shared<Node>();

    a->next = b;
    b->prev = a;

    return 0;
}
```

```
a ссылается на b
b ссылается на a
счётчики не становятся 0
деструкторы не вызываются
```

```C++
class Node
{
public:
    shared_ptr<Node> next;
    weak_ptr<Node> prev;
};
```

### corner cases

```C++
class Widget
{
public:
    shared_ptr<Widget> getPtr()
    {
        return shared_ptr<Widget>(this);
    }
};
```
Почему плохо?

Если объект уже управляется `shared_ptr`, создание нового `shared_ptr` из `this` создаст новый control block.

Правильный способ — `enable_shared_from_this`.

---

## Ticket 5 – operators overload

нельзя перегружать:
```
.
.*
::
?:
sizeof
typeid
alignof
```

1. Нельзя придумать новый оператор.
2. Нельзя изменить приоритет оператора.
3. Нельзя изменить ассоциативность.
4. Нельзя изменить количество аргументов.
5. Хотя бы один аргумент перегруженного оператора должен быть пользовательского типа.
```C++
int operator+(int a, int b)
{
    return a - b;
}
```
– no

### Binary and unary operators

#### Binary: 

```C++
class Rational
{
private:
    int numerator_ = 0;
    int denominator_ = 1;

public:
    Rational operator+(const Rational& other) const
    {
        return Rational(
            numerator_ * other.denominator_ + other.numerator_ * denominator_,
            denominator_ * other.denominator_
        );
    }
};
```
```C++
a.operator+(b)
```
– левый this, понимает для a + b

```C++
Rational operator+(const Rational& a, const Rational& b)
{
    return Rational(
        a.numerator() * b.denominator() + b.numerator() * a.denominator(),
        a.denominator() * b.denominator()
    );
}
```
```C++
operator+(a, b)
```
– понимает для a + b

#### Unary:

```C++
-a
+a
!a
++a
a++
--a
*a
&a
```

```C++
class Rational
{
private:
    int numerator_ = 0;
    int denominator_ = 1;

public:
    Rational(int numerator, int denominator)
        : numerator_(numerator)
        , denominator_(denominator)
    {
        if (denominator_ == 0)
        {
            throw invalid_argument("zero denominator");
        }
    }

    Rational operator-() const
    {
        return Rational(-numerator_, denominator_);
    }
};
```

#### postfix/prefix
```C++
++x  → x.operator++()
x++  → x.operator++(0)
```
```
prefix ++x обычно возвращает T&
postfix x++ обычно возвращает старую копию T
```

### operator[]

```C++
class Array
{
private:
    vector<int> data_;

public:
    Array(size_t size)
        : data_(size)
    {
    }

    int& operator[](size_t index)
    {
        return data_[index];
    }

    const int& operator[](size_t index) const
    {
        return data_[index];
    }
};
```

приходится реализовывать два `operator[]` — const и non-const; неконстантный возвращает ссылку, чтобы можно было присваивать, а константный возвращает значение или `const`-ссылку.

```C++
Array a(10);
a[0] = 42;

const Array b(10);
cout << b[0] << endl;
```

C++17,23:
```C++
operator[](size_t row, size_t col)
operator() // equivalent
```

#### operator <<, >>
```C++
ostream& operator<<(ostream& out, const Matrix& matrix)
{
    for (size_t row = 0; row < matrix.rows(); ++row)
    {
        for (size_t col = 0; col < matrix.cols(); ++col)
        {
            out << matrix(row, col) << " ";
        }

        out << endl;
    }

    return out;
}
```
– свободная функция, не можем добавить метод в ostream
### In/out class

In:

```C++
class BigInt
{
public:
    BigInt operator+(const BigInt& other) const;
};
```
```C++
BigInt a = 10;
BigInt b = a + 3;
```
– works
```C++
BigInt c = 3 + a;
```
– no

Out:

```C++
BigInt operator+(const BigInt& a, const BigInt& b);
```
```C++
BigInt a = 10;

BigInt x = a + 3;
BigInt y = 3 + a;
```
– both work

Practical rule:
```
operator+=, -=, *=, /=       метод класса
operator+, -, *, /           свободная функция через +=
operator==, <, >             часто свободные функции
operator<<, >>               свободные функции
operator[]                   метод
operator()                   метод
operator->                   метод
operator=                    только метод
conversion operator          только метод
```

### Compound assignment
```C++
a += b;
a -= b;
a *= b;
```

```C++
class Matrix
{
public:
    Matrix& operator+=(const Matrix& other)
    {
        // меняем текущий объект
        return *this;
    }
};

Matrix operator+(Matrix left, const Matrix& right)
{
    left += right;
    return left;
}
```

```C++
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

class Matrix
{
private:
    size_t rows_ = 0;
    size_t cols_ = 0;
    vector<double> data_;

    size_t index(size_t row, size_t col) const
    {
        if (row >= rows_ || col >= cols_)
        {
            throw out_of_range("Matrix index is out of range");
        }

        return row * cols_ + col;
    }

public:
    Matrix() = default;

    Matrix(size_t rows, size_t cols)
        : rows_(rows)
        , cols_(cols)
        , data_(rows * cols, 0.0)
    {
    }

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
        return data_[index(row, col)];
    }

    double at(size_t row, size_t col) const
    {
        return data_[index(row, col)];
    }

    Matrix& operator+=(const Matrix& other)
    {
        if (rows_ != other.rows_ || cols_ != other.cols_)
        {
            throw invalid_argument("Matrix sizes are different");
        }

        for (size_t i = 0; i < data_.size(); ++i)
        {
            data_[i] += other.data_[i];
        }

        return *this;
    }

    void print(ostream& out) const
    {
        for (size_t row = 0; row < rows_; ++row)
        {
            for (size_t col = 0; col < cols_; ++col)
            {
                out << at(row, col) << " ";
            }

            out << endl;
        }
    }
};

Matrix operator+(Matrix left, const Matrix& right)
{
    left += right;
    return left;
}

int main()
{
    Matrix a(2, 2);
    Matrix b(2, 2);

    a.at(0, 0) = 1;
    a.at(0, 1) = 2;
    a.at(1, 0) = 3;
    a.at(1, 1) = 4;

    b.at(0, 0) = 10;
    b.at(0, 1) = 20;
    b.at(1, 0) = 30;
    b.at(1, 1) = 40;

    Matrix c = a + b;

    c.print(cout);

    return 0;
}
```

### friend

свободному оператору нужен доступ к private полям $\Rightarrow$ fiend

```C++
class Matrix
{
private:
    size_t rows_;
    size_t cols_;
    vector<double> data_;

public:
    friend ostream& operator<<(ostream& out, const Matrix& matrix);
};
```
`friend` означает:

> Эта функция не является методом класса, но имеет доступ к private/protected-членам класса.

ослабляет инкапсуляцию

#### operator <=>

C++20:
There’s a new three-way comparison operator, `<=>`. The expression `a <=> b` returns an object that compares `<0` if `a < b`, compares `>0` if `a > b`, and compares `==0` if `a` and `b` are equal/equivalent.

### type cast

#### constructor cast

```C++
class Rational
{
public:
    Rational(int value)
        : numerator_(value)
        , denominator_(1)
    {
    }
};

Rational r = 5;
```

запретить такое неявное преобразование:

```C++
class Rational
{
public:
    explicit Rational(int value)
        : numerator_(value)
        , denominator_(1)
    {
    }
};
```
```C++
Rational r(5);
Rational q = Rational(5);
```

type cast operator:
```C++
class Rational
{
private:
    int numerator_ = 0;
    int denominator_ = 1;

public:
    explicit operator double() const
    {
        return static_cast<double>(numerator_) / denominator_;
    }
};
```
```C++
Rational r(1, 2);

double x = static_cast<double>(r);
```

иначе можно было бы (что опасно):
```C++
double x = r;
```

#### explicit bool
```C++
class FileHandle
{
private:
    FILE* file_ = nullptr;

public:
    explicit operator bool() const
    {
        return file_ != nullptr;
    }
};

FileHandle file;

if (file)
{
    // файл открыт
}
```
but can not:
```C++
bool b = file; // ошибка, если operator bool explicit
```

до C++11 для такого поведения использовали странные трюки вроде safe bool idiom, а с C++11 появился `explicit operator bool()`

### corner cases

```C++
if (p != nullptr && p->valid())
{
}
```

лучше не перегружать, иначе можно нарушить short-circuit

### all operators that could be possibly overloaded on C++26

```operators
1.  operator new
2.  operator delete
3.  operator new[]
4.  operator delete[]

5.  operator co_await

6.  operator()
7.  operator[]
8.  operator->
9.  operator->*

10. operator~
11. operator!
12. operator+
13. operator-
14. operator*
15. operator/
16. operator%
17. operator^
18. operator&
19. operator|

20. operator=
21. operator+=
22. operator-=
23. operator*=
24. operator/=
25. operator%=
26. operator^=
27. operator&=
28. operator|=

29. operator==
30. operator!=
31. operator<
32. operator>
33. operator<=
34. operator>=
35. operator<=>

36. operator&&
37. operator||

38. operator<<
39. operator>>
40. operator<<=
41. operator>>=

42. operator++
43. operator--

44. operator,
```

Но если считать **формы**, будет больше, потому что некоторые операторы бывают и унарными, и бинарными. Стандарт отдельно говорит, что унарные и бинарные формы `+`, `-`, `*`, `&` могут быть перегружены.

couldn't be overloaded:
```operators
.
.*
::
?:
```

---

## Ticket 6 – misc

```
const                 обещание "не менять"
function overloading  одно имя, разные параметры
default arguments     часть аргументов можно не писать
static                сущность не привязана к конкретному объекту
```

### const

```C++
int x;
const int& ref = x;

ref = 20; // UB
x = 20; //OK
```

```C++
int a = 10;
int b = 20;

const int* p = &a;

// *p = 30; // нельзя
p = &b;     // можно
```

```C++
int a = 10;
int b = 20;

int* const p = &a;

*p = 30;    // можно
// p = &b;  // нельзя
```

```C++
void printMatrix(const Matrix& matrix)
{
    matrix.print(cout);
}
```
```C++
void bad(Matrix matrix);          // копирует
void good(const Matrix& matrix);  // не копирует и не меняет
```

константные методы можно вызывать только у константных, аналог для неконст

```C++
class Array
{
private:
    vector<int> data_;

public:
    int& at(size_t index)
    {
        return data_.at(index);
    }

    const int& at(size_t index) const
    {
        return data_.at(index);
    }
};
```
```C++
Array a;
a.at(0) = 42;

const Array b;
cout << b.at(0) << endl;
// b.at(0) = 42; // нельзя
```
```explain
неконстантный объект  → можно вернуть T&
константный объект    → можно вернуть const T&
```

### mutable

instrument for lazy loading, mutex, caches

```C++
class Text
{
private:
    string data_;
    mutable bool cached_ = false;
    mutable size_t cachedLength_ = 0;

public:
    explicit Text(const string& data)
        : data_(data)
    {
    }

    size_t length() const
    {
        if (!cached_)
        {
            cachedLength_ = data_.size();
            cached_ = true;
        }

        return cachedLength_;
    }
};
```

### functions overload

could be:
```C++
int maxValue(int x, int y)
{
    return x > y ? x : y;
}

int maxValue(int x, int y, int z)
{
    return maxValue(maxValue(x, y), z);
}

double maxValue(double x, double y)
{
    return x > y ? x : y;
}
```

error:
```C++
int get();
double get(); // ошибка
```
– function call doesn't give compiler any insstructions (signature is the same)
also error:
```C++
double x = get();
```

### Overload resolution

compiler picks by conversion ranking:
```
maxValue(int, int)
maxValue(int, int, int)
maxValue(double, double)
...
```
```C++
void f(int x)
{
    cout << "int" << endl;
}

void f(double x)
{
    cout << "double" << endl;
}

int main()
{
    f(10);    // int
    f(3.14);  // double
}
```

but:
```C++
void f(long x)
{
}

void f(double x)
{
}

int main()
{
    f(10); // может быть ambiguous в зависимости от набора overloads
}
```

list of ranks:
**1 (Lowest)**`bool
**2** signed char`, `unsigned char`, `char
**3** short int`, `unsigned short int
**4**int`, `unsigned int
**5**long int`, `unsigned long int
**6** (Highest)**`long long int`, `unsigned long long int`

### name mangling

```C++
int max(int x, int y);
int max(int x, int y, int z);
double max(double x, double y);
```

for example on linux:

```bash
readelf -s file.o
```

```shell
_Z3maxii
_Z3maxiii
_Z3maxdd
```

for C to exclude name mangling for compatibility with C ABI:
```C++
extern "C"
{
    void plugin_init();
}
```

### default arguments

error:
```C++
void f(int x = 10, int y);
```

OK – from right to left:
```C++
void f(int x, int y = 10);
void g(int x, int y = 10, int z = 20);
```

could be described in `.hpp`, and then u can write without them in `.cpp`, as compiler sets default values in a place of calling

**default arguments + overload = shit**

```C++
void log(int level, const string& message = "empty")
{
}

void log(int level)
{
}
```
```C++
log(1);
```
– wtf?

### static

#### static local variable

```C++
int nextId()
{
    static int id = 0;

    ++id;

    return id;
}
```
– static local variable lives between callings:
```C++
cout << nextId() << endl; // 1
cout << nextId() << endl; // 2
cout << nextId() << endl; // 3
```
```
id хранится не на стеке каждого вызова,
а в static storage duration области программы
```
– initializes ones.

C++>=11 static local variable initialization is thread-safe: 

#### static field

```C++
class Object
{
private:
    static size_t count_;
    string name_;

public:
    Object(const string& name)
        : name_(name)
    {
        ++count_;
    }

    ~Object()
    {
        --count_;
    }

    static size_t count()
    {
        return count_;
    }
};

size_t Object::count_ = 0;
```
– name_ is static field that is one for all class

C++11/14
static field is defined at .cpp cause memory is allocated at translation unit, otherwise – linking error:
```
undefined reference to Object::count_
```

C++>=17:
```C++
class Object
{
private:
    inline static size_t count_ = 0;
};
```
– could be defined at .hpp

#### static class method

```C++
class Object
{
private:
    static size_t count_;
    string name_;

public:
    static size_t count()
    {
        return count_;
    }
};
```
```explanation
не имеет this
не привязан к конкретному объекту
не может обращаться к нестатическим полям напрямую
```
– also one for the whole class

error:
```C++
static void printName()
{
    cout << name_ << endl; // ошибка
}
```

static methods call could be for all class or for one object of this class – callings are the same due to uniqueness of static method for one class

#### internal linkage

on file/namespace level is seen only in this translation unit

anonymus namespace:
```C++
namespace
{
    void helper()
    {
    }
}
```

### corner cases

```C++
const int x = 42;

int* p = const_cast<int*>(&x);

*p = 67; // UB
```

#### static object initialization order fiasco

order of initialization between global/static is not guaranteed:

bad:
```C++
// Logger.cpp
Logger globalLogger;

// Config.cpp
Config globalConfig(globalLogger);
```

solution:
```C++
Logger& logger()
{
    static Logger instance;
    return instance;
}
```

---

## Ticket 7 – inheritance: parts

### C-style approach

```
void*
размер элемента
function pointer
ручное приведение типов
```

```C++
#include <cstdlib>
#include <iostream>

using namespace std;

struct Point
{
    int x;
    int y;
};

int comparePoints(const void* left, const void* right)
{
    const Point* a = static_cast<const Point*>(left);
    const Point* b = static_cast<const Point*>(right);

    int distA = a->x * a->x + a->y * a->y;
    int distB = b->x * b->x + b->y * b->y;

    if (distA < distB)
    {
        return -1;
    }

    if (distA > distB)
    {
        return 1;
    }

    return 0;
}

int main()
{
    Point points[3] = {
        {3, 4},
        {1, 1},
        {10, 0}
    };

    qsort(points, 3, sizeof(Point), comparePoints);

    for (size_t i = 0; i < 3; ++i)
    {
        cout << points[i].x << " " << points[i].y << endl;
    }

    return 0;
}
```

```pros
1. Работает в C.
2. Можно использовать для разных типов.
3. Не требует наследования.
4. Можно сортировать даже примитивные типы.
```
```cons
compiler doesn't checks
void* стирает тип;
нужны ручные cast'ы;
легко ошибиться в sizeof;
function pointer обычно мешает inline;
нет нормальной работы с объектами C++ и исключениями;
сложнее писать безопасный код.
```
```C++
int numbers[3] = {3, 1, 2};

qsort(numbers, 3, sizeof(int), comparePoints);

// int (*)(const void*, const void*)
// => UB
```

### OOP-style approach

```C++
// interface
class Comparable
{
public:
    virtual int compare(const Comparable* other) const = 0;

    virtual ~Comparable()
    {
    }
};

// inherit
class Point : public Comparable
{
private:
    int x_ = 0;
    int y_ = 0;

public:
    Point(int x, int y)
        : x_(x)
        , y_(y)
    {
    }

    int compare(const Comparable* other) const override
    {
        const Point* point = static_cast<const Point*>(other);

        int thisDistance = x_ * x_ + y_ * y_;
        int otherDistance = point->x_ * point->x_ + point->y_ * point->y_;

        if (thisDistance < otherDistance)
        {
            return -1;
        }

        if (thisDistance > otherDistance)
        {
            return 1;
        }

        return 0;
    }
};

// sort
void sortComparable(Comparable** array, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = i + 1; j < size; ++j)
        {
            if (array[j]->compare(array[i]) < 0)
            {
                Comparable* tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }
    }
}

// usage
int main()
{
    Comparable* points[3];

    points[0] = new Point(3, 4);
    points[1] = new Point(1, 1);
    points[2] = new Point(10, 0);

    sortComparable(points, 3);

    for (size_t i = 0; i < 3; ++i)
    {
        delete points[i];
    }

    return 0;
}
```
```pros
1. Есть общий интерфейс.
2. Можно работать с разными наследниками единообразно.
3. Можно добавлять новые классы без переписывания sort.
4. Хорошо подходит для runtime polymorphism:
   GUI widgets, game entities, plugins, interfaces.
```
```cons
Нужно хранить объекты через указатели или ссылки.
```
```C++
Point points[10];
sortComparable(points, 10); // не то
```
```cons
Нужны virtual calls.

Virtual call труднее заинлайнить.

Примитивные типы неудобны.
Типобезопасность всё ещё неполная
```

### Template-style approach

```C++
template <typename T>
void sortArray(T* array, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = i + 1; j < size; ++j)
        {
            if (array[j] < array[i])
            {
                T tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }
    }
}
```
```C++
int numbers[3] = {3, 1, 2};
sortArray(numbers, 3);

double values[3] = {3.5, 1.5, 2.5};
sortArray(values, 3);
```
```C++
class Point
{
private:
    int x_ = 0;
    int y_ = 0;

public:
    Point() = default;

    Point(int x, int y)
        : x_(x)
        , y_(y)
    {
    }

    int distanceSquared() const
    {
        return x_ * x_ + y_ * y_;
    }

    bool operator<(const Point& other) const
    {
        return distanceSquared() < other.distanceSquared();
    }

    void print() const
    {
        cout << x_ << " " << y_ << endl;
    }
};

int main()
{
    Point points[3] = {
        Point(3, 4),
        Point(1, 1),
        Point(10, 0)
    };

    sortArray(points, 3);

    for (size_t i = 0; i < 3; ++i)
    {
        points[i].print();
    }

    return 0;
}
```
```pros
1. Типобезопасно.
2. Работает с примитивными типами.
3. Не требует базового класса.
4. Не требует virtual.
5. Компилятор может inline-ить comparator/operator<.
6. Часто быстрее runtime polymorphism.
7. Удобно для STL.
```
```cons
1. Код должен быть виден компилятору, поэтому шаблоны обычно в headers.
2. Ошибки компиляции могут быть длинными.
3. Может увеличивать размер бинарника из-за разных instantiations.
4. Не даёт runtime polymorphism сам по себе.
```

### comparison
```
C-style:
  универсальность через void*
  типы почти не проверяются
  ручные cast'ы
  function pointer
  может привести к UB
  C-compatible

OOP-style:
  универсальность через общий базовый класс
  runtime polymorphism
  virtual calls
  нужен pointer/reference
  удобно для интерфейсов и плагинов
  не идеально для примитивов и value-semantics

Template-style:
  универсальность через compile-time type parameter
  типобезопасность
  inline и оптимизация
  отлично для STL и алгоритмов
  код обычно в headers
  нет runtime polymorphism без дополнительных техник
```

### public/private/protected

```C++
class Derived : public Base
{
};

class Derived : protected Base
{
};

class Derived : private Base
{
};
```

по умолчанию private

#### public

```C++
class Shape
{
public:
    void move(double dx, double dy)
    {
    }

protected:
    double x_ = 0.0;
    double y_ = 0.0;

private:
    int internalId_ = 0;
};

class Circle : public Shape
{
public:
    void print()
    {
        cout << x_ << " " << y_ << endl; // OK
        // cout << internalId_ << endl; // ошибка
    }
};

int main()
{
    Circle circle;
    circle.move(1.0, 2.0); // OK, move остался public
}
```

```
public    → public в Derived
protected → protected в Derived
private   → недоступно напрямую в Derived
```

#### protected

```
public Base members    → protected в Derived
protected Base members → protected в Derived
private Base members   → недоступны напрямую
```

```C++
class Base
{
public:
    void foo()
    {
    }
};

class Derived : protected Base
{
};

int main()
{
    Derived d;

    // d.foo(); // ошибка: foo стал protected
}

class MoreDerived : public Derived
{
public:
    void bar()
    {
        foo(); // OK
    }
};
```

#### private

```
public Base members    → private в Derived
protected Base members → private в Derived
private Base members   → недоступны напрямую
```

```C++
class Engine
{
public:
    void start()
    {
        cout << "engine start" << endl;
    }
};

class Car : private Engine
{
public:
    void drive()
    {
        start();
        cout << "drive" << endl;
    }
};

int main()
{
    Car car;

    car.drive(); // OK
    // car.start(); // ошибка
}
```

composition is better:
```C++
class Car
{
private:
    Engine engine_;

public:
    void drive()
    {
        engine_.start();
        cout << "drive" << endl;
    }
};
```

```table
Base member      public inheritance   protected inheritance   private inheritance

public           public               protected               private
protected        protected            protected               private
private          недоступно напрямую   недоступно напрямую     недоступно напрямую
```
```further
public inheritance:
  Derived is a Base

private inheritance:
  Derived implemented in terms of Base

protected inheritance:
  почти как private, но доступ сохраняется для дальнейших наследников
```

### C++11: override/final

#### override

```C++
class Button
{
public:
    virtual void draw()
    {
    }
};

class ColoredButton : public Button
{
public:
    void draw() override
    {
    }
};
```

#### name hiding/ using

```C++
class Base
{
public:
    virtual void foo(int x)
    {
        cout << "Base int" << endl;
    }

    virtual void foo(double x)
    {
        cout << "Base double" << endl;
    }
};

class Derived : public Base
{
public:
    void foo(int x) override
    {
        cout << "Derived int" << endl;
    }
};

int main()
{
    Derived d;

    d.foo(10);   // Derived int
    d.foo(3.14); // неожиданно?
}
```

better:
```C++
class Derived : public Base
{
public:
    using Base::foo;

    void foo(int x) override
    {
        cout << "Derived int" << endl;
    }
};
```

#### final

```C++
class Token final
{
};

class SpecialToken : public Token
{
}; // UB
```

virtual-method:
```C++
class Base
{
public:
    virtual void run()
    {
    }
};

class Derived : public Base
{
public:
    void run() final
    {
    }
};

class MoreDerived : public Derived
{
public:
    // void run() override {} // ошибка
};
```

#### override final together

```C++
class Derived : public Base
{
public:
    void run() override final
    {
    }
};

void run() final override
{
}
```
```explanation
override: я переопределяю базовый virtual method
final: дальше переопределять нельзя
```

### public inheritance vs composition

bad:
```C++
class Vector
{
public:
    void push_back(int value)
    {
    }
};

class Stack : public Vector
{
};
```

good:
```C++
class Stack
{
private:
    Vector data_;

public:
    void push(int value)
    {
        data_.push_back(value);
    }

    void pop()
    {
        // ...
    }
};
```

public = Derived is a Base
```
Circle is a Shape
SavingsAccount is an Account
File is a Readable
File is a Writable
```

composition > private

### compile time vs run time polymorphism

runtime
```C++
class Shape
{
public:
    virtual double area() const = 0;

    virtual ~Shape()
    {
    }
};

double totalArea(const vector<unique_ptr<Shape>>& shapes)
{
    double result = 0.0;

    for (size_t i = 0; i < shapes.size(); ++i)
    {
        result += shapes[i]->area();
    }

    return result;
}
```
```pros
можно хранить разные типы в одной коллекции через Base*
можно загружать плагины
можно добавлять новые наследники без пересборки части кода
```
```cons
virtual call
heap/pointer/reference
сложнее ownership
object slicing risk
```

compile time
```C++
template <typename Shape>
double totalArea(const vector<Shape>& shapes)
{
    double result = 0.0;

    for (size_t i = 0; i < shapes.size(); ++i)
    {
        result += shapes[i].area();
    }

    return result;
}
```
```pros
нет virtual call
можно inline
работает value-semantics
типобезопасно
```
```cons
нельзя легко хранить Circle и Rectangle в одном vector<Shape>
код должен быть известен на compile-time
может раздувать binary
```

### C++20 – concepts

C++11/14/17 error:
```C++
template <typename T>
void printArea(const T& object)
{
    cout << object.area() << endl;
}
```

у int нет area()

C++20:
```C++
template <typename T>
concept HasArea = requires(const T& object)
{
    { object.area() };
};

template <HasArea T>
void printArea(const T& object)
{
    cout << object.area() << endl;
}
```

### corner cases

```C++
class Shape
{
public:
    virtual void draw()
    {
    }
};

class Circle : Shape
{
public:
    void draw() override
    {
    }
};
```
– private inherit $\Rightarrow$ error
```C++
Circle circle;
Shape* shape = &circle; // ошибка
```

```C++
class Base
{
public:
    ~Base()
    {
    }
};

class Derived : public Base
{
private:
    int* data_;

public:
    Derived()
        : data_(new int[100])
    {
    }

    ~Derived()
    {
        delete[] data_;
    }
};

Base* ptr = new Derived();
delete ptr; // UB
```

---

## Ticket 8 – inheritance

В лекции это прямо отмечено: при множественном наследовании приведение указателя к базовому классу может потребовать сдвинуть указатель.

```
MemoryFile object:
+----------------------+
| Readable subobject   |
|   vptr Readable      |
+----------------------+
| Writable subobject   |
|   vptr Writable      |
+----------------------+
| MemoryFile fields    |
|   data_              |
|   position_          |
+----------------------+
```
```C++
MemoryFile file;

MemoryFile* p0 = &file;
Readable* p1 = &file;
Writable* p2 = &file;

cout << p0 << endl;
cout << p1 << endl;
cout << p2 << endl;
```
```output
MemoryFile*  0x1000
Readable*    0x1000
Writable*    0x1008
```

### name conflicts

```C++
class Readable
{
protected:
    size_t totalBytes_ = 0;

public:
    size_t totalBytes() const
    {
        return totalBytes_;
    }
};

class Writable
{
protected:
    size_t totalBytes_ = 0;

public:
    size_t totalBytes() const
    {
        return totalBytes_;
    }
};

class File : public Readable, public Writable
{
};
```
– ambigous

use:
```C++
cout << file.Readable::totalBytes() << endl;
cout << file.Writable::totalBytes() << endl;
```
or:
```C++
class File : public Readable, public Writable
{
public:
    size_t bytesRead() const
    {
        return Readable::totalBytes_;
    }

    size_t bytesWritten() const
    {
        return Writable::totalBytes_;
    }
};
```

### interface inheritance

```C++
class User : public Hashable, public Comparable
{
private:
    int id_ = 0;
    string name_;

public:
    User(int id, const string& name)
        : id_(id)
        , name_(name)
    {
    }

    size_t hash() const override
    {
        return static_cast<size_t>(id_);
    }

    bool equals(const Comparable& other) const override
    {
        const User* user = dynamic_cast<const User*>(&other);

        if (user == nullptr)
        {
            return false;
        }

        return id_ == user->id_;
    }
};
```
– normal, Java and C# allows only this inheritance

#### Diamond problem

```C++
class IOBase
{
protected:
    size_t totalBytes_ = 0;
};

class Readable : public IOBase
{
};

class Writable : public IOBase
{
};

class File : public Readable, public Writable
{
};
```
```result
        IOBase
       /      \
 Readable    Writable
       \      /
        File
```
```problem
File содержит два IOBase-подобъекта:
1. IOBase внутри Readable
2. IOBase внутри Writable
```

если оба базовых класса сами наследуются от одного общего класса, возникает diamond problem; без виртуального наследования у обоих базовых классов будет своя копия подобъекта `IOBase`, значит в `File` будут два подобъекта `IOBase`

### virtual inheritance

one IOBase:
```C++
class Readable : public virtual IOBase
{
};

class Writable : public virtual IOBase
{
};
```

```C++
#include <iostream>

using namespace std;

class IOBase
{
protected:
    size_t totalBytes_ = 0;

public:
    void addBytes(size_t bytes)
    {
        totalBytes_ += bytes;
    }

    size_t totalBytes() const
    {
        return totalBytes_;
    }
};

class Readable : public virtual IOBase
{
public:
    void read()
    {
        addBytes(10);
    }
};

class Writable : public virtual IOBase
{
public:
    void write()
    {
        addBytes(20);
    }
};

class File : public Readable, public Writable
{
};

int main()
{
    File file;

    file.read();
    file.write();

    cout << file.totalBytes() << endl;

    return 0;
}
```
– diamond problem solution

without virtual:
```
File:
+---------------------------+
| Readable                  |
|   IOBase                  |
|     totalBytes_           |
+---------------------------+
| Writable                  |
|   IOBase                  |
|     totalBytes_           |
+---------------------------+
```
with virtual:
```
File:
+---------------------------+
| Readable                  |
|   pointer/offset to IOBase |
+---------------------------+
| Writable                  |
|   pointer/offset to IOBase |
+---------------------------+
| File fields               |
+---------------------------+
| shared IOBase             |
|   totalBytes_             |
+---------------------------+
```

#### virtual inheritance constructors

```C++
class IOBase
{
private:
    int flags_ = 0;

public:
    explicit IOBase(int flags)
        : flags_(flags)
    {
        cout << "IOBase" << endl;
    }
};

class Readable : public virtual IOBase
{
public:
    Readable()
        : IOBase(1)
    {
        cout << "Readable" << endl;
    }
};

class Writable : public virtual IOBase
{
public:
    Writable()
        : IOBase(2)
    {
        cout << "Writable" << endl;
    }
};

class File : public Readable, public Writable
{
public:
    File()
        : IOBase(3)
        , Readable()
        , Writable()
    {
        cout << "File" << endl;
    }
};
```

IOBase is constructed by File (the most derived class):
```C++
File()
    : IOBase(3)
```

порядок:
```
1. virtual bases, если есть
2. обычные base classes в порядке объявления в списке наследования
3. поля класса
4. тело конструктора
```

в общем случае приведение указателя на наследника к указателю на базовый класс — нетривиальная операция; при множественном наследовании может потребоваться сдвинуть указатель, а при виртуальном наследовании — прочитать из памяти адрес базового подобъекта.

### corner cases

```C++
Derived* derivedArray[10];

Base** baseArray = derivedArray; // нельзя, use by each element casting
```

один из нескольких типов:
```C++
variant<Circle, Rectangle, Triangle> shape;
```

---

## Ticket 9 – templates

### C-style

if to try C-preprocesses:
```
1. Макрос — это простая текстовая подстановка.
2. Препроцессор не знает типы.
3. Препроцессор не знает области видимости.
4. Ошибки хуже читаются.
5. Отладка хуже.
6. Можно легко получить странные конфликты имён.
```

### templates

```C++
#include <cstddef>
#include <stdexcept>
#include <iostream>

using namespace std;

template <typename T>
class Array
{
private:
    T* data_ = nullptr;
    size_t size_ = 0;

public:
    explicit Array(size_t size)
        : data_(new T[size])
        , size_(size)
    {
    }

    ~Array()
    {
        delete[] data_;
    }

    size_t size() const
    {
        return size_;
    }

    T& operator[](size_t index)
    {
        if (index >= size_)
        {
            throw out_of_range("Array index is out of range");
        }

        return data_[index];
    }

    const T& operator[](size_t index) const
    {
        if (index >= size_)
        {
            throw out_of_range("Array index is out of range");
        }

        return data_[index];
    }
};

int main()
{
    Array<int> numbers(3);

    numbers[0] = 10;
    numbers[1] = 20;
    numbers[2] = 30;

    cout << numbers[1] << endl;

    Array<string> words(2);

    words[0] = "hello";
    words[1] = "templates";

    cout << words[1] << endl;

    return 0;
}
```

then compiler do instancing – creating Array_* for each new object implemented

#### typename vs class

next are equivalent:
```C++
template <typename T>
class Array
{
};

template <class T>
class Array
{
};
```
, but can not do struct T

### template funcs, methods

inner:
```C++
template <typename T>
class Array
{
public:
    T& operator[](size_t index)
    {
        return data_[index];
    }
};
```

defined in, implemented out:
```C++
template <typename T>
class Array
{
private:
    T* data_ = nullptr;
    size_t size_ = 0;

public:
    explicit Array(size_t size);
    ~Array();

    T& operator[](size_t index);
    const T& operator[](size_t index) const;
};

template <typename T>
Array<T>::Array(size_t size)
    : data_(new T[size])
    , size_(size)
{
}

template <typename T>
Array<T>::~Array()
{
    delete[] data_;
}

template <typename T>
T& Array<T>::operator[](size_t index)
{
    if (index >= size_)
    {
        throw out_of_range("Array index is out of range");
    }

    return data_[index];
}

template <typename T>
const T& Array<T>::operator[](size_t index) const
{
    if (index >= size_)
    {
        throw out_of_range("Array index is out of range");
    }

    return data_[index];
}
```

if u want in .cpp:
```C++
// Array.cpp
#include "Array.h"

template <typename T>
Array<T>::Array(size_t size)
{
}

template class Array<int>;
template class Array<double>;
```

### template functions

swap:
```C++
template <typename T>
void mySwap(T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
}
```
```C++
int x = 10;
int y = 20;

mySwap<int>(x, y);
```

parameters:
```C++
template <typename T, typename U>
auto maxValue(const T& a, const U& b)
{
    if (a < b)
    {
        return b;
    }

    return a;
}
```

#### default params

```C++
template <typename T = double>
class Vector3
{
private:
    T x_ = {};
    T y_ = {};
    T z_ = {};

public:
    Vector3() = default;

    Vector3(const T& x, const T& y, const T& z)
        : x_(x)
        , y_(y)
        , z_(z)
    {
    }
};
```
```C++
Vector3<> a;       // Vector3<double>
Vector3<int> b;    // Vector3<int>
Vector3<float> c;  // Vector3<float>
```

#### non-type params
```C++
template <typename T, size_t N>
class StaticArray
{
private:
    T data_[N];

public:
    size_t size() const
    {
        return N;
    }

    T& operator[](size_t index)
    {
        return data_[index];
    }

    const T& operator[](size_t index) const
    {
        return data_[index];
    }
};
```
– size is in compile-time

```pros
не нужна heap allocation
размер известен компилятору
можно оптимизировать
объект сам содержит массив
```
```cons
размер нельзя поменять runtime
StaticArray<int, 10> и StaticArray<int, 20> — разные типы
```

C++11:
```types
целочисленные значения
enum
указатели/ссылки с external linkage
```
```nowadays
C++17: auto non-type template parameters
C++20: structural class types как NTTP в некоторых условиях
```
```C++
template <auto Value>
class Constant
{
public:
    static constexpr auto value = Value;
};

Constant<42> x;
Constant<'a'> y;
```

### template template

```C++
template <typename T, template <typename> class Container>
class Stack
{
private:
    Container<T> data_;

public:
    void push(const T& value)
    {
        data_.push_back(value);
    }

    bool empty() const
    {
        return data_.empty();
    }

    T pop()
    {
        T value = data_.back();
        data_.pop_back();
        return value;
    }
};
```
```C++
Stack<int, vector> stack;
```

### type aliases: typedef, using
standard alias:
```C++
using IntArray = Array<int>;

IntArray a(10); // = Array<int> a(10);
```

template alias:
```C++
template <typename T>
using Matrix = vector<vector<T>>;
```
```C++
Matrix<int> m;
Matrix<double> d;
```
– not a new type, псевдоним – nickname

### specialization

#### full
```C++
template <typename T>
class TypeName
{
public:
    static string name()
    {
        return "unknown";
    }
};
```

int:
```C++
template <>
class TypeName<int>
{
public:
    static string name()
    {
        return "int";
    }
};
```

double:
```C++
template <>
class TypeName<double>
{
public:
    static string name()
    {
        return "double";
    }
};
```

```C++
cout << TypeName<int>::name() << endl;    // int
cout << TypeName<char>::name() << endl;   // unknown
```

#### partial

```C++
template <typename T>
struct IsPointer
{
    static const bool value = false;
};

template <typename T>
struct IsPointer<T*>
{
    static const bool value = true;
};
```
```C++
cout << IsPointer<int>::value << endl;   // 0
cout << IsPointer<int*>::value << endl;  // 1
```

for class:
```C++
template <typename T>
class Array
{
public:
    static string kind()
    {
        return "Array<T>";
    }
};

template <typename T>
class Array<T*>
{
public:
    static string kind()
    {
        return "Array<T*>";
    }
};
```
```C++
cout << Array<int>::kind() << endl;   // Array<T>
cout << Array<int*>::kind() << endl;  // Array<T*>
```

partial for functions is prohibited:
```C++
template <typename T>
void print(T value)
{
}

template <typename T>
void print<T*>(T* value) // нельзя
{
}
```

use overload instead:
```C++
template <typename T>
void print(T value)
{
    cout << value << endl;
}

template <typename T>
void print(T* value)
{
    if (value == nullptr)
    {
        cout << "null" << endl;
    }
    else
    {
        cout << *value << endl;
    }
}
```

#### dependency names
```C++
template <typename Container>
void printFirst(const Container& container)
{
    typename Container::value_type x = container[0];

    cout << x << endl;
}
```

```C++
template <typename Parser>
void parse(Parser& parser)
{
    parser.template read<int>();
}
```

### C++14 – variable templates

```C++
template <typename T>
constexpr T pi = T(3.1415926535897932385);

double x = pi<double>;
float y = pi<float>;
```
```C++
is_integral<T>::value
is_integral_v<T> // same
```

### C++11 – variadic templates
```C++
template <typename... Args>
void printAll(const Args&... args)
{
    // C++17 fold expression:
    ((cout << args << " "), ...);
    cout << endl;
}
```

no fold expressions, so recursion:
```C++
void printAll()
{
    cout << endl;
}

template <typename T, typename... Args>
void printAll(const T& first, const Args&... rest)
{
    cout << first << " ";
    printAll(rest...);
}
```

C++17 – fold expressions:
```C++
template <typename... Args>
auto sum(const Args&... args)
{
    return (args + ...);
}
```

### CTAD – class template argument deduction
C++11:
```C++
pair<int, string> p(1, "one");
```
C++17:
```C++
pair p(1, string("one"));
vector v = {1, 2, 3};
```

### corner cases

```C++
template <typename T>
class Array
{
private:
    T* data_;

public:
    explicit Array(size_t size)
        : data_(new T[size])
    {
    }
};
```
– bad, new T[] must be default constructible

```C++
class NoDefault
{
public:
    explicit NoDefault(int value)
    {
    }
};
```
```C++
Array<NoDefault> a(10);
```
– wont compile, compile-time error

---

## Ticket 10 – exceptions

