#ifndef _LV_TOOLS_OPTION_PARSER_HPP
#define _LV_TOOLS_OPTION_PARSER_HPP

#include <sstream>
#include <vector>
#include <memory>
#include <initializer_list>
#include <unordered_map>

namespace LV {
  namespace Tools {

    class OptionValue;

    class OptionSpec;

    typedef std::shared_ptr<OptionValue> OptionValuePtr;

    class OptionParser
    {
    public:

        typedef std::vector<std::string> ArgList;
        typedef ArgList::const_iterator  ArgIter;

        OptionParser (std::initializer_list<OptionSpec> const& option_specs);

        ~OptionParser ();

        void parse (int argc, char** argv, ArgList& unparsed_args);
        void parse (ArgList const& args, ArgList& unparsed_args);

    private:

        typedef std::vector<std::tuple<OptionSpec, int>>    OptionSpecList;
        typedef OptionSpecList::iterator                    OptionSpecListIter;

        std::unordered_map<char, OptionSpecListIter>        m_short_name_map;
        std::unordered_map<std::string, OptionSpecListIter> m_long_name_map;
        OptionSpecList                                      m_option_specs;

        ArgIter parse_partial (ArgIter start, ArgIter end, ArgList& unparsed_args);
    };

    class OptionSpec
    {
    public:

        char           short_name;
        std::string    long_name;
        std::string    description;
        bool           required;
        OptionValuePtr value;

        OptionSpec (char                  short_name_,
                    std::string const&    long_name_,
                    std::string const&    description_,
                    bool                  required_,
                    OptionValuePtr const& value_)
            : short_name  (short_name_)
            , long_name   (long_name_)
            , description (description_)
            , required    (required_)
            , value       (value_)
        {}
    };

    class OptionValue
    {
    public:

        virtual OptionParser::ArgIter parse (OptionParser::ArgIter start, OptionParser::ArgIter const& end) = 0;

        virtual ~OptionValue () {}
    };

    template <typename T>
    class OptionSingleValue
        : public OptionValue
    {
    public:

        typedef std::function<void (T const&)> Setter;

        OptionSingleValue (Setter const& setter, T const& default_value)
            : m_setter (setter)
        {
            m_setter (default_value);
        }

        OptionSingleValue (T& var, T const& default_value)
            : m_setter ([=] (T const& value) { var = value; })
        {
            m_setter (default_value);
        }

        virtual OptionParser::ArgIter parse (OptionParser::ArgIter start, OptionParser::ArgIter const& end)
        {
            if (start == end) {
                return start;
            }

            std::istringstream stream { *start };
            T parsed_value;

            if (!(stream >> parsed_value)) {
                return start;
            }

            m_setter (parsed_value);
            ++start;

            return start;
        }

        virtual ~OptionSingleValue ()
        {}

    private:

        Setter m_setter;
    };


  } // Tools namespace
} // LV namespace

#endif // _LV_TOOLS_OPTION_PARSER_HPP
