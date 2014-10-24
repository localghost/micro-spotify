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
    template<typename U,
             typename std::enable_if<!std::is_same<model, typename std::decay<U>::type>::value>::type* = nullptr>
    explicit model(U&& action) : action{std::forward<U>(action)} {}

    void call()
    {
      action();
    }

  private:
    T action;
  };

public:
  template<typename T,
           typename std::enable_if<!std::is_same<callable, typename std::decay<T>::type>::value>::type* = nullptr>
  callable(T&& t) : action{new model<typename std::decay<T>::type>{std::forward<T>(t)}} {}

  callable(const callable&) noexcept = default;
  callable(callable&&) noexcept = default;

  callable& operator=(const callable&) noexcept = default;
  callable& operator=(callable&) noexcept = default;

  void operator()()
  {
    action->call();
  }

private:
  std::shared_ptr<model_base> action;
};
}

#endif
