#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <future>
#include <stdlib.h>
#include <chrono>
#include <atomic>
#include <functional>
#include <list>
template<typename T> class Functor{
public:
    typedef decltype(&T::operator()) FPtr;
    typedef typename Functor<FPtr>::result_type result_type;
    typedef T host_type;
private:
    host_type t;

public:
    Functor(T tp): t(tp){
    }

    template<typename... Args>
    result_type operator()(Args... args){
        std::cout<<"from functor" <<std::endl;
        return t(args...);
    }
};


template<typename R, typename H, typename... Args>
class Functor<R (H::*)(Args...)> {
public:
    typedef H host_type;
    typedef R result_type;
    typedef R (H::*func)(Args...);

private:
    host_type h;
    func f;

public:
    Functor(func foo, host_type host): h(host), f(foo){
    }
    result_type operator()(Args... args){
        std::cout<<"from method" <<std::endl;
        return (h.*f)(args...);
    }
};

template<typename R, typename... Args>
class Functor<R (*)(Args...)> {
public:
    typedef R result_type;
    typedef R (*func)(Args...);

private:
    func f;

public:
    Functor(func foo): f(foo){
    }
    result_type operator()(Args... args){
        std::cout<<"from function" <<std::endl;
        return f(args...);
    }
};

template<typename Foo, typename... Args>
Functor<Foo> make_functor(Foo f, Args... args){
    return Functor<Foo>(f, args...);
}



template<template<typename...> class Collection, typename FromType, typename FooType, typename... Args>
auto _f(Collection<FromType> coll, Functor<FooType> func, Args... args)
        -> Collection<typename Functor<FooType>::result_type>{
    typedef typename Collection<FromType>::iterator Iter;
    Collection<typename Functor<FooType>::result_type> res;
    for(Iter i = coll.begin(); i != coll.end(); ++i){
        res.push_back(func(*i, args...));
    }
    return res;
}

int foo(int i){
    return i/2.0;
}

class func{
public:
    int operator()(int i, int j = 1){
        return i + j;
    }
} foonc;

int main() {
    std::list<int> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);


    l = _f(l, make_functor(&func::operator(), foonc), 66);
    l = _f(l, make_functor(foonc));
    l = _f(l, make_functor(foo));



    for(auto i = l.begin(); i != l.end(); ++i){
        std::cout<<*i<<std::endl;
    }

    return 0;
}

