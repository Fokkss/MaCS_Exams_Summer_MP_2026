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

## Ticket 2 – Работа с кучей в C++

### new/delete

