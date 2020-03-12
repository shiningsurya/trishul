# cython
cimport cython
# numpy 
import numpy  as np
cimport numpy as np

ctypedef fused datype:
    np.ndarray[np.float32_t, ndim=2]
    np.ndarray[np.uint8_t, ndim=2]

DDTYPE = np.uint64
ctypedef np.uint64_t DDTYPE_t

@cython.boundscheck (False)
@cython.wraparound  (False)
def Dedisperser (datype fb, np.ndarray[DDTYPE_t, ndim=1] delays):
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
    cdef datype ret = np.zeros ([nchans, ddnsamps], dtype=fb.dtype)
    # algo
    for isamp in range (ddnsamps):
        for ichan in range (nchans):
            tidx = isamp + delays[ichan]
            ret[ichan, isamp] = fb[ichan, tidx]
    #
    return ret

