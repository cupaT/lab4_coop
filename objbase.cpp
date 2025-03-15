#include <iostream>
#include <unknwn.h>
#include "objbase.h"
#include <cassert>

using namespace std;

// Интерфейсы
interface IX : IUnknown {
    virtual void _stdcall Fx() = 0;
};

interface IY : IUnknown {
    virtual void _stdcall Fy() = 0;
};

interface IZ : IUnknown {
    virtual void _stdcall Fz() = 0;
};

// Компонент
class CA : public IX, public IY {
public:
    // Реализация методов класса IUnknown
    virtual ULONG _stdcall AddRef() { return 0; };
    virtual ULONG _stdcall Release() { return 0; };

    virtual HRESULT _stdcall QueryInterface(const IID& iid, void** ppv) {
        if (iid == IID_IUnknown1) {
            cout << "QueryInterface: возвращаю указатель на IUnknown" << endl;
            *ppv = static_cast<IX*>(this);
        }
        else if (iid == IID_IX) {
            cout << "QueryInterface: возвращаю указатель на IX" << endl;
            *ppv = static_cast<IX*>(this);
        }
        else if (iid == IID_IY) {
            cout << "QueryInterface: возвращаю указатель на IY" << endl;
            *ppv = static_cast<IY*>(this);
        }
        else {
            cout << "Интерфейс не поддерживается" << endl;
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        reinterpret_cast<IUnknown*>(*ppv)->AddRef();
        return S_OK;
    }

    // Реализация интерфейса IX
    virtual void _stdcall Fx() { std::cout << "CA::Fx" << std::endl; };

    // Реализация интерфейса IY
    virtual void _stdcall Fy() { std::cout << "CA::Fy" << std::endl; };

};

// // Функция создания компонента
IUnknown* CreateInstance() {
    IUnknown* pI = static_cast<IX*>(new CA);
    pI->AddRef();
    return pI;
}

BOOL SameComponents(IX* pIX, IY* pIY)
{
    IUnknown* pI1 = NULL;
    IUnknown* pI2 = NULL;

    // получить указатели на IUnknown через pIX
    pIX->QueryInterface(IID_IUnknown1, (void**)&pI1);

    // получить указатели на IUnknown через pIY
    pIY->QueryInterface(IID_IUnknown1, (void**)&pI2);

    // сравнить полученные указатели
    return pI1 == pI2;
}

void f(IX* pIX)
{
    IX* pIX2 = NULL;

    // Запросить IX через IX
    HRESULT hr = pIX->QueryInterface(IID_IX, (void**)&pIX2);

    assert(SUCCEEDED(hr)); // Запрос должен быть успешным
}

void f2(IX* pIX)
{
    HRESULT hr;

    IX* pIX2 = NULL;
    IY* pIY = NULL;

    // Получить IY через IX
    hr = pIX->QueryInterface(IID_IY, (void**)&pIY);

    if (SUCCEEDED(hr))
    {
        // Получить IX через IY
        hr = pIY->QueryInterface(IID_IX, (void**)&pIX2);

        // QueryInterface должна отработать успешно
        assert(SUCCEEDED(hr));
    }
}

void f3(IX* pIX)
{
    HRESULT hr;
    IY* pIY = NULL;

    // Запросить IY у IX
    hr = pIX->QueryInterface(IID_IY, (void**)&pIY);

    if (SUCCEEDED(hr))
    {
        IZ* pIZ = NULL;

        // Запросить IZ у IY
        hr = pIY->QueryInterface(IID_IZ, (void**)&pIZ);

        if (SUCCEEDED(hr))
        {
            // Запросить IZ у IX
            hr = pIX->QueryInterface(IID_IZ, (void**)&pIZ);
            assert(SUCCEEDED(hr)); // Это должно работать
        }
    }
}





//Клиент

int main() {
    system("chcp 65001");

    cout << "Hello, world!" << endl;

    HRESULT hr;

    cout << "Клиент: получаем указатель на IUnknown" << endl;
    IUnknown* pIUnknown = CreateInstance();

    cout << "\nКлиент: получаем указатель на IX" << endl;
    IX* pIX = NULL;
    hr = pIUnknown->QueryInterface(IID_IX, (void**)&pIX);
    if (SUCCEEDED(hr)) {
        cout << "Клиент: IX успешно получен" << endl;
        pIX->Fx(); // Используем интерфейс IX
    }

    cout << "\nКлиент: получаем указатель на IY" << endl;
    IY* pIY = NULL;
    hr = pIUnknown->QueryInterface(IID_IY, (void**)&pIY);
    if (SUCCEEDED(hr)) {
        cout << "Клиент: IY успешно получен" << endl;
        pIY->Fy(); // Используем интерфейс IY
    }

    cout << "\nКлиент: получаем неподдерживаемый интерфейс" << endl;
    IZ* pIZ = NULL;
    hr = pIUnknown->QueryInterface(IID_IZ, (void**)&pIZ);
    if (SUCCEEDED(hr)) {
        cout << "Клиент: интерфейс IZ успешно получен" << endl;
    }
    else {
        cout << "Клиент: невозможно получить интерфейс IZ" << endl;
    }

    cout << "\nКлиент: получаем указатель на IY через IX" << endl;
    IY* pIYfromIX = NULL;
    hr = pIX->QueryInterface(IID_IY, (void**)&pIYfromIX);
    if (SUCCEEDED(hr)) {
        cout << "Клиент: указатель на IY успешно получен" << endl;
        pIYfromIX->Fy();
    }
    else {
        cout << "Клиент: невозможно получить указатель на IY через IX" << endl;
    }

    cout << "\nКлиент: получаем указатель на IUnknown через IY" << endl;
    IUnknown* pIUnknownFromIY = NULL;
    hr = pIY->QueryInterface(IID_IUnknown1, (void**)&pIUnknownFromIY);
    if (SUCCEEDED(hr)) {
        cout << "Равны ли два указателя?" << endl;
        if (pIUnknownFromIY == pIUnknown) {
            cout << "ДА" << endl;
        }
        else {
            cout << "НЕТ" << endl;
        }
    }

    // Тест функции f
    cout << "\nТест f()" << endl;
    try {
        f(pIX);
        cout << "f() выполнена успешно" << endl;
    }
    catch (...) {
        cout << "Ошибка при выполнении f()" << endl;
    }

    // Тест функции f2
    cout << "\nТест f2()" << endl;
    try {
        f2(pIX);
        cout << "f2() выполнена успешно" << endl;
    }
    catch (...) {
        cout << "Ошибка при выполнении f2()" << endl;
    }

    // Тест функции f3
    cout << "\nТест f3()" << endl;
    try {
        f3(pIX);
        cout << "f3() выполнена успешно" << endl;
    }
    catch (...) {
        cout << "Ошибка при выполнении f3()" << endl;
    }

    // Тест функции SameComponents
    cout << "\nТест SameComponents()" << endl;
    bool result = SameComponents(pIX, pIY);
    cout << "Результат сравнения компонентов: "
        << (result ? "указатели совпадают" : "указатели различаются")
        << endl;

    // Очистка памяти после тестов
    if (pIX != NULL) pIX->Release();
    if (pIY != NULL) pIY->Release();

    // Удаление компонента
    delete pIUnknown;

    return 0;
}