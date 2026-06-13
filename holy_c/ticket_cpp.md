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









