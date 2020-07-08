"""
Counts triggers on (RA,DEC, DM) dimension 
"""
from __future__ import print_function
import numpy as np
import matplotlib.pyplot as plt
import trishul.dbson as tdb
import os
import tqdm
import healpy as hp
#################################
DIR = "./dbsons/"
DBFILE = "./all.list"
with open (DBFILE) as f:
    DBSONS = [a.strip() for a in f.readlines()]
tDBSONs = tqdm.tqdm (DBSONS, unit='dbs', desc='DBSON')
#################################
NSIDE = 16
NPIX  = hp.nside2npix (NSIDE)
DPR   = 180.0 / np.pi
NDM   = 950
NDIM  = NPIX * NDM
#################################
DM_OFFSET = 50.0
DM_SCALE  = 1.0
N  = np.zeros ((NPIX, NDM), dtype=np.uint32)
#################################
## work loop
for f in tDBSONs:
    with tdb.DBSON (os.path.join (DIR, f)) as dbf:
        RADEG = dbf.ra * DPR
        DEDEG = dbf.dec * DPR
        ipix  = hp.ang2pix (NSIDE, RADEG, DEDEG, lonlat=True)
        ira, idec = hp.pix2ang (NSIDE, ipix, lonlat=True)
        idm   = int ( (dbf.dm - DM_OFFSET)/DM_SCALE )
        if idm < 0 or idm >= NDM:
            continue
        ## updates
        N[ipix, idm]  += 1
#################################
N.dump ("./trial.npy")
