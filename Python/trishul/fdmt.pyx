"""
FDMT routine

cython'd

Taken from FDMT Python implementation shared on the FDMT paper. 
** Entire credit to Zackay & Ofek (2014) **

Adapted to work with filterbank with fmax to fmin frequency axis.
"""

import numpy as np

cimport numpy as np

f_t = np.float32
ctypedef np.float32_t cf_t


cdef class FDMT:
    cdef public cf_t                      fmin 
    cdef public cf_t                      fmax
    cdef public cf_t                      fres
    cdef public cf_t                      df
    cdef public unsigned int              F
    cdef public unsigned int              f
    cdef public Py_ssize_t                T
    cdef public Py_ssize_t                maxdt
    cdef public np.ndarray                State
    def __init__ (self, float fch1, float foff, unsigned int nchans):
        '''
        Fast Dedispersion Measure Transform 

        Arguments
        ---------

        fch1 : float 
            First channel. The highest frequency.
        foff : float
            Frequency resolution.
        nchans: unsigned int
            Number of channels
        '''
        # args
        self.fmax  = fch1
        if foff < 0:
            foff   = -foff
        self.fmin  = fch1 - (nchans*foff)
        self.fres  = foff
        self.F     = nchans
        self.f     = np.log2 (self.F)

    cdef __ff2__(self, cf_t f1, cf_t f2):
        '''f1 < f2'''
        return 1.0/f2/f2 - 1.0/f1/f1

    cdef __initialization__ (self, np.ndarray[cf_t, ndim=2] Image):
        cdef Py_ssize_t deltaT 
        deltaT = int (np.ceil ( 
                (self.maxdt-1)                               * 
                self.__ff2__(self.fmin, self.fmin+self.fres)   /
                self.__ff2__(self.fmin, self.fmax)
                ))

        self.State = np.zeros([self.F,deltaT+2,self.T],dtype=f_t)
        self.State[0:self.F,0,0:self.T] = Image

        #print ("State shape = ({0}, {1}, {2}) size = {3} MB".format(self.F,deltaT+2,self.T, self.State.nbytes/1E6))

        cdef Py_ssize_t j_dT = 0
        for i_dT in range(1,deltaT+1):
            j_dT = self.T - i_dT
            self.State[0:self.F, i_dT  , i_dT:self.T]       =    \
            self.State[0:self.F, i_dT-1, i_dT:self.T]       +    \
            Image     [0:self.F, 0:j_dT]

    def __iteration__ (self, unsigned int ii):
        self.df = 2 ** ii * self.fres
        cdef Py_ssize_t F_jumps  = self.F / 2**ii
        cdef cf_t correction     = 0.5 * self.fres * bool (ii)

        cdef Py_ssize_t deltaT 
        deltaT = int (np.ceil (  
            ( self.maxdt-1 )                            *
            self.__ff2__ (self.fmin+self.df, self.fmin) /
            self.__ff2__ (self.fmax, self.fmin)
            ))
        
        cdef np.ndarray[cf_t, ndim=3] Output = np.zeros([F_jumps, deltaT+1, self.T],dtype = f_t)
        
        cdef cf_t         fstart
        cdef cf_t         fend
        cdef cf_t         fmidl
        cdef cf_t         fmid
        cdef Py_ssize_t   dTlocal
        cdef Py_ssize_t   dT_mid
        cdef Py_ssize_t   dT_midl
        cdef Py_ssize_t   dT_rest
        cdef Py_ssize_t   i_T_min
        cdef Py_ssize_t   i_T_max
        for i_F in range(F_jumps):
            fstart = self.fmax - self.df * i_F 
            fend   = self.fmax - self.df * ( i_F+1 )
            fmidl  = 0.5*(fend + fstart)  - correction
            fmid   = 0.5*(fend + fstart)  + correction

            dTlocal  = int (np.ceil(
                (self.maxdt-1)                        *
                self.__ff2__(fstart, fend)            /
                self.__ff2__(self.fmax,   self.fmin)  
                ))
            
            for i_dT in range(dTlocal+1):
                dT_mid  = int (round (i_dT * (
                    self.__ff2__(fstart, fmid) / 
                    self.__ff2__(fstart, fend)
                    )))
                dT_midl = int(round (i_dT * (
                    self.__ff2__(fstart, fmidl)  /
                    self.__ff2__(fstart, fend)
                    )))
                dT_rest = i_dT - dT_midl
                
                i_T_min = self.T - dT_midl
                i_T_max = self.T
                Output    [i_F  , i_dT  , i_T_min:i_T_max]      =    \
                self.State[2*i_F, dT_mid, i_T_min:i_T_max]
                
                
                i_T_min = 0
                i_T_max = self.T - dT_midl
                Output     [i_F    , i_dT   , i_T_min:i_T_max]                    =                    \
                self.State [2*i_F  , dT_mid , i_T_min:i_T_max]                    +                    \
                self.State [2*i_F+1, dT_rest, i_T_min + dT_midl:i_T_max+dT_midl]
        # save memory 
        del self.State
        self.State = Output
        #print ("State shape = ({0}, {1}, {2}) size = {3} MB".format(F_jumps,deltaT+1,self.T, self.State.nbytes/1E6))
    
    def __worker__ (self, np.ndarray[cf_t, ndim=2] Image, Py_ssize_t maxdt):
        '''Internal method
            User won't be calling this

        Arguments
        ---------
        Image: [F,T] filterbank
        maxdt: max delay in time units

        Returns
        -------
        [maxdt,T] array
        '''
        self.maxdt = maxdt
        cdef unsigned int F
        F      = Image.shape[0]
        self.T = Image.shape[1]
        if F != self.F:
            raise ValueError("Nchans mismatch!")
        #
        self.__initialization__ (Image)
        for it in range (1, self.f+1):
            self.__iteration__ (it)
        #
        F                   = self.State.shape[0]
        cdef Py_ssize_t dT  = self.State.shape[1]
        cdef Py_ssize_t T   = self.State.shape[2]
        return np.reshape (self.State, [dT,T])
    
    def Bowtie (self, np.ndarray[cf_t, ndim=2] Image, np.ndarray[np.uint32_t, ndim=1] delays):
        '''Computes bowtie plane from the filterbank using FDMT
        Arguments
        ---------
        Image : Filterbank array with shape=(NCHAN,NSAMP)
        delays : array of DM-delays in time units
        '''
        cdef Py_ssize_t nchans             = Image.shape[0]
        cdef Py_ssize_t nsamps             = Image.shape[1]
        cdef Py_ssize_t dsize              = delays.shape[0]
        cdef Py_ssize_t maxdt              = delays[dsize-1] + 1
        cdef np.ndarray fbt                = self.__worker__(Image, maxdt)
        # dmrange selection
        cdef Py_ssize_t ddnsamps           = nsamps - maxdt
        cdef np.ndarray[cf_t, ndim=2] bt   = np.zeros ((delays.size, ddnsamps), dtype=f_t)
        for i,dm in enumerate (delays):
            bt[i] = fbt[dm,:ddnsamps]
        return bt
