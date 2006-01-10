//-*- Mode: C++; compile-command: "g++ -Wall -ansi -pedantic $(pkg-config --cflags --libs libvisual) lv_fft_test.cpp -o lv_fft_test"; -*-

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <libvisual/libvisual.h>


int
main (int    n_args,
      char **args)
{
    visual_init (&n_args, &args);

    if (n_args > 1)
    {
        try
        {
            std::vector<float> input (n_args - 1);
            std::vector<float> output (input.size () / 2);

            for (int i = 1; i < n_args; i++)
                input[i - 1] = boost::lexical_cast<float> (args[i]);

            VisFFT *fft = visual_fft_new (input.size (), output.size ());

            visual_fft_perform (fft, &input[0], &output[0]);

            visual_object_unref (reinterpret_cast<VisObject *> (fft));

            std::cout << std::setprecision (6);

            std::copy (input.begin (), input.end (), std::ostream_iterator<float> (std::cout, " "));
            std::cout << '\n';

            std::copy (output.begin (), output.end (), std::ostream_iterator<float> (std::cout, " "));
            std::cout << '\n';

            return EXIT_SUCCESS;
        }
        catch (boost::bad_lexical_cast &error)
        {
            std::cerr << error.what () << std::endl;

            return EXIT_FAILURE;
        }
    }
    else
    {
        std::cerr << boost::format ("Usage: %s input1 input2 .. inputN\n") % args[0];

        return EXIT_FAILURE;
    }
}
