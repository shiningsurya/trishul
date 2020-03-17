"""
Deep learning based candidate dump vetter
"""
import os
import numpy as np
import matplotlib.pyplot as plt
import pickle as pkl
# sklearn imports
from sklearn.model_selection import StratifiedShuffleSplit
from sklearn.model_selection import train_test_split
import sklearn.metrics as skm
# torch imports
import torch
from torch.utils.data import Dataset
from torch.utils.data import DataLoader
from torch.utils.data import Sampler
from torch.utils.data import SubsetRandomSampler 
# trishul imports
from .dbson         import DBSON 

class BTElement (Dataset):
    """
    DBSON element dataset.

    Inspired from the pytorch tutorial
    """

    def __init__ (self, true, false, root_dir, transform=None):
        """
        Args:
            true_file (string, list): Path to true file list or list
            false_file (string, list): Path to false file list or list
            root_dir (string) : Path to the dbson file
            transform (callable, optional) : Optional transform to be applied on a sample
        """
        truelist = []
        if isinstance (true, str):
            with open (true, "r") as f:
                truelist = f.readlines ()
        elif isinstance (true, list):
            truelist = true
        else:
            raise ValueError ("Expected either list or str, but got=", type(true))
        ntrue = len(truelist)
        falselist = []
        if isinstance (false, str):
            with open (false, "r") as f:
                falselist = f.readlines ()
        elif isinstance (false, list):
            falselist = false
        else:
            raise ValueError ("Expected either list or str, but got=", type(false))
        nfalse = len(falselist)
        #
        self.filelist   = truelist + falselist
        self.target     = ([1]*ntrue) + ([0]*nfalse)
        self.n          = ntrue + nfalse
        self.root       = root_dir
        self.transform  = transform

    def __len__(self):
        return self.n

    def __getitem__ (self, idx):
        if torch.is_tensor (idx):
            idx = idx.tolist ()

        ret = dict ()
        ret['target'] = self.target[idx]
        fn,_ = os.path.splitext ( self.filelist[idx].strip() )
        with DBSON (os.path.join (self.root, "{0}.dbson".format(fn))) as dbs:
            ret['bt'] = np.expand_dims (np.array(dbs.bt, dtype=np.float32), axis=0)

        if self.transform:
            ret['bt'] = self.transform (ret['bt'])
        return ret

    def train_test_split (self, test_size=0.3, shuffle=True, stratify=True):
        """returns indices to split train/test"""
        d_i  = np.arange (self.n)
        if stratify:
            train_i, test_i = train_test_split (d_i, test_size=0.3, shuffle=shuffle, stratify=self.target)
        else:
            train_i, test_i = train_test_split (d_i, test_size=0.3, shuffle=shuffle)
        train_s         = SubsetRandomSampler (train_i)
        test_s          = SubsetRandomSampler (test_i)
        return train_s, test_s

class StratifiedSampler (Sampler):
    """StratifiedSampler for BTElement"""
    
    def __init__ (self, nt, nf, ns=5, ts=0.4):
        """
        Args:
            nt (int) : # of true
            nf (int) : # of false
            ns (int) : # of splits
            ts (float) : test_size
        """
        self.ntrue  = nt
        self.nfalse = nf
        self.nall   = self.ntrue + self.nfalse
        self.labels = np.concatenate ((np.ones(self.ntrue, dtype=np.uint8), np.zeros(self.nfalse, dtype=np.uint8)))
        self.nsplit = ns
        self.sss    = StratifiedShuffleSplit (self.nsplit, test_size=ts)
        self.dumbX  = np.zeros((self.nall,1),dtype=np.uint8)

    def iteration (self):
        train,test = next( self.sss.split (self.dumbX, self.labels))
        return np.hstack([train,test])

    def __iter__ (self):
        return iter (self.iteration())

    def __len__ (self):
        return self.nall

class RunningMean (object):
    """Running mean for losses."""
    def __init__ (self):
        self.n   = 0
        self.sum = 0.0

    def __call__ (self, v = None):
        """
        If called with argument, updates
        If called with no argument, returns mean
        """
        if v is None:
            return self.sum / self.n
        else:
            self.n   = self.n + 1
            self.sum = self.sum + v

    def reset (self):
        self.n    = 0
        self.sum  = 0.0

    def __str__ (self):
        return "iterations={0} mean={1}".format(self.n, self.sum/self.n)

class TopK (object):
    """Top K for metrics
    self.topK is numpy array of size k
    topK is ascending order
    """
    def __init__ (self, K=5):
        self.k     = K
        self.topk  = np.zeros (K, dtype=np.float32)
    def __call__ (self, v):
        """Updates topK with v"""
        idx = np.searchsorted (self.topk, v)
        if idx == 0 and self.topk[0] < v:
            self.topk[0] = v
        else:
            self.topk[idx] = v

def Recall_score (outputs, labels):
    """Recall for tensor inputs"""
    la   = labels.data.cpu().numpy ()
    ou   = outputs.data.cpu().numpy ().argmax (1)
    return skm.recall_score (la, ou)

def Precision_score (outputs, labels):
    """Precision for tensor inputs"""
    la   = labels.data.cpu().numpy ()
    ou   = outputs.data.cpu().numpy ().argmax (1)
    return skm.precision_score (la, ou)

class Curve (object):
    def __init__ (self, metrics, type_="learning"):
        self.t       = type_
        self.m       = list( metrics )
        self.metrics = {k:[] for k in metrics}
        self.metrics['loss'] = []

    def __call__ (self, rmet):
        '''What is returned by trainer/evaluator'''
        for k in self.m:
            self.metrics[k].append (rmet[k]())
        self.metrics['loss'].append (sum(rmet['loss'])/len(rmet['loss']))

    def __getitem__(self, idx):
        return self.metrics[idx]

    def __getstate__(self):
        mv = [self.metrics[k] for k in self.m]
        return [self.t, self.m, self.metrics['loss'], mv]

    def __setstate__ (self, t, m, ml, mv):
        self.t = t
        self.m = m
        self.metrics['loss'] = ml
        for k,v in zip(m, mv):
            self.metrics[k] = v

def PlotCurve (x, saveas=None):
    """x is Curve object or list"""
    fig, ax  = plt.subplots(2,1,sharex=True)
    if isinstance (x, Curve):
        x = [x]
    for ilx in x:
        ax[0].plot (ilx['loss'], label="{0}-loss".format(ilx.t))
        for k in ilx.m:
            ax[1].plot (ilx[k], label="{0}-{1}".format(ilx.t, k))
    # legend
    ax[0].legend()
    ax[1].legend()
    ax[1].set_xlabel ("Epoch")
    ax[0].set_ylabel ("Binary cross entropy loss")
    ax[1].set_ylabel ("Metrics")
    if saveas:
        plt.savefig (saveas)
    else:
        plt.show ()
    
def WritePickle (x, ofile):
    """Writes any python object"""
    with open(ofile, "wb") as f:
        pkl.dump (x, f)

def ReadPickle (ifile):
    """Read any pickle file"""
    with open (ofile, "rb") as f:
        return pkl.load (f)

def DumpModel (ofile, model, optimizer=None, best=False):
    """Writes the model to the file"""
    wr = dict ()
    wr['model_state'] = model.state_dict ()
    if optimizer:
        wr['optimizer_state'] = optimizer.state_dict()
    #
    ff = "{0}.pth".format(ofile)
    torch.save (wr, ff)
    if best:
        ff = "{0}_best.pth".format(ofile)
        torch.save (wr, ff)

