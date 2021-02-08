"""
Candidates IO

Part of Trishul
"""

import pandas as pd

HEIMDALL_COLS = ['sigma','index','time','boxcar','dm_index','dm','group_id','sample_start','sample_stop']
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
    return df
