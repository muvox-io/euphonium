#include "BerryBind.h"

Berry::Berry(bvm *vm) : vm(vm) {}

Berry::Berry()
{
    vm = be_vm_new();
}

Berry::~Berry()
{
}

int Berry::call(bvm *vm)
{
    be_getupval(vm, 0, 0);

    auto function = (std::function<int(Berry &)> *)be_tocomptr(vm, -1);

    auto l = Berry(vm);
    return (*function)(l);
}

void Berry::execute_string(const std::string &data)
{
    if (be_loadstring(vm, data.c_str()) == 0)
    {
        be_pcall(vm, 0);
    }
    else
    {
        be_dumpexcept(vm);
    }
    be_pop(vm, 1);
}

void Berry::lambda(std::function<int(Berry &)> *function, const std::string &name)
{
    lambdas.push_back(function);
    closure(Berry::call, 1);
    comptr(function);
    be_setupval(vm, -2, 0);
    be_pop(vm, 1);
    setglobal(name);
    be_pop(vm, 1);
}

void Berry::closure(int (*callback)(bvm *), const int i)
{
    be_pushntvclosure(vm, callback, i);
}

void Berry::boolean(const bool b)
{
    be_pushbool(vm, b);
}

bool Berry::boolean(const int i)
{
    if (!be_isbool(vm, i))
        be_raise(vm, "internal_error", "is not boolean");
    return be_tobool(vm, i);
}

void Berry::string(const std::string &string)
{
    be_pushstring(vm, string.c_str());
}

std::string Berry::string(const int i)
{
    if (!be_isstring(vm, i))
        be_raise(vm, "internal_error", "Is not string");
    return be_tostring(vm, i);
}

void Berry::real(const breal b)
{
    be_pushreal(vm, b);
}

breal Berry::toreal(const int i)
{
    if (!be_isreal(vm, i))
        be_raise(vm, "internal_error", "is not breal");
    return be_toreal(vm, i);
}

void Berry::number(const bint b)
{
    be_pushint(vm, b);
}

bint Berry::tonumber(const int i)
{
    if (!be_isint(vm, i))
        be_raise(vm, "internal_error", "is not breal");
    return be_toint(vm, i);
}

void Berry::setglobal(const std::string &name)
{
    be_setglobal(vm, name.c_str());
}

void Berry::comptr(const void *d)
{
    be_pushcomptr(vm, const_cast<void *>(d));
}

void *Berry::comptr(const int i)
{
    if (!be_iscomptr(vm, i))
        be_raise(vm, "internal_error", "is not comptr");
    return be_tocomptr(vm, i);
}

template <>
int Berry::ret<bool>(const bool r)
{
    boolean(r);
    be_return(vm);
}

template <>
int Berry::ret<berry_map>(const berry_map r)
{
    map(r);
    be_return(vm);
}

template <>
int Berry::ret<std::string>(const std::string r)
{
    string(r);
    be_return(vm);
}

template <>
int Berry::ret<breal>(const breal r)
{
    real(r);
    be_return(vm);
}

template <>
int Berry::ret<bint>(const bint r)
{
    number(r);
    be_return(vm);
}

template <>
std::string Berry::arg<std::string>(const int i)
{
    return string(i);
}

template <>
float Berry::arg<float>(const int i)
{
    return toreal(i);
}

template <>
bool Berry::arg<bool>(const int i)
{
    BELL_LOG(info, "bvm", "getting arg");
    return boolean(i);
}

template <>
bint Berry::arg<bint>(const int i)
{
    return tonumber(i);
}

template <>
berry_map Berry::arg<berry_map>(const int i)
{
    return map(i);
}

template <>
int Berry::arg<int>(const int i)
{
    return tonumber(i);
}