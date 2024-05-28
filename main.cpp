#include <iostream>

using namespace std;

template<class T>
class MyShared
    {
    T* p = nullptr;
    atomic<int> * count = nullptr;

    public:
    MyShared (T* p = nullptr) : p(p), count(new std::atomic<int>(1)) {}
    MyShared (MyShared& other) : p(other.p), count(other.count)
        {
        if (count) 
            {
            ++(*count);
            }
        }
        
    MyShared& operator = (const MyShared& other)
        {
        if (this != &other) 
            {
            cleanup();
            p = other.p;
            count = other.count;
            if (count) 
                {
                ++(*count);
                }
            }
        return *this;
        }
        
    MyShared(MyShared&& other) : p(other.p), count(other.count)
        {
        other.p = nullptr;
        other.count = nullptr;
        }
        
    MyShared& operator = (MyShared&& other)
        {
        if (this != &other) 
            {
            cleanup();
            p = other.p;
            count = other.count;
            other.p = nullptr;
            other.count = nullptr;
            }
            return *this;
        }
        
    ~MyShared() 
        {
        cleanup();
        }
        
    T* get() const 
        {
        return p;
        }
    T& operator*()
        {
        return *p;
        }
        
    T* operator->()
        {
        return p;
        }

    private:
    void cleanup() 
        {
        if (count && --(*count) == 0) 
            {
            delete p;
            delete count;
            }
        }
    };

template<class T, class... Args>

MyShared<T> Make_MyShared(Args&&... args) 
    {
    return MyShared<T>(new T(forward<Args>(args)...));
    }

class MyPoint 
    {
    int x, y;

    public:
        
    MyPoint(int x, int y) : x(x), y(y)
        {
        //cout << "MyPoint(" << x << ", " << y << ")\n";
        }

    void print()
        {
        cout << "MyPoint(" << x << ", " << y << ")\n";
        }
    };

int main() 
    {
    MyShared<MyPoint> point1 = Make_MyShared<MyPoint>(3, 4);
    point1->print();
    
    MyShared<MyPoint> point2 = point1;
    point2->print();
    
    MyShared<MyPoint> point3 = Make_MyShared<MyPoint>(5, 6);
    point3->print();
    
    point3 = point1;
    point3->print();
    
    MyShared<MyPoint> point4 = move(point1);
    point4->print();

    return 0;
    }
