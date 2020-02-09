"""
Dedispersion routines
"""
import numpy as np
from .fdmt import FDMT 
from .incoherent import Dedisperser
from collections import namedtuple

def ddfb (x, delays):
    '''Incoherent de-dispersion

    Arguments
    ---------
    x: NumPy array (nsamps, nchans)
    delays: Numpy Array (nchans) delay in time index
        dtype=np.uint64

    Returns
    -------
    NumPy array (nsamps - maxdelay, nchans)
    '''
    nchans, nsamps = x.shape
    maxdelay = delays[-1]
    if nsamps < maxdelay:
        raise ValueError ("DM delay is more than data!")
    #
    ddnsamps = int( nsamps - maxdelay )
    ret = np.zeros ((nchans, ddnsamps), dtype=x.dtype)
    for isamp in range (ddnsamps):
        for ichan in range (nchans):
            ret[ichan,isamp] = x[ichan,int(isamp+delays[ichan])]
    return ret

def Delays (dm, freqs, tsamp=781.25E-6):
    '''Delays in time index'''
    nchans = freqs.size
    ret = np.zeros (nchans, dtype=np.uint64)
    f0 = freqs[0]
    if02 = 1.0/f0/f0
    df = freqs[1]- freqs[0]
    #
    for i in range (nchans):
        f1 = f0 + (i * df)
        if12 = 1.0/f1/f1
        ret[i] = dm * 4148.741601 * ( if12 - if02 ) / tsamp
    return ret

def MaxDelay (dms, freqs, tsamp=781.25E-6):
    f0 = freqs.max()
    if02 = 1.0/f0/f0
    f1 = freqs.min()
    if12 = 1.0/f1/f1
    delays = np.empty_like(dms, dtype=np.uint32)
    for i,d in enumerate (dms):
        delays[i] = d * 4148.741601 * ( if12 - if02 ) / tsamp
    return delays

def FreqTable (x):
    '''Handy function to create freq table'''
    return x.fch1 + ( np.arange (x.nchans) * x.foff )

def DMRanger (x, dmsize=256, dmwidth=50, return_index=True):
    # get dm
    dms = np.linspace (x.dm - 0.5*dmwidth, x.dm + 0.5*dmwidth, dmsize)
    idm = np.searchsorted (dms, x.dm)
    dms[idm] = x.dm
    freqs = FreqTable (x)
    fmin,fmax = freqs[-1], freqs[0]
    idelays  = MaxDelay (dms, freqs)
    if return_index:
        return dms, idelays
    else:
        return dms

def DMDelay (x):
    # freqs
    fmax = x.fch1
    if02 = 1.0/fmax/fmax
    fmin = fmax + (x.nchans*x.foff)
    if12 = 1.0/fmin/fmin
    return x.dm * 4148.741601 * ( if12 - if02 )


def Incoherent(x):
    '''Dedisperses fbson object'''
    if x.fb is None:
        raise ValueError ("Filterbank not loaded!")
    freqs = FreqTable (x)
    idelays = Delays (x.dm, freqs)
    return Dedisperser (x.fb, idelays)

def BowTie (x, idelays):
    if x.fb is None:
        raise ValueError ("FBSON filterbank is not loaded.")
    fff = FDMT (x.fch1, x.foff, x.nchans)
    return fff.Bowtie (x.fb, idelays)

def DedispBundle (x):
    ret                   = namedtuple ("DedispBundle", ['bt', 'dd', 'btimes', 'dtimes', 'dms', 'delays'])
    ret.dms, ret.delays   = DMRanger (x)
    ret.bt                = BowTie (x, ret.delays)
    ret.btimes            = np.arange (ret.bt.shape[1]) * x.tsamp
    ret.dd                = Incoherent (x)
    ret.dtimes            = np.arange (ret.dd.shape[1]) * x.tsamp
    return ret

