#ifndef BASE_CANCELLATION_TOKEN_H
#define BASE_CANCELLATION_TOKEN_H

struct cancellation_token
{
  struct data
  {
    std::atomic<bool> cancelled{false};
    std::atomic<unsigned> count{0};
  };

  cancellation_token() : state{new data}
  {
    ++state->count;
  }

  cancellation_token(const cancellation_token& other)
  {
    state = other.state;
    ++state->count;
  }

  cancellation_token& operator=(const cancellation_token& other)
  {
    cancellation_token tmp{other};
    *this = std::move(tmp);
    return *this;
  }

  ~cancellation_token()
  {
    if (0 == --state->count)
      delete state;
  }

  data* state;
};

#endif
