"""
All things to do with fake-fbson and fake-dbson

Why create fake-fbson when you can create fake-dbson
save one step and one hassle
"""

from .fakefb import Fakefb
from .dbson  import DBSON, WriteDBSON
from .dedisp import DedispBundle
from .defaults import DefFBSON


def TrueDBSON (sn, dm, width, pt = 0.1, nsamps=256, odir="./", fake=None, rms=4.0, seed=10101):
    """ True dbson 
    Args:
        sn (float)
        dm (float)
        width (float) in ms
        pt (float) in seconds peaktime
        fake : instance of Fakefb or it shall be created
        rms (float) noise floor
        seed  (hhmmss) in the group name
    """
    ret            = DefFBSON (seed)
    ret.sn         = sn
    ret.dm         = dm
    ret.width      = width/1e3
    ret.filename   = "{0}_muos_ea{1:02d}_dm{2:05.2f}_sn{3:05.2f}_wd{4:05.2f}.fbson".format(ret.group, ret.antenna, dm, sn, width)
    #
    ret.source_name = "TFAKE" # True FAKE
    fake = fake or Fakefb (ret.tsamp, ret.nchans, ret.fch1, ret.foff, rms)
    # XXX 0.3 added for 30 dm-delay for bowtie plane
    ret.fb       = fake(sn, dm, width/1e3, pt, 0.6)
    ret.nsamps   = ret.fb.nbytes
    ret.duration = ret.fb.shape[1] * ret.tsamp
    ret.tpeak    = pt
    #
    ddb = DedispBundle (ret)
    with DBSON(ret, ddb, sampout=nsamps) as dbs:
        WriteDBSON (dbs, odir)

