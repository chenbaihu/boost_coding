#ifndef H_TEST_
#define H_TEST_

template<typename T>
class Test {
    public:
    Test(T t):t_(t) { }
    public:
        int a() { return a_; }
        void set_t(T t);
        T get_t();
    private:
        int a_;
        T t_;
};

#endif
