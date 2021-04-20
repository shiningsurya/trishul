
"""



"""

import numpy as np

from trishul import fdmt          as trishul_fdmt
from trishul import incoherent    as trishul_incoherent

DM_FAC = 4.148741601E-3

class BTDD:
    """
    Class to streamline computation of bowtie plane and de-dispersed filterbank.

    FDMT algorithm requires the number of DM-trials to be less than number-of-chans. 

    This class makes use of the optimization trick.
    Bowtie (A..B)  --> Incoherent (A) & Bowtie (B-A)
    Incoherent (C) --> Incoherent (A) & Incoherent (C-A)
    where A,B,C are delays

    For simplicity sake, the steps are numbered:
    -1-   Incoherent (A)
    -2.1- Incoherent (C-A) 
    -2.2- Bowtie (B-A)
    """
    def __init__ (self,
            fch1=None,foff=None,nchans=None,
            tsamp=None, 
            ndm=None,
            verbose=False,
        ):
        """

        Arguments
        ---------
        -All freq in MHz
        """
        self.v       = verbose
        ##
        self.fch1    = fch1
        self.foff    = foff
        self.nchans  = nchans
        self.f_axis  = self.fch1 + (np.arange (nchans, dtype=np.float32) * self.foff)
        ##
        self.fullband_delay      = DM_FAC * (  (self.f_axis[self.nchans-1]/1E3)**-2 - (self.f_axis[0]/1E3)**-2 )
        self.inband_delay        = np.zeros (nchans, dtype=np.float32)
        for i,f in enumerate (self.f_axis):
            self.inband_delay[i] = DM_FAC * ( (f/1E3)**-2 - (self.f_axis[0]/1E3)**-2 )
        ##
        self.tsamp   = tsamp
        self.ndm     = ndm
        ##
        self.fdmt    = trishul_fdmt.FDMT (self.fch1, self.foff, self.nchans)
        self.max_d   = 0
        ##

    def __call__ (self, dm, dit=1):
        """
        Sets DM and calculates the delay arrays

        Arguments
        ---------

        dm: float
            Dispersion measure of the candidate

        dit: int
            Decimation in time performed while reading

        """
        ## 
        itsamp              = self.tsamp * dit
        ## reset
        self.__reset__ ()
        ## for step 1
        ddm                 = int (dm * self.fullband_delay  / itsamp )
        step1_ddm           = int(max (ddm-128, 0))
        step1_dm            = step1_ddm * itsamp / self.fullband_delay
        self.step1_delays        = np.uint64 (self.inband_delay * step1_dm / itsamp )
        ## for step 2.1
        step21_dm           = dm - step1_dm
        self.step21_delays       = np.uint64 (self.inband_delay * step21_dm / itsamp )
        ## for step 2.2
        ## this bowtie computation is {0..256}
        ##
        self.max_d          = 256 + self.step1_delays[self.nchans-1] + 1
        if self.v:
            print (f" d_delay={self.max_d}")

        # print (f"  dm step1={step1_dm} step2={step21_dm} sum={step1_dm+step21_dm} org={dm}")
        # print (f" ddm step1={step1_ddm} org={ddm}")
        # print (self.step1_delays)
        # print (self.step21_delays)

    def __reset__ (self):
        self.max_d           = 0
        self.step1_delays    = None
        self.step21_delays   = None

    def work (self, fb):
        """
        fb is (nchans, nsamps)

        No slicing is done here. That should be done by the caller.
        """
        ## actual computation
        step1_dd            = trishul_incoherent.Dedisperser (fb, self.step1_delays)
        step21_dd           = trishul_incoherent.Dedisperser (step1_dd, self.step21_delays)
        step22_bt           = self.fdmt.Bowtie_maxdt (step1_dd, 256)

        ## return
        return step22_bt, step21_dd
