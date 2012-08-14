#ifndef _LV_TOOLS_OPTION_PARSER_HPP
#define _LV_TOOLS_OPTION_PARSER_HPP

#include <sstream>
#include <vector>
#include <memory>
#include <initializer_list>
#include <stdexcept>

namespace LV {
  namespace Tools {

    class OptionArg;

    struct OptionSpec;

    class OptionParserError;

    typedef std::shared_ptr<OptionArg> OptionArgPtr;

    class OptionParser
    {
    public:

        class Error;

        typedef std::vector<std::string> ArgList;

        OptionParser (std::initializer_list<OptionSpec> const& option_specs);
        ~OptionParser ();

        void parse (int argc, char** argv, ArgList& unparsed_args);
        void parse (ArgList const& args, ArgList& unparsed_args);

    private:

        class Impl;
        const std::unique_ptr<Impl> m_impl;
    };

    struct OptionSpec
    {
        char         short_name;
        std::string  long_name;
        std::string  description;
        OptionArgPtr argument;
    };

    class OptionArg
    {
    public:

        virtual bool parse (std::string const& arg) = 0;

        virtual ~OptionArg () {}
    };

    template <typename T>
    class OptionSingleArg
        : public OptionArg
    {
    public:

        typedef std::function<void (T const&)> Setter;

        OptionSingleArg (Setter const& setter, T const& default_value = T ())
            : m_setter (setter)
        {
            m_setter (default_value);
        }

        OptionSingleArg (T& var, T const& default_value = T ())
            : m_setter ([&] (T const& value) { var = value; })
        {
            m_setter (default_value);
        }

        virtual bool parse (std::string const& arg)
        {
            std::istringstream stream (arg);
            T parsed_arg;

            if (!(stream >> parsed_arg)) {
                return false;
            }

            m_setter (parsed_arg);

            return true;
        }

        virtual ~OptionSingleArg ()
        {}

    private:

        Setter m_setter;
    };

    class OptionParser::Error
        : public std::exception
    {
    public:

        Error () = delete;

        static Error invalid_option (std::string const& name);
        static Error missing_arg    (std::string const& name);
        static Error invalid_arg    (std::string const& name, std::string const& arg);
        static Error unexpected_arg (std::string const& name, std::string const& arg);

        virtual char const* what () const noexcept
        {
            return m_what.c_str ();
        }

        virtual ~Error () noexcept
        {}

    private:

        std::string m_what;

        explicit Error (std::string const& what);
    };

  } // Tools namespace
} // LV namespace

#endif // _LV_TOOLS_OPTION_PARSER_HPP
