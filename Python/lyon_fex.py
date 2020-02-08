# coding: utf-8
import os
import gc
import sys
import numpy as np
from scipy.stats import skew, kurtosis
# my trishul
import trishul.fbson as tfb
import trishul.dedisp as tdd
# plotting
import matplotlib
matplotlib.rcParams.update ({'font.size': 8})
matplotlib.use('Agg')
import matplotlib.pyplot as plt
PLOTDIR="/lustre/aoc/observers/nm-10966/lyonpng/"
FEADIR="/lustre/aoc/observers/nm-10966/lyonfea/"
textstr = '{:^13}{:^13}{:^13}\n{:^13}{:^13.3e}{:^13.3e}\n{:^13}{:^13.3e}{:^13.3e}\n{:^13}{:^13.3f}{:^13.3f}\n{:^13}{:^13.3f}{:^13.3f}'
fstr = "{0},{1},{2},{3},{4},{5},{6},{7}\n"
if __name__ == "__main__":
    if len(sys.argv) == 1:
        fb = '/home/shining/work/vlite/searches/snestimator/20191121_045949_muos_ea99_dm56.75_sn106.58.fbson'
    else:
        fb = sys.argv[1]    
    f  = tfb.FBSON (fb, loadFB=True)
    bf,ext = os.path.splitext (os.path.basename (fb))
    dms, delays = tdd.DMRanger(f)
    gc.collect ()
    bt = tdd.BowTie (f, delays)
    gc.collect ()
    dmcount, tscount = bt.shape
    times = np.arange (tscount) * f.tsamp
    # slicing
    dt = int (0.5 * dmcount)
    # peak_time correction
    if f.peak_time < 0.0:
      pt = delays[dt]  - delays[0]
      # assuming one dm-delay correction
    else:
      pt = int ( f.peak_time / f.tsamp )
    ###
    [idt],[ipt] = np.where (bt == bt.max())
    ###
    dmslice = bt[:,ipt]
    tsslice = bt[dt]
    # axes
    fig  = plt.figure ()
    fig.suptitle (os.path.basename(f.filename))
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
    axpt.text (0.75,0.9,"Time slice={0:3.2f}s".format(pt*f.tsamp), transform=axpt.transAxes)
    axpt.xaxis.tick_top()
    axpt.set_yticks ([])
    axdt.plot (dmslice, dms)
    axdt.text (0.2,0.97,"DM slice={0:3.2f}pc/cc".format(f.dm), transform=axdt.transAxes)
    axdt.yaxis.tick_right ()
    axdt.set_xticks ([])
    # text
    ss = textstr.format (
        "Moments",     "TimeSlice",       "DMSlice",
        "Mean",        np.mean(tsslice),  np.mean(dmslice),
        "Variance",    np.var(tsslice),   np.var(dmslice),
        "Skewness",    skew(tsslice),     skew(dmslice),
        "Kurtosis",    kurtosis(tsslice), kurtosis(dmslice)
    )
    axtt.axis ("off")
    axtt.text (0.0,1.0, ss, transform=axtt.transAxes,
    horizontalalignment="left", verticalalignment="top"
            )
    plt.savefig (os.path.join(PLOTDIR, bf)+".png", dpi=300)
    # save
    with open(os.path.join(FEADIR, bf)+".lyon", "w") as f:
      f.write(fstr.format (
        np.mean(tsslice),  np.mean(dmslice),
        np.var(tsslice),   np.var(dmslice),
        skew(tsslice),     skew(dmslice),
        kurtosis(tsslice), kurtosis(dmslice)
      ))
      # tmean, dmean, tvar, dvar, tskew, dskew, tkur, dkur

# np.where (bt == bt.max())
# dmslice = bt[128]
# tislice = bt[:,130]
# plt.clf()
# plt.plot (dmslice)
# plt.plot (tislice)
# np.mean ( dmslice)
# np.var (dmslice)
# skew (dmslice)
# skew (tislice)
# kurtosis (dmslice)
# kurtosis (tislice)
# kurtosis (tislice) - 3
# np.var (dmslice), np.var (tislice)
# bt.shape
# plt.clf()
# plt.imshow (np.var(bt,axis=1), origin='lower', aspect='auto')
# plt.clf()
# plt.plot ( np.var (bt, axis=1))
# plt.clf()
# plt.plot ( np.var (bt, axis=1), legend="Variance")
# plt.clf()
# plt.plot ( np.var (bt, axis=1), label="Variance")
# plt.plot ( np.mean (bt, axis=1), label="Variance")
# plt.plot ( skew (bt, axis=1), label="Variance")
# plt.plot ( kurtosis (bt, axis=1), label="Variance")
# plt.yscale ('log')
# plt.xlabel ("DM Index")
# plt.xlim(0,255)
# plt.legend (loc='best')
# plt.clf ()
# plt.plot ( np.var (bt, axis=1), label="Variance")
# plt.plot ( np.mean (bt, axis=1), label="Mean")
# plt.plot ( kurtosis (bt, axis=1), label="Kurtosis")
# plt.plot ( skew (bt, axis=1), label="Skewness")
# plt.yscale ("log")
# plt.xlim(0,255)
# plt.xlabel ("DM Index")
# k
# plt.ylabel ("Central moments")
# plt.legend (loc="best")
# plt.grid (True)
# plt.grid (which='both')
# plt.grid (which='both')
# get_ipython().run_line_magic('pinfo', 'plt.grid')
# plt.grid (which='both', axis='both')
# plt.grid (which='both', axis='both')
# plt.grid (b=True,which='both', axis='both')
# plt.title ("BowTie plane - DM Slice")
# plt.savefig ("bt_dmslice.pdf",dpi=300)
# plt.clf()
# plt.plot ( np.mean (bt, axis=0), label="Mean")
# plt.plot ( skew (bt, axis=0), label="Skewness")
# plt.plot ( kurtosis (bt, axis=0), label="Kurtosis")
# plt.plot ( np.var (bt, axis=0), label="Variance")
# plt.yscale ("log")
# plt.legend (loc="best")
# bt.shape
# plt.xlim(0,353)
# plt.xlabel ("Time Index")
