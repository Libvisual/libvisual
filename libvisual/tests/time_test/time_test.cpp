#include <libvisual/libvisual.h>
#include <iostream>
#include <cassert>

int main (int argc, char** argv)
{
    LV::System::init (argc, argv);

    using LV::Time;

    // TODO: Randomize test times

    auto time1 = Time::from_secs (1.25);
    assert (time1.to_secs  () == 1.25);
    assert (time1.to_msecs () == 1250);
    assert (time1.to_usecs () == 1250000);

    auto time2 = Time::from_msecs (1250);
    assert (time2.to_secs () == 1.25);
    assert (time2.to_msecs () == 1250);
    assert (time2.to_usecs () == 1250000);

    auto time3 = Time::from_usecs (1250000);
    assert (time3.to_secs () == 1.25);
    assert (time3.to_msecs () == 1250);
    assert (time3.to_usecs () == 1250000);

    LV::System::destroy ();

    return EXIT_SUCCESS;
}
