#ifndef BERRY_BIND_H
#define BERRY_BIND_H
#include <string>
#include <vector>
#include <functional>
#include <tuple>
#include <map>
#include <any>
#include <type_traits>
#include <EuphoniumLog.h>
#include <exception>
#include <iostream>
#include <cstring>
#include "be_vm.h"

typedef std::map<std::string, std::any> berry_map;

template <typename T>
berry_map to_berry_map(std::map<std::string, T> map)
{
    berry_map berryMap;
    for (auto &pair : map)
    {
        berryMap[pair.first] = pair.second;
    }
    return berryMap;
}

class Berry
{
private:
    bool del;
    bvm *vm;
    std::vector<std::function<int(Berry &)> *> lambdas;
    static int call(bvm *vm);

    template <class T>
    T arg(const int i)
    {

        return *(T *)comptr(i);
    }

    template <typename T>
    int ret(const T r)
    {
        be_return(vm);
    }

    template <class T>
    T *argp(const int i)
    {
        return (T *)comptr(i);
    }

    template <typename T, typename T1, typename... Args>
    std::tuple<T, T1, Args...> args(const int i = 1) {
        return std::tuple_cat(std::make_tuple(arg<T>(i)), args<T1, Args...>(i + 1));
    }

    template <typename T>
    std::tuple<T> args(const int i = 1)
    {
        return std::tuple<T>(arg<T>(i));
    }

    template <typename... Args>
    struct sizer
    {
        static const int size = sizeof...(Args);
    };

    template <int N>
    struct apply_method
    {
        template <class T, typename R, typename... MethodArgs,
                  typename... TupleArgs, typename... Args>
        static R apply(T *o, R (T::*method)(MethodArgs...),
                       std::tuple<TupleArgs...> &t, Args... args)
        {
            return apply_method<N - 1>::apply(o, method, t, std::get<N - 1>(t), args...);
        }
    };

    template <int N>
    struct apply_function
    {
        template <typename R, typename... FunctionArgs, typename... TupleArgs,
                  typename... Args>
        static R apply(R (*function)(FunctionArgs...),
                       std::tuple<TupleArgs...> &t, Args... args)
        {
            return apply_function<N - 1>::apply(function, t, std::get<N - 1>(t), args...);
        }
    };

    template <int N>
    struct apply_function_stdfunc
    {
        template <typename R, typename... FunctionArgs, typename... TupleArgs,
                  typename... Args>
        static R apply(std::function<R(FunctionArgs...)> function,
                       std::tuple<TupleArgs...> &t, Args... args)
        {
            return apply_function_stdfunc<N - 1>::apply(function, t, std::get<N - 1>(t), args...);
        }
    };

public:
    Berry();
    Berry(bvm *vm);
    ~Berry();

    bvm *rawPtr()
    {
        return vm;
    }

    void execute_string(const std::string &string);

    void lambda(std::function<int(Berry &)> *function, const std::string &name);

    void get_global(const std::string &name)
    {
        be_getglobal(vm, name.c_str());
    }

    void pcall(const int args)
    {
        be_pcall(vm, args);
        be_pop(vm, args + 1);
    }

    void comptr(const void *d);
    void *comptr(const int i = 1);

    void closure(int (*)(bvm *), const int i = -1);
    void setglobal(const std::string &name);

    void boolean(const bool);
    bool boolean(const int i = -1);

    void number(const bint);
    bint tonumber(const int i = -1);

    void string(const std::string &);

    berry_map map(const int i = -1)
    {
        be_getmember(vm, i, ".p");

        berry_map result;
        if (!be_ismap(vm, -1))
        {
            throw std::runtime_error("Is not a map");
        }

        // push the iterator
        be_pushiter(vm, -1);

        while (be_iter_hasnext(vm, -2))
        {
            be_iter_next(vm, -2);
            auto key = string(-2);

            BELL_LOG(info, "berry", "Got key [%s]", key.c_str());
            ;

            if (be_isstring(vm, -1))
            {
                result[key] = string(-1);
            }
            else if (be_isnumber(vm, -1))
            {
                result[key] = (int)tonumber(-1);
            }
            else if (be_isbool(vm, -1))
            {
                result[key] = boolean(-1);
            }
            else if (be_isinstance(vm, -1))
            {
                result[key] = map(-1);
            }
            else
            {
                BELL_LOG(info, "berry", "Unknown type");
            }

            be_pop(vm, 2);
        }

        be_pop(vm, 1);

        return result;
    }

    void map(const berry_map &berryMap)
    {
        startMap();
        // iterate over each berryMap field
        auto it = berryMap.begin();

        while (it != berryMap.end())
        {
            std::string key = it->first;
            auto value = it->second;

            if (auto x = std::any_cast<std::string>(&value))
            {
                string(key);
                string(std::any_cast<std::string>(value));
                pushMapPair();
            }
            else if (auto x = std::any_cast<const char *>(&value))
            {
                string(key);
                be_pushstring(vm, std::any_cast<const char *>(value));
                pushMapPair();
            }
            else if (auto x = std::any_cast<bool>(&value))
            {
                string(key);
                boolean(std::any_cast<bool>(value));
                pushMapPair();
            }
            else if (auto x = std::any_cast<int>(&value))
            {
                string(key);
                number(std::any_cast<int>(value));
                pushMapPair();
            }
            else if (auto x = std::any_cast<berry_map>(&value))
            {
                string(key);
                map(std::any_cast<berry_map>(value));
                pushMapPair();
            }
            // Increment the Iterator to point to next entry
            it++;
        }

        endMap();
    }

    std::string string(const int i = -1);

    void real(const breal);
    breal toreal(const int i = -1);

    int doreturn()
    {
        be_return(vm);
    }

    template <typename R, class T, typename... Args>
    void export_method(const std::string &name,
                       R (T::*method)(Args...))
    {
        auto function = new std::function<int(Berry &)>([method](Berry &vm) -> int
                                                        {
                                                            auto tuple = vm.args<Args...>(2);
                                                            if constexpr (std::is_same_v<R, void>)
                                                            {
                                                                apply_method<std::tuple_size<decltype(tuple)>::value>::apply(vm.argp<T>(1), method, tuple);
                                                                return vm.doreturn();
                                                            }
                                                            else
                                                            {
                                                                return vm.ret(
                                                                    apply_method<std::tuple_size<decltype(tuple)>::value>::apply(vm.argp<T>(1), method, tuple));
                                                            }
                                                        });
        lambda(function, name);
    }

    void startMap()
    {
        be_newmap(vm);
    }

    void mapPush(const std::string &key, const std::string &value)
    {
        string(key);
        string(value);
        pushMapPair();
    }

    void mapPush(const std::string &key, int value)
    {
        string(key);
        number(value);
        pushMapPair();
    }

    void mapPush(const std::string &key, bool value)
    {
        string(key);
        boolean(value);
        pushMapPair();
    }

    void pushMapPair()
    {
        be_setindex(vm, -3);
        be_pop(vm, 2);
    }

    void endMap()
    {
        be_getbuiltin(vm, "map");
        be_pushvalue(vm, -2);
        be_call(vm, 1);
        be_moveto(vm, -2, -3);
        be_pop(vm, 2);
    }

    template <typename R, typename... Args>
    void export_function(const std::string &name,
                         std::function<R(Args...)> callback)
    {
        auto function = new std::function<int(Berry &)>([callback](Berry &vm) -> int
                                                        {
                                                            auto tuple = vm.args<Args...>();

                                                            if constexpr (std::is_same_v<R, void>)
                                                            {
                                                                apply_function_stdfunc<std::tuple_size<decltype(tuple)>::value>::apply(callback, tuple);
                                                                return vm.doreturn();
                                                            }
                                                            else
                                                            {
                                                                return vm.ret(
                                                                    apply_function_stdfunc<std::tuple_size<decltype(tuple)>::value>::apply(callback, tuple));
                                                            }
                                                        });
        lambda(function, name);
    }

    template <typename R, typename... Args>
    void export_function(const std::string &name,
                         R (*callback)(Args...))
    {
        auto function = new std::function<int(Berry &)>([callback](Berry &vm) -> int
                                                        {
                                                            auto tuple = vm.args<Args...>();

                                                            if constexpr (std::is_same_v<R, void>)
                                                            {
                                                                apply_function<std::tuple_size<decltype(tuple)>::value>::apply(callback, tuple);
                                                                return vm.doreturn();
                                                            }
                                                            else
                                                            {
                                                                return vm.ret(
                                                                    apply_function<std::tuple_size<decltype(tuple)>::value>::apply(callback, tuple));
                                                            }
                                                        });
        lambda(function, name);
    }

    template <typename R>
    void export_function(const std::string &name, R (*callback)())
    {
        auto function = new std::function<int(Berry &)>([callback](Berry &vm) -> int
                                                        {
                                                            if constexpr (std::is_same_v<R, void>)
                                                            {
                                                                (*callback)();
                                                                return vm.doreturn();
                                                            }
                                                            else
                                                            {
                                                                return vm.ret((*callback)());
                                                            }
                                                        });
        lambda(function, name);
    }

    template <typename R, class T>
    void export_method(const std::string &name, R (T::*method)())
    {
        auto function = new std::function<int(Berry &)>([method](Berry &vm) -> int
                                                        {
                                                            if constexpr (std::is_same_v<R, void>)
                                                            {
                                                                vm.argp<T>(1)->*method();
                                                                return vm.doreturn();
                                                            }
                                                            else
                                                            {
                                                                return vm.ret((vm.argp<T>(1)->*method)());
                                                            }
                                                        });
        lambda(function, name);
    }

    template <typename R>
    void export_function(const std::string &name, std::function<R(void)> callback)
    {
        auto function = new std::function<int(Berry &)>([callback](Berry &vm) -> int
                                                        {
                                                            if constexpr (std::is_same_v<R, void>)
                                                            {
                                                                callback();
                                                                return vm.doreturn();
                                                            }
                                                            else
                                                            {
                                                                return vm.ret(callback());
                                                            }
                                                        });
        lambda(function, name);
    }

    template <class C, typename Ret, typename... Ts>
    void export_this(const std::string &name, C *c, Ret (C::*m)(Ts...))
    {
        std::function<Ret(Ts...)> res = [=](auto &&...args)
        { return (c->*m)(std::forward<decltype(args)>(args)...); };
        export_function(name, res);
    }
};

template <>
int Berry::ret<bool>(const bool r);

template <>
int Berry::ret<bint>(const bint r);

template <>
int Berry::ret<breal>(const breal r);

template <>
int Berry::ret<berry_map>(const berry_map r);

template <>
int Berry::ret<std::string>(const std::string r);

template <>
std::string Berry::arg<std::string>(const int i);

template <>
bint Berry::arg<bint>(const int i);

template <>
float Berry::arg<float>(const int i);

template <>
int Berry::arg<int>(const int i);

template <>
bool Berry::arg<bool>(const int i);

template <>
berry_map Berry::arg<berry_map>(const int i);

template <>
struct Berry::apply_method<0>
{
    template <class T, typename R, typename... MethodArgs,
              typename... TupleArgs, typename... Args>
    static R apply(T *o, R (T::*method)(MethodArgs...),
                   std::tuple<TupleArgs...> &t, Args... args)
    {
        return (o->*method)(args...);
    }
};

template <>
struct Berry::apply_function<0>
{
    template <typename R, typename... FunctionArgs, typename... TupleArgs,
              typename... Args>
    static R apply(R (*function)(FunctionArgs...),
                   std::tuple<TupleArgs...> &t, Args... args)
    {
        return (*function)(args...);
    }
};

template <>
struct Berry::apply_function_stdfunc<0>
{
    template <typename R, typename... FunctionArgs, typename... TupleArgs,
              typename... Args>
    static R apply(std::function<R(FunctionArgs...)> function,
                   std::tuple<TupleArgs...> &t, Args... args)
    {
        return function(args...);
    }
};

#endif
