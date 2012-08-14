#include "option_parser.hpp"
#include <stdexcept>

namespace LV {
  namespace Tools {

    OptionParser::OptionParser (std::initializer_list<OptionSpec> const& option_specs)
    {
        m_option_specs.reserve (option_specs.size ());

        for (auto const& option_spec : option_specs) {
            m_option_specs.emplace_back (option_spec, 0);

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

    void OptionParser::parse (ArgList const& args, ArgList& unparsed_args)
    {
        auto start = args.begin ();
        auto end   = args.end ();

        while (start != end) {
            start = parse_partial (start, end, unparsed_args);
        }

        // Check if required arguments are parsed
        for (auto const& entry : m_option_specs) {
            OptionSpec const& spec  = std::get<0> (entry);
            int               count = std::get<1> (entry);

            if (spec.required && count == 0) {
                throw std::runtime_error ("Required option not set");
            }
        }
    }

    OptionParser::ArgIter OptionParser::parse_partial (ArgIter start, ArgIter end, ArgList& unparsed_args)
    {
        if ((*start)[0] == '-') {
            OptionSpecListIter spec = m_option_specs.end ();

            if ((*start)[1] == '-') {
                // Lookup long option name
                auto entry = m_long_name_map.find (start->substr (2));
                if (entry == m_long_name_map.end ()) {
                    throw std::runtime_error ("Invalid short option");
                }

                spec = entry->second;
                ++start;
            }
            else {
                // Lookup short option name
                auto entry = m_short_name_map.find ((*start) [1]);
                if (entry == m_short_name_map.end ()) {
                    throw std::runtime_error ("Invalid long option");
                }

                spec = entry->second;
                ++start;
            }

            return std::get<0> (*spec).value->parse (start, end);
        }
        else {
            unparsed_args.emplace_back (*start);
            ++start;

            return start;
        }
    }

  } // Tools namespace
}  // LV namespace
