# coding: utf-8
import os
import gc
import sys
import numpy as np
from scipy.stats import skew, kurtosis
# my trishul
import trishul.fbson  as tfb
import trishul.dedisp as tdd
import trishul.plot   as tp
# plotting
import matplotlib
matplotlib.rcParams.update ({'font.size': 8})
matplotlib.use('Agg')
import matplotlib.pyplot as plt
ROOT="/lustre/aoc/observers/nm-10966/fbsons/vd11core"
PLOTDIR=ROOT+"/candplot/"
FEADIR=ROOT+"lyonfea/"
textstr = '{:^13}{:^13}{:^13}\n{:^13}{:^13.3e}{:^13.3e}\n{:^13}{:^13.3e}{:^13.3e}\n{:^13}{:^13.3f}{:^13.3f}\n{:^13}{:^13.3f}{:^13.3f}'
fstr = "{0},{1},{2},{3},{4},{5},{6},{7}\n"
if __name__ == "__main__":
    if len(sys.argv) == 1:
        fb = '/home/shining/work/vlite/searches/snestimator/20191121_045949_muos_ea99_dm56.75_sn106.58.fbson'
    else:
        fb = sys.argv[1]    
    f  = tfb.FBSON (fb, loadFB=True)
    bf,ext = os.path.splitext (os.path.basename (fb))
    dx = tdd.DedispBundle (f)
    dms = dx['dms']
    bt = dx['bt']
    dmcount, tscount = bt.shape
    times = np.arange (tscount) * f.tsamp
    # slicing
    dt = int (0.5 * dmcount)
    # peak_time correction
    try:
      ipt = int (f.tpeak // f.tsamp)
    except:
      idt,ipt = np.where (bt == bt.max())
      idt = idt[0]
      ipt = ipt[0]
    ###
    dmslice = bt[:,ipt]
    tsslice = bt[dt]
    # axes
    fig  = plt.figure ()
    print type(dx)
    #tp.Candplot (f, **dx, fig=fig)
    plt.savefig (os.path.join(PLOTDIR, bf)+".png")
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
