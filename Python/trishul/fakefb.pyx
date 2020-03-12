"""
Fake FB routine

cython'd

Inspired from dedisp fake filterbank generated.

Only supporting nbit==8
"""

import  numpy as np
cimport numpy as np
# Oh what would I do without you?

cimport cython

d_t    = np.uint8
f_t    = np.float32
i_t    = np.uint64
ctypedef np.uint8_t   cd_t
ctypedef np.float32_t cf_t
ctypedef np.uint64_t  ci_t

cdef class Fakefb:
    # parameters
    cdef public cf_t     tsamp
    cdef public ci_t     nchans
    cdef public cf_t     fch1 
    cdef public cf_t     foff 
    cdef public cf_t     rms
    # arrays
    cdef public np.ndarray    freqs
    cdef public np.ndarray    delays
    # methods
    def __init__(self, 
            float        tsamp   = 781.25E-6, 
            unsigned int nchans  = 4096,
            float        fch1    = 361.941448,
            float        foff    = -0.01023,
            float        rms     = 4.0
            ):
        '''
        FakeFB
        Args:
            tsamp (float) : 781.25E-6
            nchans (nchans) : 4096
            fch1 (float)  : 361.25 MHz
            foff (float)  : -0.01  MHz (must be negative)
            rms  (float)  : noise power (def=4.0)
        '''
        self.tsamp     = tsamp
        self.nchans    = nchans
        self.fch1      = fch1
        self.foff      = foff
        self.rms       = rms
        #
        self.freqs     = self.fch1 + ( np.arange (self.nchans, dtype=f_t) * self.foff )
        self.delays    = np.zeros (nchans, dtype=i_t)

    cdef set_delays (self, float dm):
        '''Set delays'''
        cdef cf_t if02 =  ( self.freqs[0]  ) ** -2
        cdef cf_t fac  = dm  * 4148.741601 / self.tsamp
        self.delays    = np.array( fac * ( np.power (self.freqs, -2.0) - if02 ), dtype=i_t )

    @cython.boundscheck (False)
    @cython.wraparound  (False)
    cdef worker(self, float sn, float dm, float wd, float it = 0.2, float jt = 0.1):
        '''
        Args:
            sn (float): how strong?
            dm (float): What dm to expect?
            wd (float): Width  in seconds
            it (float): This is the peak time 
                CARE: Always de-disperse to the highest frequency
            jt (float): Time after peak.
        '''
        cdef cf_t sigamp   = sn  * (1 - ( self.rms/100 ))
        cdef cf_t noiseamp = 128 * (1 - ( self.rms/100 ))
        self.set_delays (dm)
        cdef ci_t iit      =  int( it / self.tsamp )
        cdef ci_t wit      =  int( wd / self.tsamp )
        cdef ci_t ijt      =  int( jt / self.tsamp )
        cdef ci_t maxdelay =  self.delays[self.nchans-1]
        cdef ci_t tsize    =  iit + wit + maxdelay + ijt
        #
        cdef np.ndarray[cf_t,ndim=2] fb = self.rms * np.random.randn (self.nchans, tsize).astype(f_t)
        fb += noiseamp
        cdef ci_t ichan    = 0
        cdef ci_t iwt      = 0
        cdef ci_t jwt      = 0
        for ichan in np.arange (self.nchans, dtype=i_t):
            iwt = self.delays[ichan] + iit
            jwt = iwt + wit
            fb [ichan, iwt:jwt] += sigamp
        return fb 

    def __call__ (self, float sn, float dm, float wd, float it = 0.2, float jt = 0.1):
        return self.worker (sn, dm, wd, it, jt)
