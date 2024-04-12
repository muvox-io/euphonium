#include "BerryBind.h"
#include <stdio.h>
#include <memory>
#include "EuphLogger.h"
#include "be_debug.h"

using namespace berry;

berry::moduleMap berry::moduleLambdas;

VmState::VmState(bvm* vm) : vm(vm) {}

int berryStdoutBSD(void* ctx, const char* buf, int len) {
  auto state = (VmState*)ctx;
  state->stdoutIntercept(buf, len);
  return len;
}

ssize_t berryStdoutLinux(void* ctx, const char* buf, size_t len) {
  auto state = (VmState*)ctx;
  state->stdoutIntercept(buf, len);
  return len;
}

static char stdoutLineBuffer[256] = {0};

VmState::VmState() {
#ifdef __linux__
  stdoutRedirect = fopencookie(
      this, "w", (cookie_io_functions_t){.write = berryStdoutLinux});
#else
  stdoutRedirect = funopen(this, NULL, &berryStdoutBSD, NULL, NULL);
#endif
  setvbuf(stdoutRedirect, stdoutLineBuffer, _IOLBF, sizeof(stdoutLineBuffer));
  be_set_stdout(stdoutRedirect);
  vm = be_vm_new();
  be_set_obs_hook(vm, &VmState::berryObservability);
  be_pushntvfunction(vm, VmState::get_module_member);
  be_setglobal(vm, "get_native");
  be_pop(vm, 1);
}

void VmState::stdoutIntercept(const char* buf, ssize_t len) {
  fwrite(buf, sizeof(char), len, stdout);
  if (stdoutCallback != nullptr) {
    stdoutCallback(buf, len);
  }
}

void VmState::berryObservability(bvm* vm, int event...) {
  va_list param;
  va_start(param, event);
  switch (event) {
    case BE_OBS_PCALL_ERROR:  // error after be_pcall
    {
      int32_t top = be_top(vm);
      // check if we have two strings for an Exception
      if (top >= 2 && be_isstring(vm, -1) && be_isstring(vm, -2)) {
        EUPH_LOG(error, "Berry error: %s", be_tostring(vm, -2),
                 be_tostring(vm, -1));
        // be_tracestack(vm);
      }
    }
  }
  va_end(param);
}

VmState::~VmState() {}

// Retrieves function pointer by module name and its function name
int VmState::get_module_member(bvm* vm) {

  int top = be_top(vm);
  if (top == 2 && be_isstring(vm, 2)) {
    auto module = std::string(be_tostring(vm, 1));
    auto functionName = std::string(be_tostring(vm, 2));
    if (moduleLambdas.find(module + "." + functionName) !=
        moduleLambdas.end()) {
      be_pop(vm, 2);
      auto func = moduleLambdas.at(module + "." + functionName);
      be_pushntvclosure(vm, VmState::call, 1);
      be_pushcomptr(vm, func);
      be_setupval(vm, -2, 0);
      be_pop(vm, 1);
      be_return(vm);
    }
  }

  be_pop(vm, top);
  be_return_nil(vm);
}

int VmState::call(bvm* vm) {
  be_getupval(vm, 0, 0);

  auto function = (std::function<int(VmState&)>*)be_tocomptr(vm, -1);

  auto l = VmState(vm);
  return (*function)(l);
}

void VmState::execute_string(const std::string& data,
                             const std::string& tag = "script") {
  // create an array
  if (be_loadbuffer(vm, tag.c_str(), data.c_str(), data.size()) == 0) {
    if (be_pcall(vm, 0) != 0) {

      auto errorMessage = dumpException();

      bell::bellGlobalLogger->error(tag, 0, TAG, "Berry error: %s",
                                    errorMessage.c_str());
      if (stdoutCallback != nullptr) {
        stdoutCallback(errorMessage.c_str(), errorMessage.size());
        stdoutCallback("\n", 1);
      }
      throw BerryErrorException(errorMessage);
      
    }
  } else {
    auto errorMessage = dumpException();
    bell::bellGlobalLogger->error(tag, 0, TAG, "Berry error: %s",
                                  errorMessage.c_str());
    if (stdoutCallback != nullptr) {
      stdoutCallback(errorMessage.c_str(), errorMessage.size());
      stdoutCallback("\n", 1);
    }
     throw BerryErrorException(errorMessage);
  }
  be_pop(vm, 1);
}

/**
 * @brief Native function pushed to the berry stack that a value as a string.
 * 
 * @param vm 
 * @param esc 
 * @return int 
 */
static int _dvfunc(bvm* vm) {
  const char* s = be_tostring(vm, 1);
  std::string* exceptionMsg = (std::string*)be_tocomptr(vm, 2);
  *exceptionMsg += s;
  be_return_nil(vm);
}

int VmState::dumpValue(int index, std::string& exceptionMsg) {
  int res, top = be_top(vm) + 1;
  index = be_absindex(vm, index);
  be_pushntvfunction(vm, _dvfunc);
  be_pushvalue(vm, index);
  be_pushcomptr(vm, std::addressof(exceptionMsg));
  res = be_pcall(vm, 2); /* using index to store result */
  be_remove(vm, top);    /* remove '_dumpvalue' function */
  be_remove(vm, top);    /* remove the value */
  be_remove(vm, top);    /* remove the exception message  pointer */
  if (res == BE_EXCEPTION) {
    be_dumpexcept(vm);
  }
  return res;
}

std::string VmState::dumpException() {
  std::string exceptionMsg;
  do {

    /* print exception value */
    if (dumpValue(-2, exceptionMsg))
      break;

    exceptionMsg += ": ";
    /* print exception argument */
    if (dumpValue(-1, exceptionMsg))
      break;
    exceptionMsg += "\n";
    /* print stack traceback */
    exceptionMsg += traceStack();

  } while (0);
  if (exceptionMsg.ends_with("\n")) {
    exceptionMsg = exceptionMsg.substr(0, exceptionMsg.size() - 1);
  }
  be_pop(vm, 2); /* pop the exception value & argument */
  return exceptionMsg;
}

// Copied from be_debug.c
static inline void repair_stack(bvm* vm) {
  bcallsnapshot* cf;
  bcallsnapshot* base =
      static_cast<bcallsnapshot*>(be_stack_base(&vm->tracestack));
  bcallsnapshot* top =
      static_cast<bcallsnapshot*>(be_stack_top(&vm->tracestack));
  /* Because the native function does not push `ip` to the
     * stack, the ip on the native function frame corresponds
     * to the previous Berry closure. */
  for (cf = top; cf >= base; --cf) {
    if (!var_isclosure(&cf->func)) {
      /* the last native function stack frame has the `ip` of
             * the previous Berry frame */
      binstruction* ip = cf->ip;
      /* skip native function stack frames */
      for (; cf >= base && !var_isclosure(&cf->func); --cf)
        ;
      /* fixed `ip` of Berry closure frame near native function frame */
      if (cf >= base)
        cf->ip = ip;
    }
  }
}

static inline std::string sourceinfo(bproto* proto, binstruction* ip) {

  std::string result;
#if BE_DEBUG_RUNTIME_INFO
  char buf[24];
  be_assert(proto != NULL);
  if (proto->lineinfo && proto->nlineinfo) {
    blineinfo* it = proto->lineinfo;
    blineinfo* end = it + proto->nlineinfo;
    int pc = cast_int(ip - proto->code - 1); /* now vm->ip has been increased */
    for (; it < end && pc > it->endpc; ++it)
      ;
    snprintf(buf, sizeof(buf), ":%d:", it->linenumber);
#if BE_DEBUG_SOURCE_FILE
    result += be_str2cstr(proto->source);
#endif
    result += buf;
  } else {
    result += "<unknown source>:";
  }
#else
  (void)proto;
  (void)ip;
  result += "<unknown source>:";
#endif

  return result;
}

std::string VmState::traceStack() {
  std::string result;
  EUPH_LOG(error, TAG, "traceStack()");
  if (be_stack_count(&vm->tracestack)) {
    EUPH_LOG(error, TAG, "   ...OK");
    repair_stack(vm);
    bcallsnapshot* cf;
    bcallsnapshot* base =
        static_cast<bcallsnapshot*>(be_stack_base(&vm->tracestack));
    bcallsnapshot* top =
        static_cast<bcallsnapshot*>(be_stack_top(&vm->tracestack));
    result += "stack traceback:\n";
    for (cf = top; cf >= base; --cf) {
      if (cf <= top - 10 && cf >= base + 10) {
        if (cf == top - 10)
          result += "\t...\n";
        continue;
      }
      if (var_isclosure(&cf->func)) {
        bclosure* cl = static_cast<bclosure*>(var_toobj(&cf->func));
        result += "\t";
        result += sourceinfo(cl->proto, cf->ip);
        result += " in function `";
        result += be_str2cstr(cl->proto->name);
        result += "'\n";

      } else {
        result += "\t<native>: in native function\n";
      }
    }
  }
  return result;
}

void VmState::lambda(std::function<int(VmState&)>* function,
                     const std::string& name, const std::string& module) {
  if (module == "") {
    lambdas.push_back(function);
    closure(VmState::call, 1);
    comptr(function);
    be_setupval(vm, -2, 0);
    be_pop(vm, 1);
    set_global(name);
    be_pop(vm, 1);
  } else {
    moduleLambdas.insert({module + "." + name, function});
  }
}

void VmState::closure(int (*callback)(bvm*), const int i) {
  be_pushntvclosure(vm, callback, i);
}

void VmState::boolean(const bool b) {
  be_pushbool(vm, b);
}

bool VmState::boolean(const int i) {
  if (!be_isbool(vm, i))
    be_raise(vm, "internal_error", "is not boolean");
  return be_tobool(vm, i);
}

void VmState::string(const std::string& string) {
  be_pushstring(vm, string.c_str());
}

std::string VmState::string(const int i) {
  if (!be_isstring(vm, i))
    be_raise(vm, "internal_error", "Is not string");
  return be_tostring(vm, i);
}

void VmState::real(const breal b) {
  be_pushreal(vm, b);
}

breal VmState::toreal(const int i) {
  if (!be_isreal(vm, i))
    be_raise(vm, "internal_error", "is not breal");
  return be_toreal(vm, i);
}

void VmState::number(const bint b) {
  be_pushint(vm, b);
}

bint VmState::tonumber(const int i) {
  if (!be_isint(vm, i))
    be_raise(vm, "internal_error", "is not breal");
  return be_toint(vm, i);
}

void VmState::set_global(const std::string& name) {
  be_setglobal(vm, name.c_str());
}

berry::list VmState::list(const int i) {
  be_getmember(vm, i, ".p");

  berry::list result;
  if (!be_islist(vm, -1)) {
    throw std::runtime_error("Is not a list");
  }

  // push the iterator
  be_pushiter(vm, -1);

  while (be_iter_hasnext(vm, -2)) {
    be_iter_next(vm, -2);

    if (be_isstring(vm, -1)) {
      result.push_back(string(-1));
    } else if (be_islist(vm, -1)) {
      result.push_back(list(-1));
    } else if (be_isnumber(vm, -1)) {
      result.push_back((int)tonumber(-1));
    } else if (be_islist(vm, -1)) {
      result.push_back(list(-1));
    } else if (be_isbool(vm, -1)) {
      result.push_back(boolean(-1));
    } else if (be_isinstance(vm, -1)) {
      be_getmember(vm, -1, ".p");

      if (be_ismap(vm, -1)) {
        be_pop(vm, 1);
        result.push_back(map(-1));
      } else if (be_islist(vm, -1)) {
        be_pop(vm, 1);
        result.push_back(list(-1));
      } else {
        be_pop(vm, 1);
      }
    } else {
      // Unknown type
    }

    be_pop(vm, 1);
  }

  be_pop(vm, 1);

  return result;
}

berry::map VmState::map(const int i) {
  be_getmember(vm, i, ".p");

  berry::map result;
  if (!be_ismap(vm, -1)) {
    throw std::runtime_error("Is not a map");
  }

  // push the iterator
  be_pushiter(vm, -1);

  while (be_iter_hasnext(vm, -2)) {
    be_iter_next(vm, -2);
    auto key = string(-2);

    if (be_isstring(vm, -1)) {
      result[key] = string(-1);
    } else if (be_isnumber(vm, -1)) {
      result[key] = (int)tonumber(-1);
    } else if (be_islist(vm, -1)) {
      result[key] = list(-1);
    } else if (be_isbool(vm, -1)) {
      result[key] = boolean(-1);
    } else if (be_isinstance(vm, -1)) {
      be_getmember(vm, -1, ".p");
      if (be_ismap(vm, -1)) {
        be_pop(vm, 1);
        result[key] = map(-1);
      } else if (be_islist(vm, -1)) {
        be_pop(vm, 1);
        result[key] = list(-1);
      } else {
        be_pop(vm, 1);
      }
    } else {
      // Unknown type
    }

    be_pop(vm, 2);
  }

  be_pop(vm, 1);

  return result;
}

void VmState::list(const berry::list& berryList) {

  be_newlist(vm);
  // iterate over all items in berryList
  for (auto& value : berryList) {
    if (std::any_cast<std::string>(&value)) {
      string(std::any_cast<std::string>(value));
      push_data();
    } else if (std::any_cast<const char*>(&value)) {
      be_pushstring(vm, std::any_cast<const char*>(value));
      push_data();
    } else if (std::any_cast<bool>(&value)) {
      boolean(std::any_cast<bool>(value));
      push_data();
    } else if (std::any_cast<int>(&value)) {
      number(std::any_cast<int>(value));
      push_data();
    } else if (std::any_cast<berry::list>(&value)) {
      list(std::any_cast<berry::list>(value));
      push_data();
    } else if (std::any_cast<berry::map>(&value)) {
      map(std::any_cast<berry::map>(value));
      push_data();
    }
  }

  end_element("list");
}

void VmState::map(const berry::map& berryMap) {
  be_newmap(vm);

  // iterate over each berryMap field
  auto it = berryMap.begin();

  while (it != berryMap.end()) {
    std::string key = it->first;
    auto value = it->second;
    be_stack_require(vm, 2 + BE_STACK_FREE_MIN);
    if (std::any_cast<std::string>(&value)) {
      string(key);
      string(std::any_cast<std::string>(value));
      set_index(2);
    } else if (std::any_cast<const char*>(&value)) {
      string(key);
      be_pushstring(vm, std::any_cast<const char*>(value));
      set_index(2);
    } else if (std::any_cast<bool>(&value)) {
      string(key);
      boolean(std::any_cast<bool>(value));
      set_index(2);
    } else if (std::any_cast<berry::list>(&value)) {
      string(key);
      list(std::any_cast<berry::list>(value));
      set_index(2);
    } else if (std::any_cast<int>(&value)) {
      string(key);
      number(std::any_cast<int>(value));
      set_index(2);
    } else if (std::any_cast<berry::map>(&value)) {
      string(key);
      map(std::any_cast<berry::map>(value));
      set_index(2);
    }
    // Increment the Iterator to point to next entry
    it++;
  }

  end_element("map");
}

void VmState::comptr(const void* d) {
  be_pushcomptr(vm, const_cast<void*>(d));
}

void* VmState::tocomptr(const int i) {
  if (!be_iscomptr(vm, i))
    be_raise(vm, "internal_error", "is not comptr");
  return be_tocomptr(vm, i);
}

template <>
int VmState::ret<bool>(const bool r) {
  boolean(r);
  be_return(vm);
}

template <>
int VmState::ret<berry::map>(const berry::map r) {
  map(r);
  be_return(vm);
}

template <>
int VmState::ret<berry::list>(const berry::list r) {
  list(r);
  be_return(vm);
}

template <>
int VmState::ret<std::string>(const std::string r) {
  string(r);
  be_return(vm);
}

template <>
int VmState::ret<breal>(const breal r) {
  real(r);
  be_return(vm);
}

template <>
int VmState::ret<bint>(const bint r) {
  number(r);
  be_return(vm);
}

template <>
int VmState::ret<uint64_t>(const uint64_t r) {
  number(r);
  be_return(vm);
}

template <>
int VmState::ret<int>(const int r) {
  number(r);
  be_return(vm);
}

template <>
int VmState::ret<uint8_t>(const uint8_t r) {
  number(r);
  be_return(vm);
}
template <>
std::string VmState::arg<std::string>(const int i) {
  return string(i);
}

template <>
const std::string& VmState::arg<const std::string&>(const int i) {
  return string(i);
}

template <>
float VmState::arg<float>(const int i) {
  return toreal(i);
}

template <>
bool VmState::arg<bool>(const int i) {
  return boolean(i);
}

template <>
bint VmState::arg<bint>(const int i) {
  return tonumber(i);
}

template <>
uint8_t VmState::arg<uint8_t>(const int i) {
  return tonumber(i);
}

template <>
uint64_t VmState::arg<uint64_t>(const int i) {
  return tonumber(i);
}

template <>
berry::map VmState::arg<berry::map>(const int i) {
  return map(i);
}

template <>
berry::list VmState::arg<berry::list>(const int i) {
  return list(i);
}

template <>
int VmState::arg<int>(const int i) {
  return tonumber(i);
}
