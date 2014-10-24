#ifndef BASE_REQUEST_MAP_H
#define BASE_REQUEST_MAP_H

#include <map>
#include <type_traits>

namespace base {
template<typename T>
class request_map
{
public:
  typedef unsigned long id_type;
  typedef T request_type;

  template<typename U>
  id_type insert(U&& request)
  {
    static_assert(std::is_same<request_type, typename std::decay<U>::type>::value,
                  "Class's and method's template arguments differ");

    id_type id = next_id++;
    // FIXME detect collisions and re-use free ids
    requests[id] = std::forward<U>(request);
    return id;
  }

  void erase(id_type id)
  {
    requests.erase(id);
  }

  T& get(id_type id)
  {
    return requests[id];
  }

private:
  id_type next_id = 0;
  std::map<id_type, request_type> requests;
};
}

#endif
