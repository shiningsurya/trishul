"""
Dedispersion routines
"""
import numpy as np

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

def FreqTable (nchans, f0, foff):
    '''Handy function to create freq table'''
    return f0 + ( np.arange (nchans) * foff )

def DedispFBSON (x):
    '''Dedisperses fbson object'''
    if x.fb is None:
        raise ValueError ("Filterbank not loaded!")
    freqs = FreqTable (x.nchans, x.fch1, x.foff)
    idelays = Delays (x.dm, freqs)
    return ddfb (x.fb, idelays)
