#!/usr/bin/env python

"""
Usage: make_configFile.py output_file ave_events
"""

import sys
import npstat
from npstat import ArchiveRecord_AbsDiscreteDistribution1D as DistroRecord


def make_mixing_distributions(ar, nevents):
    # Distribution of the number of mixed in events
    events_mix_distro = npstat.Poisson1D(nevents)
    ar.store(DistroRecord(events_mix_distro, "Event Distribution", "Mixing"))

    # Distribution of time slices
    time_shift_distro = npstat.DiscreteTabulated1D(-1L, (0.5, 0.0, 0.5))
    ar.store(DistroRecord(time_shift_distro, "TS Distribution", "Mixing"))
    return


def main(argv):
    # Parse command line options
    argc = len(argv)
    if (argc == 0):
        # Convention used here: command invoked without any arguments
        # prints its usage instruction and exits successfully
        print __doc__
        return 0
    elif (argc == 2):
        # We have the correct number of arguments
        i = 0
        output_file = argv[i]; i+=1
        nevents = float(argv[i]); i+=1
    else:
        # The number of arguments is incorrect
        print __doc__
        return 1

    # Create the string achive which will hold all config objects
    ar = npstat.StringArchive()

    # Call functions which construct the config objects
    make_mixing_distributions(ar, nevents)

    # Write out the configuration archive
    if npstat.writeStringArchive(ar, output_file):
        print 'Wrote object configuration file "%s"'  % output_file
        return 0
    else:
        print 'Failed to write file "%s"' % output_file
        return 2


if __name__=='__main__':
    sys.exit(main(sys.argv[1:]))
