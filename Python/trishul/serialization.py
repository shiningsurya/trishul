"""

I make use of `python-ubjson`

Universal Binary JSON
"""
import os
import sys

import ubjson

import numpy as np

__all__ = ["read_dbson", "write_dbson"]

def read_dbson (fname):
    """
    Reads dbson 
    """
    with open(fname, 'rb') as f:
        return to_dbson (ubjson.loadb (f.read()))


def write_dbson (fname, obj):
    """
    Writes dbson. 
    """
    with open (fname, "wb") as f:
        ubjson.dump (from_dbson(obj), f)


def to_dbson (x):
    """Given ub-serialized data read into python dictionary"""
    ret = dict ()
    ## copy everything else except payloads
    for k in ['sn', 'dm', 'time', 'width',]:
        ret[k]    = x[k]

    ret['label']  = x['label']

    ## filterbank headers
    for k in ['src_raj','src_dej','tsamp','fch1','foff','tstart']:
        ret[k]    = x[k]

    for k in ['nchans', 'nbits']:
        ret[k]    = x[k]

    ## payloads
    xdd           = x['dd']
    ret['dd']     = np.frombuffer (xdd['data'], xdd['dtype']).reshape (xdd['shape'])

    xdd           = x['bt']
    ret['bt']     = np.frombuffer (xdd['data'], xdd['dtype']).reshape (xdd['shape'])

    ## string
    ret['id']         = x['id']
    ret['filname']    = x['filname']

    return ret


def from_dbson (x):
    """Write into ub-serialized"""
    ret = dict ()
    for k in ['sn', 'dm', 'time', 'width',]:
        ret[k] = float(x[k])

    ret['dd']  = {'data':x['dd'].tobytes(), 'shape':x['dd'].shape, 'dtype':str(x['dd'].dtype)}
    ret['bt']  = {'data':x['bt'].tobytes(), 'shape':x['bt'].shape, 'dtype':str(x['bt'].dtype)}

    ret['label'] = int (x['label'])

    ## filterbank headers
    for k in ['src_raj','src_dej','tsamp','fch1','foff','tstart']:
        ret[k] = float(x[k])

    for k in ['nchans', 'nbits']:
        ret[k]    = int(x[k])

    ## string
    ret['id']         = x['id']
    ret['filname']    = x['filname']

    return ret

