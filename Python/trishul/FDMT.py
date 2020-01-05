"""
FDMT routine

Taken from FDMT python implementation
shared on the FDMT paper. 

Entire credit to Zackay & Ofek (2014)

Adapted to work with filterbank with fmax to fmin frequency axis.

"""

import numpy as np


class FDMT:
    def __init__ (self, fmin, fmax, maxdt, datatype=np.float32):
        '''
        fmin: lowest frequency in MHz/GHz.
        fmax: highest frequency in MHz/GHz.
        maxdt: maximum delay in time units
        '''
        # args
        self.fmin  = fmin
        self.fmax  = fmax
        self.maxdt = maxdt
        self.dtype = datatype
        # 
        self.F = None
        self.T = None
        self.f = None
        self.State = None
        self.df = None
        self.fres = None

    def __ff2__(self, f1, f2):
        '''f1 < f2'''
        return 1.0/f1/f1 - 1.0/f2/f2

    def __initialization__ (self, Image):
        self.df   = (self.fmax - self.fmin)/float(self.F)
        self.fres = self.df
        deltaT = int (np.ceil ( 
                (self.maxdt-1)                               * 
                self.__ff2__(self.fmin, self.fmin+self.df)   /
                self.__ff2__(self.fmin, self.fmax)
                ))

        self.State = np.zeros([self.F,deltaT+1,self.T],self.dtype)
        self.State[:,0,:] = Image

        for i_dT in range(1,deltaT+1):
            self.State[:,i_dT,i_dT:]       =    \
            self.State[:,i_dT-1,i_dT:]     +    \
            Image     [:,:-i_dT]

    def __iteration__ (self, ii):
        self.df = 2 ** ii * self.fres
        deltaT = int (np.ceil (  
            ( self.maxdt-1 )                            *
            self.__ff2__ (self.fmin, self.fmin+self.df) /
            self.__ff2__ (self.fmin, self.fmax)
            ))
        
        F_jumps = int( self.F / 2**ii )
        Output = np.zeros([F_jumps, deltaT+1, self.T],self.dtype)
        
        if ii>0:
            correction = 0.5 * self.fres
        else:
            correction = 0

        for i_F in range(F_jumps):
            fstart = (self.fmax - self.fmin)/float(F_jumps) * i_F + self.fmin
            fend   = (self.fmax - self.fmin)/float(F_jumps) * ( i_F+1 ) + self.fmin
            fmid   = 0.5*(fend - fstart) + fstart - correction
            fmidl  = 0.5*(fend - fstart) + fstart + correction
            dTlocal  = int (np.ceil(
                (self.maxdt-1)                        *
                self.__ff2__(fstart, fend)            /
                self.__ff2__(self.fmin,   self.fmax)
                ))
            
            for i_dT in range(dTlocal+1):
                dT_mid = int (round (i_dT * (
                    self.__ff2__(fstart, fmid) / 
                    self.__ff2__(fstart, fend)
                    )))
                
                dT_midl = int (round (i_dT * (
                    self.__ff2__(fstart, fmidl)  /
                    self.__ff2__(fstart, fend)
                    )))

                dT_rest = int (i_dT - dT_midl)
                
                i_T_min = 0
                i_T_max = dT_midl
                Output[i_F,i_dT,i_T_min:i_T_max]           =    \
                self.State[2*i_F, dT_mid,i_T_min:i_T_max]
                
                
                i_T_min = dT_midl
                i_T_max = self.T
                Output[i_F,i_dT,i_T_min:i_T_max]                =                    \
                self.State [2*i_F, dT_mid,i_T_min:i_T_max]      +                    \
                self.State [2*i_F+1, dT_rest,i_T_min - dT_midl:i_T_max-dT_midl]
        self.State = Output
    
    def run (self, Image):
        self.F, self.T = Image.shape
        self.f = int ( np.log2 (self.F) )
        self.fres = (self.fmax - self.fmin)/float(self.F)
        #
        self.__initialization__ (Image)
        for it in range (1, self.f+1):
            self.__iteration__ (it)
        #
        [F,dT,T] = self.State.shape
        return np.reshape (self.State, [dT,T])
        

