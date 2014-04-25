#ifndef BASE_CALLABLE_H
#define BASE_CALLABLE_H

#include <type_traits>
#include <memory>

#include <base/compatibility.h>

namespace base {
class callable FINAL
{
  struct callable_model_base
  {
    virtual ~callable_model_base() { }
    virtual void call() const = 0;
  };

  template<typename CallableT>
  class callable_model : public callable_model_base
  {
  public:
    // CallableU is the same as CallableT
    template<typename CallableU, typename std::enable_if<!std::is_same<callable_model, CallableU>::value>::type* = nullptr>
    callable_model(CallableU&& c) : callable_(c) { }

    void call() const { callable_(); }

  private:
    CallableT callable_;
  };

public:
  callable() = default;

  // FIXME the enable_f check is not sufficient probably! Though it prevents
  //       from "too perfect forwarding" but it accepts arguments of e.g.
  //       integral type
  template<typename CallableT, typename std::enable_if<!std::is_same<callable, CallableT>::value>::type* = nullptr>
  callable(CallableT&& c)
  {
    callable_.reset(new callable_model<CallableT>{std::forward<CallableT>(c)});
  }

  void operator()()
  {
    if (callable_)
      callable_->call();
  }

private:
  std::shared_ptr<const callable_model_base> callable_;
};
}

#endif
