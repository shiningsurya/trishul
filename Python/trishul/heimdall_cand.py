'''
Class to read candidates written by Heimdall

Part of Trishul
'''
import re
import os
import copy

import numpy as np
import datetime as dt

import astropy.time as at
from astropy.coordinates import SkyCoord


TSAMP = 781.25E-6
DATEFMT = "%Y%m%d_%H%M%S"
# REGEXP
CANDF_PATTERN = "(?P<dt>\d{8}\_\d{6})\_muos\_(?P<ant>ea\d{2})[\_kur]*\.cand"
reCF = re.compile (CANDF_PATTERN)
# sn (float) - index (uint) - time (float) - width (uint)  - dmindex (uint) - dm (float) - group# (uint) - i0 (uint) - i1 (uint)
CAND_PATTERN = "(?P<sn>\d+[.]?\d*)\s+\d+\s+(?P<pt>\d+[.]?\d*)\s+(?P<wd>\d+)\s+\d+\s+(?P<dm>\d+[.]\d*)\s+\d+\s+\d+\s+\d+"
reCP = re.compile (CAND_PATTERN)
RADEC_PATTERN = "(?P<ra>\d+[.]?\d*)\s*(?P<dec>[+-]?\d+[.]?\d*)\n"
reRD = re.compile (RADEC_PATTERN)


def ExtractDTandAnt(fname):
    """To extract the datetime and antenna"""
    # 20180521_162250_muos_ea02_kur.cand
    # 20180521_162250_muos_ea02.cand
    m = reCF.findall (fname)
    date, ant = None, None
    if not m:
        print ( "Error reading Candidate ", fname )
    else:
        date = dt.datetime.strptime (m[0][0], DATEFMT)
        ant  = m[0][1]
    return date, ant

class CandidateData:
    '''
    Container class containing the candidates

    Attributes
    ----------
    n : int
        Number of candidates
    sn : array, float
        Signal to Noise of candidates in an array
    width : array, float
        Width in times of candidates in an array
    peak_time : array, float
        Peak times in an array
    DM : array, float
        Dispersion measure in an array
    filename : str
        Name of the filename
    '''
    def __init__(self, ff, tsamp=0.):
        '''
        Arugments
        ---------
        ff: str
            Name of the file
        tsamp: float (default=None)
            Sampling time.
            Heimdall candidates do not provide tsamp info
        '''
        self.tsamp = tsamp

        with open(ff, 'r') as ffo:
            flines = ffo.read()
        self.__reader__ (flines)

    def __enter__ (self):
        return self

    def __exit__(self, type, value, traceback):
        del self

    def __reader__ (self, flines):
        """When files are read"""
        # regex matching
        cands = reCP.findall (flines)
        # loading
        self.n           = len(cands)
        self.sn          = np.zeros(self.n, dtype=np.float32)
        self.boxcar      = np.zeros(self.n, dtype=np.uint16)
        self.width       = np.zeros(self.n, dtype=np.float32)
        self.peak_time   = np.zeros(self.n, dtype=np.float32)
        self.dm          = np.zeros(self.n, dtype=np.float32)
        # index 
        for i, l in enumerate(cands):
            self.sn        [ i ]   = float( l[0] )
            self.peak_time [ i ]   = float( l[1] )
            self.boxcar    [ i ]   = int  ( l[2] ) 
            self.width     [ i ]   = float( l[2] ) * TSAMP
            self.dm        [ i ]   = float( l[3] )


def FilterCandidateData(cdata, sncut=[0,10], dmcut=[0,100]):
    '''
    To create a new copy of CandidateData instance and
    apply filtering
    '''
    # copying
    ret = copy.deepcopy(cdata)
    # filtering -- select deletes
    sn_selects = np.logical_or(ret.sn < sncut[0], ret.sn > sncut[1])
    dm_selects = np.logical_or(ret.dm < dmcut[0], ret.dm > dmcut[1])
    al_selects = np.where( np.logical_or(sn_selects, dm_selects) )
    # selection
    ret.sn = np.delete(ret.sn,al_selects)
    ret.dm = np.delete(ret.dm,al_selects)
    ret.width = np.delete(ret.width,al_selects)
    ret.peak_time = np.delete(ret.peak_time,al_selects)
    ret.n = ret.sn.size
    return ret

def WriteCandidateData(cdata, filename=None, tsamp=TSAMP):
    '''
    Writes out candidates in Heimdall format
    If filename is None, filename is constructed from cdata.
    DISCLAIMER: SOME DATA WAS LET GO WHILE CREATING CANDIDATE DATA
    HENCE, THIS FILE AND ORIGINAL FILE WON'T BE SAME
    '''
    # filename logic
    if filename is None:
        filename = "{0}_muos_{1}_fil.cand".format( cdata.tstart.strftime(DATEFMT) ,cdata.ant)
    # generating meta data
    peak_index = np.array( cdata.peak_time / tsamp , dtype=np.uint64)
    filterwidth = np.array(cdata.width / tsamp, dtype=np.uint64)
    i0 = np.array( peak_index - (0.5 * filterwidth), dtype=np.uint64 )
    i1 = np.array( peak_index + (0.5 * filterwidth), dtype=np.uint64 )
    # writing
    with open(filename,'w') as f:
        for i in xrange(cdata.n):
            line = ''
            line += "{0:3.2f}\t".format(cdata.sn[i])
            line += "{0:d}\t".format(peak_index[i])
            line += "{0:3.3f}\t".format(cdata.peak_time[i])
            line += "{0:1d}\t".format(filterwidth[i])
            line += "{0:d}\t".format(0)
            line += "{0:3.2f}\t".format(cdata.dm[i])
            line += "{0:d}\t".format(0)
            line += "{0:d}\t".format(i0[i])
            line += "{0:d}\n".format(i1[i])
            # write
            f.write(line)

class CoincideCandidateData(Candidates):
    '''
    Like CandidateData, 
    '''
    def __init__(self, r, antenna=None, _tstart=None):
        '''
        Arguments
        ---------
        r: list of 4 lists
            ordering:
            IPT, ISN, IDM, IWD
        '''
        # all the lens should be same
        assert len( set(map(len,r)) ) == 1
        self.peak_time = np.array(r[0])
        self.sn = np.array(r[1])
        self.dm = np.array(r[2])
        self.width = np.array(r[3])
        self.n = self.sn.size
        self.ant = antenna
        self.tstart = _tstart
