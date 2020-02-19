"""
All things to do with a dbson
"""
import os
import numpy as np
import ubjson
import sys

from .fbson import fscrunch
from .fbson import FBSON 
from .dedisp import DedispBundle

def scaler (x):
    MIN = x.min()
    MAX = x.max()
    y = np.array( ( 255/(MAX-MIN) ) * (x - MIN) , dtype=np.uint8 )
    return y

__all__ = ["DBSON"]

def WriteDBSON (x):
    '''Writes DBSON 

    Arguments
    ---------
    x: DBSON
    '''
    ret  = dict () # this gets written
    #
    ret['time'] = dict ()
    for k in ['peak_time','tpeak','tstart','tsamp','duration','nsamps']:
        ret['time'][k] = x.__dict__[k]
    #
    ret['frequency'] = dict ()
    for k in ['fch1','foff','nchans']:
        ret['frequency'][k] = x.__dict__[k]
    # 
    ret['indices'] = dict ()
    for k in ['i0','i1','epoch','fbsize']:
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
    ret['dd'] = scaler (x.dd).tobytes ()
    # writing
    fname,_ = os.path.splitext (x.filename)
    with open ("{0}.dbson".format(fname),"wb") as f:
        ubjson.dump (ret, f)

class DBSON(object):
    '''DBSON object'''
    def __init__ (self, xf, dx=None, chanout=64, sampout=256):
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
        elif isinstance(xf, str) or  isinstance (xf, FBSON):
            self.__read_fbson__ (xf, dx=dx, chanout=chanout, sampout=sampout)
        else:
            raise ValueError ("Input argument should be (str,FBSON, dbson)")

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
        _dd      = np.frombuffer (x['dd'], dtype=np.uint8)
        _bt      = np.frombuffer (x['bt'], dtype=np.uint8)
        self.dd  = np.reshape (_dd, (self.nchans, self.nsamps))
        self.bt  = np.reshape (_bt, (self.ndm, self.nsamps))

    def __read_fbson__(self, xf, dx=None, chanout=64,sampout=256):
        '''Read from fbson

        Arguments
        ---------
        xf : str or FBSON
        dx : DedispFBSON
        '''
        # FBSON setup
        if isinstance (xf, str):
            x = FBSON(xf, 1)
        else:
            x = xf
        # DedispBundle setup
        if dx is None:
            dx = DedispBundle (x)
        samps = min (dx.bt.shape[1], dx.dd.shape[1])
        # processing
        pt = 0.1  # peak_time
        try:
            pt = x.tpeak
        except:
            _, ipt = np.where (dx.bt == dx.bt.max())
            pt = ipt[0] * x.tsamp
        fslice = int ( pt//x.tsamp )
        # crunching, slicing
        dd64 = fscrunch (dx.dd, chanout)
        istart = int ( fslice - ( sampout//2 ) )
        istop  = int ( fslice + ( sampout//2 ) )
        if istart < 0:
            istart = 0
        if istop > samps:
            istop = samps-1
        self.nsamps = istop - istart
        # putting stuff in
        # data
        self.dd = dd64[:,istart:istop]
        self.bt = dx.bt[:,istart:istop]
        # dm
        self.dm1  = dx.dms[0]
        self.dmoff= dx.dms[1] - dx.dms[0]
        self.ndm  = dx.dms.size
        # parameters
        for k in ['ra', 'dec', 'group', 'source_name', 'antenna', 'nbits']:
            self.__dict__[k] = x.__dict__[k]
        # indices
        for k in ['i0', 'i1', 'epoch']:
            self.__dict__[k] = x.__dict__[k]
        self.fbsize = self.nsamps * chanout * x.nbits / 8
        # frequency
        self.fch1   = x.fch1
        self.foff   = x.foff * chanout / x.nchans
        self.nchans = chanout
        # time
        for k in ['tsamp','peak_time']:
            self.__dict__[k] = x.__dict__[k]
        cut = istart * x.tsamp
        if pt < cut:
            raise ValueError ("[EE] shouldn't happen error")
        self.tpeak = pt - cut
        self.tstart = x.tstart + (self.tpeak/86400.0) 
        self.duration = sampout * x.tsamp
        # last
        for k in ['sn','dm','width','filename']:
            self.__dict__[k] = x.__dict__[k]

    def __str__ (self) :
        return  \
            "S/N: {0:3.2f}\nDM: {1:3.2f} pc/cc\nWidth: {2:3.2f} ms\nPeak Time: {3:3.2f} s\nAntenna: {4}\nSource: {5}\n"\
            "Tstart(MJD): {7:7.2f}\nNbits: {8:1d}\nNchans: {9:4d}".format(
            self.sn, self.dm, 1e3*self.width, self.tpeak, 
            self.antenna, self.source_name, self.duration, self.tstart, self.nbits, self.nchans)




