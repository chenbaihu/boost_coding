template<typename T>
class Test {
    public:
    Test(T t):t_(t) { }
    public:
        int a() { return a_; }
        void set_t(T t);
        T get_t();
        //void set_t(T t) { t_ = t; };
        //T get_t() { return t_; };
    private:
        int a_;
        T t_;
};
