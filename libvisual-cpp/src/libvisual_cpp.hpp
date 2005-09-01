#ifndef LVCPP_LVCPP_HPP
#define LVCPP_LVCPP_HPP

#include <string>
#include <stdexcept>

namespace Lv
{
  class unexpected_error
      : public std::runtime_error
  {
  public:

      unexpected_error ()
          : std::runtime_error ("Unexpected exception")
      {}
  };


  const char *get_lv_version ();

  int init (int &argc, char **&argv, bool trap_exceptions = true);

  int init (bool trap_exceptions = true);

  bool is_init ();

  int quit ();

  int init_path_add (const std::string& path);

} // namespace Lv

#endif // #ifdef LVCPP_LVCPP_HPP
