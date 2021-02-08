"""
Plotting routines
"""
import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mc

from skimage.measure import block_reduce

def BTC (bt=None, dm=None, t=None, fig=None):
    '''
    bt is bt array
                bt, =dd, 
                dm_axis=, f_axis, t_axis,
                fil_header = fh, (SIGMA, DM, TIME)
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

def candidate_plot (bt, dd, dm_axis, f_axis, tsamp, sdt, fig=None, path="./"):
    '''Candidate plot
    Arguments
    ---------
    fbson: instance of trishul.fbson.FBSON
                bt, =dd, 
                dm_axis=, f_axis, t_axis,
                fil_header = fh, (SIGMA, DM, TIME, PADDING)

    fig: plt.figure instance
    '''
    if fig:
        plt.clf()
    else:
        fig = plt.figure()
    t0 = sdt[2] - sdt[3]
    # axes objects
    axdd = fig.add_subplot (2,2,3)                                                          # dedispersed filterbank
    axbt = fig.add_subplot (2,2,1, sharex=axdd)                                             # bow-tie plane
    axst = fig.add_subplot (2,2,2)                                                          # s/n (t)
    axsd = fig.add_subplot (2,2,4)                                                          # s/n (d)
    # plotting
    t1 = t0 + (dd.shape[1]*tsamp)
    pdd = block_reduce (dd, (4,4), func=np.mean)
    axdd.imshow (pdd, aspect='auto', extent=[t0, t1, f_axis[0], f_axis[-1]], origin='lower', cmap='jet', norm=mc.LogNorm())
    axdd.set_xlabel ("Time [s]")
    axdd.set_ylabel ("Freq [MHz]")
    # --
    tslice = dm_axis.size//2
    fslice = int(sdt[3] // tsamp)
    #
    axst.step (t0+np.arange(bt.shape[1])*tsamp, bt[tslice], 'k')
    # axst.set_xlabel ("Time [s]")
    axst.spines['left'].set_visible (False)
    axst.spines['right'].set_visible (False)
    axst.spines['top'].set_visible (False)
    axst.set_yticks ([])
    axst.axvline (sdt[2], ls='--',c='r', alpha=0.5)
    # --
    axsd.step (dm_axis, bt[:,fslice], 'r')
    axsd.set_xlabel ("DM [pc/cc]")
    axsd.set_yticks ([])
    axsd.spines['left'].set_visible (False)
    axsd.spines['right'].set_visible (False)
    axsd.spines['top'].set_visible (False)
    axsd.axvline (sdt[1], ls='--',c='k', alpha=0.5)
    # --
    t1 = t0 + (bt.shape[1]*tsamp)
    pbt  = block_reduce (bt, (4,4), func=np.mean)
    axbt.imshow (pbt, cmap=plt.cm.jet, aspect='auto', extent=[t0, t1, dm_axis[0], dm_axis[-1]], origin='lower')
    # axbt.set_xlabel ("Time [s]")
    axbt.set_ylabel ("DM [pc/cc]")
    # text
    txtstr = "S/N: {sn:3.2f} DM: {dm:4.2f}\nUNIX: {epoch} Time: {time:4.2f}"
    fig.suptitle (txtstr.format(sn=sdt[0], dm=sdt[1], epoch=sdt[4], time=sdt[2]))
    fstr  = "{epoch:15.5f}_sn{sn:3.2f}_dm{dm:4.2f}.png"
    fig.savefig (os.path.join(path, fstr.format(sn=sdt[0], dm=sdt[1], epoch=sdt[4])), bbox_inches='tight', dpi=300)

