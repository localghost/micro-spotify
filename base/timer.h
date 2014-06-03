namespace base {
class timer FINAL
{
public:
  timer() = default;
  explicit timer(callable callable_);
  timer(const thread& thread_, callable callable_);
  ~timer();
  
  void invoke();

private:
  thread* thread_;
};
}
