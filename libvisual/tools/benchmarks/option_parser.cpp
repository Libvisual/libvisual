#include "option_parser.hpp"
#include <stdexcept>

namespace LV {
  namespace Tools {

    OptionParser::OptionParser (std::initializer_list<OptionSpec> const& option_specs)
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

    OptionParser::~OptionParser ()
    {}

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

    OptionParser::ArgIter OptionParser::parse_long_option (ArgIter current, ArgIter end)
    {
        auto name_end_pos = current->find ('=');

        auto option_name = current->substr (2, name_end_pos - 2);
        auto option_arg  = current->substr (name_end_pos + 1);

        ++current;

        auto entry = m_long_name_map.find (option_name);

        if (entry == m_long_name_map.end ()) {
            throw Error::invalid_option (option_name);
        }

        auto& option_spec = entry->second;

        if (!option_spec->argument) {
            if (!option_arg.empty ()) {
                throw Error::unexpected_arg (option_name, option_arg);
            }
            return current;
        }

        if (option_arg.empty ()) {
            if (current != end && (*current)[0] != '-') {
                option_arg = *current;
                ++current;
            } else {
                throw Error::missing_arg (option_name);
            }
        }

        if (!option_spec->argument->parse (option_arg)) {
            throw Error::invalid_arg (option_name, option_arg);
        }

        return current;
    }

    OptionParser::ArgIter OptionParser::parse_short_option (ArgIter current, ArgIter end)
    {
        auto option_name = (*current)[1];
        auto option_arg  = current->substr (2);

        ++current;

        auto entry = m_short_name_map.find (option_name);

        if (entry == m_short_name_map.end ()) {
            throw Error::invalid_option (std::string (1, option_name));
        }

        auto& option_spec = entry->second;

        if (!option_spec->argument) {
            if (!option_arg.empty ()) {
                throw Error::unexpected_arg (std::string (1, option_name), option_arg);
            }
            return current;
        }

        if (option_arg.empty ()) {
            if (current != end && (*current)[0] != '-') {
                option_arg = *current;
                ++current;
            } else {
                throw Error::missing_arg (std::string (1, option_name));
            }
        }

        if (!option_spec->argument->parse (option_arg)) {
            throw Error::invalid_arg (std::string (1, option_name), option_arg);
        }

        return current;
    }

    void OptionParser::parse (ArgList const& args, ArgList& unparsed_args)
    {
        unparsed_args.clear ();

        auto current = args.begin ();
        bool parse_options = true;

        while (current != args.end ()) {
            if (*current == "--") {
                parse_options = false;
                ++current;
                continue;
            }

            if (parse_options) {
                if ((*current)[0] == '-') {
                    if ((*current)[1] == '-') {
                        current = parse_long_option (current, args.end ());
                        continue;
                    } else {
                        current = parse_short_option (current, args.end ());
                        continue;
                    }
                }
            }

            unparsed_args.emplace_back (*current);
            ++current;
        }
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
}  // LV namespace
