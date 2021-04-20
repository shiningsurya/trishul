"""
All things to do with a dbson
"""
import os
import numpy as np
import ubjson
import sys

from .fbson import fscrunch
from .fbson import FBSON 
#from .dedisp import DedispBundle

def scaler (x, flip=False):
    MIN = x.min()
    MAX = x.max()
    if flip:
        y = np.array( ( 255/(MAX-MIN) ) * (x.T.ravel() - MIN) , dtype=np.uint8 )
    else:
        y = np.array( ( 255/(MAX-MIN) ) * (x.ravel() - MIN) , dtype=np.uint8 )
    return y

__all__ = ["DBSON"]

def WriteDBSON (x, outdir = "./"):
    '''Writes DBSON 

    Arguments
    ---------
    x: DBSON
    '''
    ret  = dict () # this gets written
    #
    ret['time'] = dict ()
    for k in ['peak_time','tstart','tsamp','duration','nsamps']:
        ret['time'][k] = x.__dict__[k]
    #
    ret['frequency'] = dict ()
    for k in ['fch1','foff','nchans']:
        ret['frequency'][k] = x.__dict__[k]
    # 
    ret['indices'] = dict ()
    for k in ['i0','i1','epoch']:
        ret['indices'][k] = x.__dict__[k]
    # 
    ret['parameters'] = dict ()
    for k in ['nbits','antenna','source_name','ra','dec','group']:
        ret['parameters'][k] = x.__dict__[k]
    # 
    ret['dms'] = dict ()
    for k in ['dm1','dmoff','ndm']:
        ret['dms'][k] = x.__dict__[k]
    # 
    for k in ['sn','dm','width', 'filename']:
        ret[k] = x.__dict__[k]
    # data products
    ret['bt'] = scaler (x.bt).tobytes ()
    ret['dd'] = scaler (x.dd, flip=True).tobytes ()
    # writing
    fname,_ = os.path.splitext (x.filename)
    with open (os.path.join (outdir, "{0}.dbson".format(fname)),"wb") as f:
        ubjson.dump (ret, f)

class DBSON(object):
    '''DBSON object'''
    def __init__ (self, xf, dx=None,):
        '''init
        Arguments
        ---------
        x : str
            path to dbson, 
            path to fbson,
            FBSON object

        '''
        if isinstance (xf, str) and xf.endswith("dbson"):
            self.__read_dbson__ (xf)
        else:
            raise ValueError ("Input argument should be (str,FBSON, dbson)")

    def __enter__ (self):
        return self

    def __exit__(self, type, value, traceback):
        del self

    def __read_dbson__ (self, filename):
        self.filename = os.path.basename (filename)
        with open(filename, "rb") as f:
            x = ubjson.loadb(f.read())
        self._x_ = x
        for k,v in x.items():
            if k in ["sn", "dm", "width"]:
                self.__dict__[k] = v
        # 
        for sd in ["frequency", "time", "parameters", "indices", "dms"]:
            for k,v in x[sd].items():
                self.__dict__[k] = v
        # tsamp correction
        if self.tsamp >= 1:
            self.tsamp = self.tsamp / 1E6
        # data products
        _dd      = np.fromiter (x['dd'], dtype=np.uint8)
        _bt      = np.fromiter (x['bt'], dtype=np.uint8)
        self.dd  = np.reshape (_dd, (self.nsamps, self.nchans)).T
        self.bt  = np.reshape (_bt, (self.ndm, self.nsamps))

    def __str__ (self) :
        return  \
            "S/N: {0:3.2f}\nDM: {1:3.2f} pc/cc\nWidth: {2:3.2f} ms\nPeak Time: {3:3.2f} s\nAntenna: {4}\nSource: {5}\n"\
            "Tstart(MJD): {7:7.2f}\nNbits: {8:1d}\nNchans: {9:4d}".format(
            self.sn, self.dm, 1e3*self.width, self.peak_time, 
            self.antenna, self.source_name, self.duration, self.tstart, self.nbits, self.nchans)




