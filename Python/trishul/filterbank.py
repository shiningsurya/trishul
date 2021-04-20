
"""
A class-like interface to filterbank?

"""

import numpy as np

import sigpyproc as spp

## kind of sucks that `scikit-image` is a dependency only because of this `block_reduce`
from skimage.measure import block_reduce


class Filterbank:
    """
    I don't know if I should put this in a separate class of its own
    """

    def __init__ (self, filname, mask=None, gulp=1024):
        """
        Creates a filterbank instance interfacing with `sigpyproc`
        """
        self.filname         = filname
        self.fil             = spp.FilReader (filname)
        self.h               = self.fil.header
        ##
        self.gulp            = gulp
        self.mask            = None

    def decimated_read (self, start, width, tfac, ffac=1):
        """
        Reads filterbank and decimates in chunks to reduce IO/memory burden

        Take care of the decimations while computing `start` and `width`

        Arguments
        ---------
            start: unsigned int start of the samples
            width: unsigned int number of samples to read

            tfac: decimation in time-axis (axis=1)
            ffac: decimation in freq-axis (axis=0)

            gulp: int number of samples to read in a gulp
            mask: array of nchans 

        Returns
        -------
            filterbank: numpy.ndarray of float32
        """
        nchans_in    = self.h['nchans']
        nchans_ou    = int(nchans_in // ffac )
        nsamps_ou    = int(( width // tfac ) + 1)

        ofb          = np.zeros ((nchans_ou, nsamps_ou), dtype=np.float32)

        A,B          = 0,0
        rp           = self.fil.readPlan (self.gulp, start=start, nsamps=width, verbose=False,)
        for iread, _, payload in rp:
            gfb      = payload.reshape ((-1, nchans_in)).T
            if self.mask is not None:
                gfb[self.mask] = 0

            hfb      = block_reduce (gfb, (ffac, tfac), func=np.mean, cval=np.median(gfb))
            B        = A + hfb.shape[1]
            ofb[:,A:B] = hfb
            A        = B

        return ofb
