"""defaults go here
even fakes carry defaults
"""

TSAMP    = 781.25E-6
FCH1     = 361.9414488190918
FOFF64   = -0.65525515625
FOFF4K   = -0.01023836181640625
NCHAN4K  = 4096
NCHAN64  = 64
NDM      = 256
DMOFF    = 0.19607843137254832
DMWIDTH  = 50
NBITS    = 8
ANTENNA  = 00
RA       = 0.00
DEC      = 0.00
SOURCE   = "ARTIFICIAL"
TSTART   = 54321.012345
EPOCH    = 1000000000.0
I0       = 1000000010.0
# I1 is I0 + dur
# GROUP    = "19000101_211202"

class DefDBSON(object):
    """Fake dbson preloaded with defaults.
        
        XXX
        Few things are still to be adjusted
        at the site
    """
    def __init__ (self, randid = 10101, nsamp = 256):
        # time
        self.tstart      = TSTART
        self.tsamp       = TSAMP
        self.nsamps      = nsamp
        self.duration    = nsamp * TSAMP
        self.peak_time   = 0.0
        # frequency
        self.fch1        = FCH1
        self.foff        = FOFF64
        self.nchans      = NCHAN64
        # indices
        self.i0          = I0
        self.i1          = I0 + self.duration
        self.epoch       = EPOCH
        # parameters
        self.nbits       = NBITS
        self.antenna     = ANTENNA
        self.source_name = SOURCE
        self.ra          = RA
        self.dec         = DEC
        self.group       = "19000101_{0:06d}".format(randid)
        # dms
        self.ndm         = NDM
        self.dmoff       = DMOFF
        self.dms         = 0.0
        #
        self.sn          = 0.0
        self.dm          = 0.0
        self.width       = 0.0
        self.bt          = None
        self.dd          = None


class DefFBSON(object):
    """Fake fbson preloaded with defaults.
        
        XXX
        Few things are still to be adjusted
        at the site
    """
    def __init__ (self, randid = 10101):
        # time
        self.tstart      = TSTART
        self.tsamp       = TSAMP
        self.nsamps      = 0
        self.duration    = 0.0
        self.peak_time   = I0 - EPOCH
        self.tpeak       = 0.0
        # frequency
        self.fch1        = FCH1
        self.foff        = FOFF4K
        self.nchans      = NCHAN4K
        # indices
        self.i0          = I0
        self.i1          = I0 + self.duration
        self.epoch       = EPOCH
        # parameters
        self.nbits       = NBITS
        self.antenna     = ANTENNA
        self.source_name = SOURCE
        self.ra          = RA
        self.dec         = DEC
        self.group       = "19000101_{0:06d}".format(randid)
        #
        self.sn          = 0.0
        self.dm          = 0.0
        self.width       = 0.0
        self.fb          = None
