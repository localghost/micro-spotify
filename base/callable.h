#ifndef BASE_CALLABLE_H
#define BASE_CALLABLE_H

#include <memory>
#include <type_traits>

namespace base {
class callable
{
private:
  class model_base
  {
  public:
    virtual ~model_base() = default;
    virtual void call() = 0;
  };

  template<typename T>
  class model : public model_base
  {
  public:
    template<typename U, typename std::enable_if<!std::is_same<model, U>::value>::type* = nullptr>
    explicit model(U&& action) : action{std::forward<U>(action)} {}

    void call()
    {
      action();
    }

  private:
    T action;
  };

public:
  template<typename T>
  explicit callable(T&& t) : action{new model<T>{std::forward<T>(t)}} {}

  void operator()()
  {
    action->call();
  }

private:
  std::shared_ptr<model_base> action;
};
}

#endif
