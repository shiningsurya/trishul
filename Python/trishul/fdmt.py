"""
FDMT routine

Taken from FDMT Python implementation shared on the FDMT paper. 
** Entire credit to Zackay & Ofek (2014) **

Adapted to work with filterbank with fmax to fmin frequency axis.
"""

import numpy as np


class FDMT:
    def __init__ (self, fch1, foff, nchans, datatype=np.float32):
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
        datatype : np.float32
        '''
        # args
        self.fmax  = fch1
        if foff < 0:
            foff = -foff
        self.fmin  = fch1 - (nchans*foff)
        self.fres  = foff
        self.dtype = datatype
        self.F = nchans
        self.f = int ( np.log2 (self.F) )

    def __ff2__(self, f1, f2):
        '''f1 < f2'''
        return 1.0/f1/f1 - 1.0/f2/f2

    def __initialization__ (self, Image):
        deltaT = int (np.ceil ( 
                (self.maxdt-1)                               * 
                self.__ff2__(self.fmin, self.fmin+self.fres)   /
                self.__ff2__(self.fmin, self.fmax)
                ))

        self.State = np.zeros([self.F,deltaT+1,self.T],self.dtype)
        self.State[:,0,:] = Image

        for i_dT in range(1,deltaT+1):
            self.State[:, i_dT  , i_dT:]       =    \
            self.State[:, i_dT-1, i_dT:]       +    \
            Image     [:, :-i_dT]

    def __iteration__ (self, ii):
        self.df = 2 ** ii * self.fres
        deltaT = int (np.ceil (  
            ( self.maxdt-1 )                            *
            self.__ff2__ (self.fmin, self.fmin+self.df) /
            self.__ff2__ (self.fmin, self.fmax)
            ))
        
        F_jumps = int( self.F / 2**ii )
        Output = np.zeros([F_jumps, deltaT+1, self.T],self.dtype)
        
        correction = 0.5 * self.fres * bool (ii)

        for i_F in range(F_jumps):
            fstart = self.fmax - (self.fmax - self.fmin)/float(F_jumps) * i_F 
            fend   = self.fmax - (self.fmax - self.fmin)/float(F_jumps) * ( i_F+1 )
            fmid   = 0.5*(fend + fstart)  - correction
            fmidl  = 0.5*(fend + fstart)  + correction
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
                dT_midl = int (round (i_dT * (
                    self.__ff2__(fstart, fmidl)  /
                    self.__ff2__(fstart, fend)
                    )))
                dT_rest = int (i_dT - dT_midl)
                
                i_T_min = self.T - dT_midl
                i_T_max = self.T
                Output    [i_F  , i_dT  , i_T_min:i_T_max]      =    \
                self.State[2*i_F, dT_mid, i_T_min:i_T_max]
                
                
                i_T_min = 0
                i_T_max = self.T - dT_midl
                Output     [i_F    , i_dT   , i_T_min:i_T_max]                    =                    \
                self.State [2*i_F  , dT_mid , i_T_min:i_T_max]                    +                    \
                self.State [2*i_F+1, dT_rest, i_T_min + dT_midl:i_T_max+dT_midl]
        self.State = Output
    
    def __worker__ (self, Image, maxdt):
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
        F, self.T = Image.shape
        if F != self.F:
            raise ValueError("Nchans mismatch!")
        #
        self.__initialization__ (Image)
        for it in range (1, self.f+1):
            self.__iteration__ (it)
        #
        [F,dT,T] = self.State.shape
        return np.reshape (self.State, [dT,T])
    
    def Bowtie (self, Image, delays):
        '''Computes bowtie plane from the filterbank using FDMT
        Arguments
        ---------
        Image : Filterbank array with shape=(NCHAN,NSAMP)
        delays : array of DM-delays in time units
        '''
        nchans, nsamps = Image.shape
        maxdt = delays.max()+1
        fbt = self.__worker__(Image, maxdt)
        # dmrange selection
        ddnsamps = nsamps - maxdt
        bt = np.zeros ((delays.size, ddnsamps))
        for i,dm in enumerate (delays):
            bt[i] = fbt[dm,:ddnsamps]
        return bt
