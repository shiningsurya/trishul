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
# torch vision imports
from skimage.measure import block_reduce
import torchvision.transforms as tvt
# trishul imports
from .dbson         import DBSON 

NPYDICT = {
        'vall_btdd.npy':                   (818848,2,32,32),
        'whitenoise_btdd.npy':             (9717  ,2,32,32),
        'faketrue_btdd.npy':               (8480  ,2,32,32),
        'vall_psr_btdd.npy':               (23574 ,2,32,32),
        'vall_psr7_btdd.npy':              (19197,2,32,32),
        'vall_rfi_btdd.npy':               (8207  ,2,32,32),
        'vall_dm150_rfi_btdd.npy':         (20660 ,2,32,32),
}
"""
True.FAKETRUE     =    8480
True.PSR          =   23574
True              =   32054
True.PSR7         =   19197
---
False.RFI         =    8207
False.DM150       =   20660
False             =   28867
---
ALL               =  818848

ATTN:
    For all the classifier tasks, 
    a two element array is used for each target
    -- First element  is TRUE
    -- Second element is FALSE
"""

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
            abt   = np.array (dbs.bt, dtype=np.float32)
            #bbt   = np.flipud (np.fliplr (abt))
            #cbt   = np.dstack ([0.5*(abt+bbt),0.5*(abt-bbt)])
            #ret['bt'] = np.moveaxis (cbt, 2, 0)
            ret['bt'] = np.expand_dims (abt, axis=0)

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

class Element (Dataset):
    """
    DBSON element of dataset.

    Two channels.
    Shape = (2, 64, 64)
    First channel for Bowtie, second channel for de-dispersed filterbank
    """
    def __init__ (self, filelist, root_dir='', transform=None):
        """
        Args:
            filelist (string, list): Path to true file list or list
            root_dir (string) : Path to the dbson file
            transform (callable, optional) : Optional transform to be applied on a sample
        """
        allist = []
        if isinstance (filelist, str):
            with open (filelist, "r") as f:
                allist = f.readlines ()
        elif isinstance (filelist, list):
            allist = filelist 
        else:
            raise ValueError ("Expected either list or str, but got=", type(filelist))
        self.n = len(allist)
        #
        self.filelist   = allist
        self.root       = root_dir
        self.transform  = transform

    def __len__(self):
        return self.n

    def __getitem__ (self, idx):
        if torch.is_tensor (idx):
            idx = idx.tolist ()

        ret = dict ()
        fn,_ = os.path.splitext ( self.filelist[idx].strip() )
        with DBSON (os.path.join (self.root, "{0}.dbson".format(fn))) as dbs:
            bt    = np.array (dbs.bt, dtype=np.float32) / 255.
            dd    = np.array (dbs.dd, dtype=np.float32) / 255.
            ## because all the dbsons sizes are hardcoded
            tbt   = block_reduce (bt.reshape(1, 256, 256), (1,4,4), func=np.mean)
            tdd   = block_reduce (dd.reshape(1, 64, 256), (1,1,4), func=np.mean)
            #bbt   = np.flipud (np.fliplr (abt))
            #cbt   = np.dstack ([0.5*(abt+bbt),0.5*(abt-bbt)])
            #ret['bt'] = np.moveaxis (cbt, 2, 0)
            ret['payload'] = np.concatenate ((tbt, tdd), axis=0)

        if self.transform:
            ret['payload'] = self.transform (ret['payload'])
        return ret

    def train_test_split (self, test_size=0.3, shuffle=True,):
        """returns indices to split train/test"""
        d_i  = np.arange (self.n)
        train_i, test_i = train_test_split (d_i, test_size=0.3, shuffle=shuffle)
        train_s         = SubsetRandomSampler (train_i)
        test_s          = SubsetRandomSampler (test_i)
        return train_s, test_s

class NpyElement (object):
    """
    Encapsulates multiple numpy memmap files as a single memmap file
    """
    def __init__ (self, keys, root_dir="./", ):
        """
        Args:
            keys (list): Filenames which are to be used as keys in the 
                NPYDICT 
            root_dir (str): Where the actual memmap files are
        """
        mmdict = {'dtype':np.float32, 'mode':'r'}
        self.ns  = []      # container for elements
        self.fmaps = []    # container for memmap files
        # work
        for k in keys:
            self.fmaps.append (np.memmap (os.path.join (root_dir, k), shape=NPYDICT[k], **mmdict))
            self.ns.append (self.fmaps[-1].shape[0])
        # total len
        self.nlen = sum(self.ns)


    def __len__ (self, ):
        return self.nlen

    def __getitem__ (self, idx):
        """For for only supports single element access"""
        r = idx
        for i, ni in enumerate(self.ns):
            if r >= ni:
                r = r - ni
            else:
                return self.fmaps[i][r]
        return IndexError

class NpyClfDatasets (Dataset):
    """
    Element but interfaces with multiple numpy
    file using memory maps per each class

    Kind of assumed you are going to do with CAE
    """
    def __init__ (self, trues, falses, root_dir='./', transform=None):
        """
        Args:
            trues(list)  : List of btdd files.
            falses(list) : List of btdd files.
            transform (callable, optional): Optional transform to be applied
        """
        self.trues  = NpyElement (trues, root_dir=root_dir)
        self.falses = NpyElement (falses,root_dir=root_dir)
        self.ntrue  = self.trues.nlen
        self.nfalse = self.falses.nlen
        self.n      = self.ntrue + self.nfalse
        self.target = np.zeros (self.n, dtype=np.uint8)
        self.target[:self.ntrue] = 1
        self.transform = transform

    def __len__(self):
        return self.n

    def __getitem__ (self, idx):
        """I guess this doesn't support list indexing"""
        if torch.is_tensor (idx):
            idx = idx.tolist ()
        ret = dict ()
        if idx < self.ntrue:
            ret['payload'] = torch.Tensor (self.trues[idx])
            ret['target']  = torch.Tensor ([1]).to (torch.long)
        else:
            ridx = idx - self.ntrue
            ret['payload'] = torch.Tensor (self.falses[ridx])
            ret['target']  = torch.Tensor ([0]).to (torch.long)
        if self.transform:
            ret['payload'] = self.transform (ret['payload'])
        return ret

    def train_test_split (self, test_size=0.3, shuffle=True,random_state=None):
        """returns indices to split train/test"""
        d_i  = np.arange (self.n)
        train_i, test_i = train_test_split (d_i, test_size=test_size, shuffle=shuffle, stratify=self.target, random_state=random_state)
        train_s         = SubsetRandomSampler (train_i)
        test_s          = SubsetRandomSampler (test_i)
        return train_s, test_s

class NpyDatasets (Dataset):
    """
    Element but interfaces with multiple numpy
    file using memory maps 

    Kind of assumed you are going to do with CAE
    """
    def __init__ (self, filedict, root_dir='./', transform=None):
        """
        Args:
            filedict (dict) keys=filenames, values=number of triggers
                This is badly needed because numpy memory maps dont 
                carry any shape information
            root_dir (string) path to the dataset
            transform (callable, optional): Optional transform to be applied
        """
        self.fmap = dict()
        mmdict = {'dtype':np.float32, 'mode':'r'}
        self.n = 0
        ## open mmaps in strictly read mode
        for k, v in filedict.items():
            self.fmap[v[0]] = np.memmap (os.path.join(root_dir, k), shape=v, **mmdict)
            self.n += v[0]
        self.fns = list (filedict.keys())
        self.transform = transform


    def __len__(self):
        return self.n

    def __getitem__ (self, idx):
        """I guess this doesn't support list indexing"""
        if torch.is_tensor (idx):
            idx = idx.tolist ()
        ret = dict ()
        ridx = idx
        fidx = 0
        for k,v in self.fmap.items():
            if k <= ridx:
                ridx = ridx - k
                fidx = fidx + 1
                continue
            if k > ridx:
                ret['payload'] = torch.from_numpy( v[ridx] )
                ret['source']  = os.path.basename(v.filename)
                break

        if self.transform:
            ret['payload'] = self.transform (ret['payload'])
        return ret

    def train_test_split (self, test_size=0.3, shuffle=True,):
        """returns indices to split train/test"""
        d_i  = np.arange (self.n)
        train_i, test_i = train_test_split (d_i, test_size=0.3, shuffle=shuffle)
        train_s         = SubsetRandomSampler (train_i)
        test_s          = SubsetRandomSampler (test_i)
        return train_s, test_s

class NpySupervised(Dataset):
    """
    Element but interfaces with multiple numpy
    file using memory maps 

    Kind of assumed you are going to do with CAE
    """
    def __init__ (self, filedict, root_dir='./', transform=None):
        """
        Args:
            filedict (dict) keys=filenames, values=number of triggers
                This is badly needed because numpy memory maps dont 
                carry any shape information
            root_dir (string) path to the dataset
            transform (callable, optional): Optional transform to be applied
        """
        self.fmap = dict()
        mmdict = {'dtype':np.float32, 'mode':'r'}
        self.n = 0
        ## open mmaps in strictly read mode
        for k, v in filedict.items():
            self.fmap[v[0]] = np.memmap (os.path.join(root_dir, k), shape=v, **mmdict)
            self.n += v[0]
        self.fns = list (filedict.keys())
        self.transform = transform


    def __len__(self):
        return self.n

    def __getitem__ (self, idx):
        """I guess this doesn't support list indexing"""
        if torch.is_tensor (idx):
            idx = idx.tolist ()
        ret = dict ()
        ridx = idx
        fidx = 0
        for k,v in self.fmap.items():
            if k <= ridx:
                ridx = ridx - k
                fidx = fidx + 1
                continue
            if k > ridx:
                ret['payload'] = torch.from_numpy( v[ridx] )
                ret['source']  = os.path.basename(v.filename)
                break

        if self.transform:
            ret['payload'] = self.transform (ret['payload'])
        return ret

    def train_test_split (self, test_size=0.3, shuffle=True,):
        """returns indices to split train/test"""
        d_i  = np.arange (self.n)
        train_i, test_i = train_test_split (d_i, test_size=0.3, shuffle=shuffle)
        train_s         = SubsetRandomSampler (train_i)
        test_s          = SubsetRandomSampler (test_i)
        return train_s, test_s

class NpyDataset (Dataset):
    """
    Element but interfaces with a numpy
    file using memory maps 

    Kind of assumed you are going to do with CAE
    Kind of also assumes you are going to do only one npy file
    """
    def __init__ (self, mfile, mshape, root_dir='./',transform=None):
        """
        Args:
            mfile (str): path of npy file
            mshape (tuple) : (_, 2, 32, 32)
            transform (callable, optional): Optional transform to be applied
        """
        self.fmap = np.memmap (
                os.path.join (root_dir,mfile),
                dtype=np.float32, mode='r', shape=mshape)
        self.n = mshape[0]
        self.transform = transform

    def __len__(self):
        return self.n

    def __getitem__ (self, idx):
        """supports list indexing"""
        if torch.is_tensor (idx):
            idx = idx.tolist ()
        ret = dict ()
        ret['payload'] = np.array (self.fmap[idx])

        if self.transform:
            ret['payload'] = self.transform (ret['payload'])
        return ret

    def train_test_split (self, test_size=0.3, shuffle=True,random_state=None):
        """returns indices to split train/test"""
        d_i  = np.arange (self.n, dtype=np.uint32)
        train_i, test_i = train_test_split (d_i, test_size=0.3, shuffle=shuffle, random_state=random_state)
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
    def __init__ (self, metrics, epoch = 0, type_="learning"):
        self.t       = type_
        self.m       = list( metrics )
        self.metrics = {k:[] for k in metrics}
        self.metrics['loss'] = []
        self.e       = epoch

    def __call__ (self, rmet):
        '''What is returned by trainer/evaluator'''
        for k in self.m:
            self.metrics[k].append (rmet[k]())
        self.metrics['loss'].append (sum(rmet['loss'])/len(rmet['loss']))

    def __getitem__(self, idx):
        return self.metrics[idx]

    def __getstate__(self):
        mv = [self.metrics[k] for k in self.m]
        return [self.e, self.t, self.m, self.metrics['loss'], mv]

    def __setstate__ (self, state):
        self.e = state[0]
        self.t = state[1]
        self.m = state[2]
        self.metrics = dict ()
        self.metrics['loss'] = state[3]
        for k,v in zip(state[2], state[4]):
            self.metrics[k] = v

    def append (self, o):
        """Adds another curve object to self
            All checks are done prior to calling 
        """
        self.metrics['loss'] += o.metrics['loss']
        for k in self.m:
            self.metrics[k]  += o.metrics[k]

    def __lt__ (self, o):
        """Less than o"""
        return self.e < o.e
    def __eq__ (self, o):
        """Equal"""
        return self.e == o.e

    def __repr__ (self):
        """String"""
        return "Trishul.DeepUtils.Curve Epoch:{0} of type:{1} with len:{2}".format(self.e, self.t, len(self.metrics['loss']))

###
COLOR_DICT                            = dict ()
COLOR_DICT['training']                = dict ()
COLOR_DICT['training']['loss']        = "r"
COLOR_DICT['training']['recall']      = "g"
COLOR_DICT['training']['precision']   = "b"
COLOR_DICT['validation']              = dict ()
COLOR_DICT['validation']['loss']      = "k"
COLOR_DICT['validation']['recall']    = "y"
COLOR_DICT['validation']['precision'] = "m"
###
def MergeCurves (*args):
    """Merge curves from different epoch runs"""
    args = sorted (args)
    ts = set ( [a.t for a in args] )
    ms = []
    for a in args:
        ms += a.m
    ms = list(set(ms))
    # for all such training/validation
    state = {k:Curve(ms,type_=k) for k in ts}
    for a in args:
        state[a.t].append (a)
    return list (state.values())

def PlotCurve (x, saveas=None):
    """x is Curve object or list"""
    fig, ax  = plt.subplots(2,1,sharex=True)
    if isinstance (x, Curve):
        x = [x]
    for ilx in x:
        epc = np.arange(len(ilx['loss'])) + ilx.e
        ax[0].step (epc, ilx['loss'], label="{0}-loss".format(ilx.t))
        for k in ilx.m:
            ax[1].step (epc, ilx[k], label="{0}-{1}".format(ilx.t, k))
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
    with open (ifile, "rb") as f:
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

def LoadModel (ifile, model, optimizer=None, cpu=True):
    """Loads the model"""
    location = None
    if cpu:
        location = torch.device('cpu')
    ret = torch.load (ifile, map_location=location)
    model.load_state_dict (ret['model_state'])
    if optimizer:
        optimizer.load_state_dict (ret['optimizer_state'])
    return ret
