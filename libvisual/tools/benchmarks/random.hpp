#ifndef _LV_TOOLS_RANDOM_HPP
#define _LV_TOOLS_RANDOM_HPP

#include <random>
#include <type_traits>

namespace LV {
  namespace Tools {

    // Creates a container filled with random values, each in the interval [min, max]
    template <class Container>
    Container make_random (typename Container::value_type min, typename Container::value_type max, typename Container::size_type size)
    {
        typedef std::mt19937_64 RandomNumGen;
        std::random_device rd;
        RandomNumGen rng { rd () };

        typedef typename Container::value_type T;
        typedef typename std::conditional<
                             std::is_floating_point<T>::value,
                             std::uniform_real_distribution<T>,
                             std::uniform_int_distribution<T>
                         >::type Distribution;

        Distribution distrib (min, max);

        Container container (size);
        for (auto& i : container) {
            i = distrib (rng);
        }

        return container;
    }

  } // Tools namespace

} // LV namespace

#endif // _LV_TOOLS_RANDOM_HPP
