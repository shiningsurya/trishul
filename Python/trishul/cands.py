"""
Candidates IO

Part of Trishul
"""

import numpy  as np
import pandas as pd

HEIMDALL_COLS = ['sigma','index','time','width_units','dm_index','dm','group_id','sample_start','sample_stop']
HEIMDALL_DT_FORMATS = {
        'datetime+beam': "%Y-%m-%d-%H:%M:%S_{beam}.cand",
        'datetime': "%Y-%m-%d-%H:%M:%S.cand",
}

PRESTO_SPS_COLS = ['dm', 'sigma', 'time', 'index', 'boxcar']

def read_heimdall_cands (filename,):
    """Reads candidates written by Heimdall

    Arguments
    ---------
    filename: str
        Filename of the candidate file
    parse: str
        Notes the filename format
        Can be either None or keys of HEIMDALL_DT_FORMATS
        #TODO
    """
    df = pd.read_csv (filename, delim_whitespace=True, names=HEIMDALL_COLS)
    return df

def read_presto_cands (filename,):
    """Reads candidates written by PRESTO:single_pulse_search

    Arguments
    ---------
    filename: str
        Filename of the candidate file
    """
    df = pd.read_csv (filename, delim_whitespace=True, comment='#', names=PRESTO_SPS_COLS)
    df['width_units'] = df.boxcar.apply (np.log2)
    return df

def read_cands (filename):
    """reads on its own"""
    if filename.endswith ('cand'):
        cands = read_heimdall_cands (filename)
    elif filename.endswith ('singlepulse'):
        cands = read_presto_cands (filename)
    else:
        raise ValueError ('Candidate file format not understood.')
    return cands

def masker (df, dmlow=None, dmhigh=None, snlow=None, snhigh=None, wdlow=None, wdhigh=None):
    """Applies mask"""
    mask = df.time >= 0.0       # all true mask
    #
    if dmlow:
        mask = mask & (df.dm >= dmlow)
    if dmhigh:
        mask = mask & (df.dm <= dmhigh)
    if snlow:
        mask = mask & (df.sigma >= snlow)
    if snhigh:
        mask = mask & (df.sigma <= snhigh)
    if wdlow:
        mask = mask & (df.width_units >= wdlow)
    if wdhigh:
        mask = mask & (df.width_units <= wdhigh)
    #
    return mask
