#include "option_parser.hpp"
#include <unordered_map>

namespace LV {
  namespace Tools {

    class OptionParser::Impl
    {
    public:

        typedef ArgList::const_iterator  ArgIter;

        typedef std::vector<OptionSpec>  OptionSpecList;
        typedef OptionSpecList::iterator OptionSpecListIter;

        std::unordered_map<char, OptionSpecListIter>        m_short_name_map;
        std::unordered_map<std::string, OptionSpecListIter> m_long_name_map;
        OptionSpecList                                      m_option_specs;

        std::tuple<std::string, std::string, OptionSpec*> parse_option_name (ArgIter current);
        ArgIter                                           parse_option      (ArgIter current, ArgIter end);

        Impl (std::initializer_list<OptionSpec> const& option_specs);
        ~Impl ();
    };

    OptionParser::OptionParser (std::initializer_list<OptionSpec> const& option_specs)
        : m_impl { new Impl (option_specs) }
    {}

    OptionParser::~OptionParser ()
    {}

    void OptionParser::parse (ArgList const& args, ArgList& unparsed_args)
    {
        // Clear the unparsed argument vector and reserve some space
        // for quick appends
        unparsed_args.clear ();
        unparsed_args.reserve (args.size () / 2);

        bool parse_options = true;

        // Point current token to the first argument
        auto current = args.begin ();

        // Parse argument list
        while (current != args.end ()) {
            // Turn off option parsing once -- is encountered
            if (*current == "--") {
                parse_options = false;
                ++current;
                continue;
            }

            // Parse current token as an option
            if (parse_options) {
                current = m_impl->parse_option (current, args.end ());
                continue;
            }

            // Put non-options and non-option-arguments into the
            // unparsed vector
            unparsed_args.emplace_back (*current);
            ++current;
        }
    }

    void OptionParser::parse (int argc, char** argv, ArgList& unparsed_args)
    {
        // Convert C-string array into a string vector before
        // passing to the actual parser

        ArgList args;
        args.reserve (argc + 1);
        for (int i = 0; i <= argc; i++) {
            args.emplace_back (argv[i]);
        }

        parse (args, unparsed_args);
    }

    OptionParser::Impl::Impl (std::initializer_list<OptionSpec> const& option_specs)
    {
        m_option_specs.reserve (option_specs.size ());

        for (auto& option_spec : option_specs) {
            m_option_specs.emplace_back (option_spec);

            // Get iterator to the appended entry
            auto back_iter = m_option_specs.end ();
            --back_iter;

            // Add entry to short-name map
            m_short_name_map[option_spec.short_name] = back_iter;

            // Add entry to long-name map if a long name is specified
            if (!option_spec.long_name.empty ()) {
                m_long_name_map[option_spec.long_name] = back_iter;
            }
        }
    }

    OptionParser::Impl::~Impl ()
    {}

    std::tuple<std::string, std::string, OptionSpec*> OptionParser::Impl::parse_option_name (ArgIter current)
    {
        if ((*current)[0] == '-') {
            if ((*current)[1] == '-') {
                // Token is prefixed with --, it is a long option in either of two formats:
                // a. with argument: --arg-name=ARG-VALUE
                // b. without argument: --arg-name
                auto name_end_pos = current->find ('=');
                auto name  = current->substr (2, name_end_pos - 2);
                auto arg   = current->substr (name_end_pos + 1);
                auto entry = m_long_name_map.find (name);
                auto spec  = (entry != m_long_name_map.end ()) ? &(*entry->second) : nullptr;

                return std::make_tuple (name, arg, spec);
            } else {
                // Token is prefixed with -, it is a short option in either of two formats:
                // a. with argument: -xARG-VALUE
                // b. without argument: -x
                auto name  = (*current)[1];
                auto arg   = current->substr (2);
                auto entry = m_short_name_map.find (name);
                auto spec  = (entry != m_short_name_map.end ()) ? &(*entry->second) : nullptr;

                return std::make_tuple (std::string (1, name), arg, spec);
            }
        }

        throw; // should never reach here
    }

    OptionParser::Impl::ArgIter OptionParser::Impl::parse_option (ArgIter current, ArgIter end)
    {
        std::string name;
        std::string arg;
        OptionSpec* spec;

        // Parse the current token as the option name
        std::tie (name, arg, spec) = parse_option_name (current);
        ++current;

        // Make sure the option is valid
        if (!spec) {
            throw Error::invalid_option (name);
        }

        // Handle no-argument options
        if (!spec->argument) {
            // Check if an extraneous argument was supplied
            if (!arg.empty ()) {
                throw Error::unexpected_arg (name, arg);
            }
            return current;
        }

        // If no argument was supplied along with the option name
        // (e.g. -xARG or --xxx=ARG), we take the next token as the
        // argument
        if (arg.empty ()) {
            // Make sure there are more tokens, and that the new token
            // is not another option
            if (current == end || (*current)[0] == '-') {
                throw Error::missing_arg (name);
            }

            arg = *current;
            ++current;
        }

        // Parse and validate the option argument
        if (!spec->argument->parse (arg)) {
            throw Error::invalid_arg (name, arg);
        }

        return current;
    }

    OptionParser::Error::Error (std::string const& what)
        : m_what (what)
    {}

    OptionParser::Error OptionParser::Error::invalid_option (std::string const& name)
    {
        return Error ("Invalid option '" + name + "'");
    }

    OptionParser::Error OptionParser::Error::missing_arg (std::string const& name)
    {
        return Error ("Option '" + name + "' requires an argument");
    }

    OptionParser::Error OptionParser::Error::invalid_arg (std::string const& name, std::string const& arg)
    {
        return Error ("Invalid argument '" + arg + "' to option '" + name + "'");
    }

    OptionParser::Error OptionParser::Error::unexpected_arg  (std::string const& name, std::string const& arg)
    {
        return Error ("Unexpected argument '" + arg + "' to option '" + name + "'");
    }

  } // Tools namespace
} // LV namespace
