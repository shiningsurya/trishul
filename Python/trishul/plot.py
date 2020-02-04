"""
Plotting routines
"""
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

from . import dedisp as tdd

def BTCPlot (bt, dm=None, t=None, fig=None):
    '''
    bt is bt array
    '''
    fig = fig or plt.figure ()
    axbt = fig.add_axes([0.1, 0.1, 0.6, 0.6])
    # [0.1, 0.6, 0.1, 0.8]
    axpt = fig.add_axes([0.1, 0.7, 0.6, 0.2], sharex=axbt)
    # [0.1, 0.6, 0.8, 0.9]
    axdt = fig.add_axes([0.7, 0.1, 0.2, 0.6], sharey=axbt)
    # [0.6, 0.9, 0.1, 0.8]
    axtt = fig.add_axes([0.7, 0.7, 0.2, 0.2])
    # [0.6, 1.0, 0.8, 1.0]
    # plots
    axbt.imshow (bt, origin="lower", aspect="auto",
            extent=[times[0], times[-1], dms[0], dms[-1]]
    )
    axbt.set_xlabel ("Time (s)")
    axbt.set_ylabel ("DM (pc/cc)")
    axpt.plot (times, tsslice)
    axpt.text (0.75,0.9,"Time slice={0:3.2f}s".format(f.peak_time), transform=axpt.transAxes)
    axpt.xaxis.tick_top()
    axpt.set_yticks ([])
    axdt.plot (dmslice, dms)
    axdt.text (0.2,0.97,"DM slice={0:3.2f}pc/cc".format(f.dm), transform=axdt.transAxes)
    axdt.yaxis.tick_right ()
    axdt.set_xticks ([])

def BTPlot (bt, dms, times, fig=None):
    '''
    BowTie plot
    Needs dm array and times array
    '''
    fig = fig or plt.figure ()
    axbt = fig.add_axes([0.1, 0.1, 0.8, 0.8])
    axbt.imshow (bt, origin="lower", aspect="auto",
            extent=[times[0], times[-1], dms[0], dms[-1]]
    )
    axbt.set_xlabel ("Time (s)")
    axbt.set_ylabel ("DM (pc/cc)")

def FBPlot (fb, freqs, times, fig=None):
    '''
    Filterbank plot
    Needs freq array and times array
    '''
    fig = fig or plt.figure ()
    axbt = fig.add_axes([0.1, 0.1, 0.8, 0.8])
    axbt.imshow (fb, origin="lower", aspect="auto",
            extent=[times[0], times[-1], freqs[0], freqs[-1]]
    )
    axbt.set_xlabel ("Time [s]")
    axbt.set_ylabel ("Freq [MHz]")

def Candplot (fbson, dms=None, delays=None, bt=None, dd=None, fig=None):
    '''Candidate plot
    Arguments
    ---------
    fbson: instance of trishul.fbson.FBSON

    fig: plt.figure instance
    '''
    fig = fig or plt.figure()
    # main computation steps
    if dms is None or delays is None:
        dms, delays = tdd.DMRanger (fbson)
    if bt is None:
        bt = tdd.BowTie (fbson, delays)
    if dd is None:
        dd = tdd.DedispFBSON (fbson)
    # aux steps
    btimes = np.arange (bt.shape[1]) * fbson.tsamp
    dtimes = np.arange (dd.shape[1]) * fbson.tsamp
    dfreqs = tdd.FreqTable (fbson)
    tslice = bt.shape[0]//2
    try:
        fslice = int ( fbson.tpeak//fbson.tsamp )
    except:
        fslice = int ( fbson.peak_time // fbson.tsamp )
    # axes objects
    axdd = fig.add_subplot (2,2,3)                                     # dedispersed filterbank
    axbt = fig.add_subplot (2,2,1, sharex=axdd, projection='3d')       # bow-tie plan
    axst = fig.add_subplot (2,2,2, sharex=axdd)                        # s/n (t)
    axsd = fig.add_subplot (2,2,4)                                     # s/n (d)
    # plotting
    axdd.imshow (dd, aspect='auto', extent=[dtimes[0], dtimes[-1], dfreqs[0], dfreqs[-1]], origin='lower')
    axdd.set_xlabel ("Time [s]")
    axdd.set_ylabel ("Freq [MHz]")
    # --
    axst.step (btimes, bt[tslice], 'k')
    axst.set_xlabel ("Time [s]")
    axst.spines['left'].set_visible (False)
    axst.spines['right'].set_visible (False)
    axst.spines['top'].set_visible (False)
    axst.set_yticks ([])
    # --
    axsd.step (dms, bt[:,fslice], 'r')
    axsd.set_xlabel ("DM [pc/cc]")
    axsd.set_yticks ([])
    axsd.spines['left'].set_visible (False)
    axsd.spines['right'].set_visible (False)
    axsd.spines['top'].set_visible (False)
    # --
    tmesh, dmesh = np.meshgrid (btimes, dms)
    # print ("proj=", axbt.get_proj())
    # axbt.get_proj = lambda : np.dot (Axes3D.get_proj(axbt), np.diag([0.75,0.5,1.5,1.]))
    axbt.plot_surface (tmesh, dmesh, bt, cmap=plt.cm.jet, rcount=100, ccount=100)
    axbt.set_xlabel ("Time [s]")
    axbt.set_ylabel ("DM [pc/cc]")
    axbt.set_zticks ([])
    axbt.zaxis.set_pane_color ((1.0,1.0,1.0,0.0))
    axbt.xaxis.set_pane_color ((1.0,1.0,1.0,0.0))
    axbt.yaxis.set_pane_color ((1.0,1.0,1.0,0.0))
    axbt.xaxis._axinfo['grid']['color'] = (1,1,1,0)
    axbt.yaxis._axinfo['grid']['color'] = (1,1,1,0)
    axbt.zaxis._axinfo['grid']['color'] = (1,1,1,0)
    axbt.view_init (65, 270)
    # text
    axst.text (0.9,1.0,fbson.__str__(), horizontalalignment='right', verticalalignment='top', transform=axst.transAxes)
    # last step
    fig.tight_layout()
    
