# cython
cimport cython
# numpy 
import numpy  as np
cimport numpy as np

FDTYPE = np.float32
DDTYPE = np.uint64
ctypedef np.float32_t FDTYPE_t 
ctypedef np.uint64_t DDTYPE_t

@cython.boundscheck (False)
@cython.wraparound  (False)
def Dedisperser (np.ndarray[FDTYPE_t, ndim=2] fb, np.ndarray[DDTYPE_t, ndim=1] delays):
    if fb.shape[0] != delays.shape[0]:
        raise ValueError ("Incorrect delays size!")
    # constants 
    cdef Py_ssize_t nchans   = fb.shape[0]
    cdef Py_ssize_t nsamps   = fb.shape[1]
    cdef Py_ssize_t maxdelay = delays[nchans -1]
    if nsamps <= maxdelay:
        raise ValueError ("DM range too high!")
    cdef Py_ssize_t ddnsamps = nsamps - maxdelay
    cdef Py_ssize_t tidx     = 0
    # output array
    cdef np.ndarray[FDTYPE_t, ndim=2] ret = np.zeros ([nchans, ddnsamps], dtype=FDTYPE)
    # algo
    for isamp in range (ddnsamps):
        for ichan in range (nchans):
            tidx = isamp + delays[ichan]
            ret[ichan, isamp] = fb[ichan, tidx]
    #
    return ret

