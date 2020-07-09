"""
Coadds triggers selecting (RA,DEC,DM) from input Pandas file
"""
from __future__ import print_function
import numpy as np
import matplotlib.pyplot as plt
import trishul.dbson as tdb
import os
import tqdm
import healpy as hp
#################################
PDFILE = "./al_070720_u1_selected.pkl"
DIR = "./dbsons/"
DBFILE = "./all.list"
with open (DBFILE) as f:
    DBSONS = [a.strip() for a in f.readlines()]
tDBSONs = tqdm.tqdm (DBSONS, unit='dbs', desc='DBSON')
df = pd.read_pickle (PDFILE)
NDIM, _ = df.shape
#################################
NSIDE = 16
NPIX  = hp.nside2npix (NSIDE)
DPR   = 180.0 / np.pi
NDM   = 950
#################################
DM_OFFSET = 50.0
DM_SCALE  = 1.0
N    = np.zeros ((NDIM,), dtype=np.uint64)
BT   = np.zeros ((NDIM, 256, 256), dtype=np.float64)
DD   = np.zeros ((NDIM, 64, 256), dtype=np.float64)
PP   = np.zeros ((NDIM, 256), dtype=np.float64)
#################################
## work loop
for f in tDBSONs:
    with tdb.DBSON (os.path.join (DIR, f)) as dbf:
        RADEG = dbf.ra * DPR
        DEDEG = dbf.dec * DPR
        ipix  = hp.ang2pix (NSIDE, RADEG, DEDEG, lonlat=True)
        idm   = int ( (dbf.dm - DM_OFFSET)/DM_SCALE )
        im    = (df.ipix == ipix) & (df.idm == idm)
        if im.sum() > 1:
            print ("This shouldn't have happened!")
        elif im.sum() < 0:
            continue
        idx = im[im].index[0]
        ## updates
        BT[idx] += (dbf.bt/255.)
        DD[idx] += (dbf.dd/255.)
        PP[idx] += (dbf.pp/255.)
        N[idx]  += 1
#################################
BT.dump ("./_070720_u1_selected_bt.npy")
DD.dump ("./_070720_u1_selected_dd.npy")
PP.dump ("./_070720_u1_selected_pp.npy")
N.dump  ("./_070720_u1_selected_n.npy")
