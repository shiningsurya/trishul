"""
Interface to models

Part of Trishul
"""
import os
import pickle as pkl

import numpy as np

from trishul import deepmodels as dm

import torch
from skimage.measure import block_reduce

ALL_MODELS = ['Battousai9']

class Battousai:
    def __init__ (self, verbose=False):
        self.name = "Battousai9"
        self.pickle_path = os.path.join(os.path.dirname(__file__),"../../Models/battousai9_cnn.pkl") 
        self.v = verbose
        #
        self.dm_width = 50
        self.ndm      = 256
        self.d_width  = self.dm_width / self.ndm
        #
        self.fch1     = 361.9414488190918
        self.foff     = -0.65525515625
        self.nchans   = 64
        self.f_width  = -self.foff * self.nchans
        #
        self.tsamp    = 0.00078125
        self.nsamps   = 256
        self.t_width  = self.nsamps * self.tsamp
        #
        self.clf      = dm.Battousai()
        self.load_weights ()
        self.clf.eval ()
        #
        self.clf_inshape   = (1, 2, 32, 32)
        self.d_res = self.d_width / 32
        self.t_res = self.t_width / 32
        self.f_res = -self.f_width / 32
        if self.v:
            print (f"CNN            = {self.name}")
            print (f"DM resolution  = {self.d_res}")
            print (f"FREQ resolution= {self.f_res}")
            print (f"TIME resolution= {self.t_res}")

        self.T_FAC  = 1
        self.F_FAC  = 1
        self.D_FAC  = 8
        #
        self.input  = torch.zeros (self.clf_inshape)

    def resolution (self, toff=None, foff=None, dmoff=None):
        if toff:
            self.T_FAC = int (self.t_res // toff)
        if foff:
            self.F_FAC = int (self.f_res // foff)
        if dmoff:
            self.D_FAC = int (self.d_res // dmoff)
        #
        if self.v:
            print (f"(DTF) = ({ self.D_FAC }, { self.T_FAC }, { self.F_FAC })")


    def load_weights (self,):
        with open(self.pickle_path, "rb") as f:
            self.clf.load_state_dict (pkl.load(f))

    def __call__ (self, bt, dd):
        abt = block_reduce (bt, (self.D_FAC, self.T_FAC), func=np.mean)
        add = block_reduce (dd, (self.F_FAC, self.T_FAC), func=np.mean)
        self.input[0,0,:,:32] = torch.from_numpy (abt[:,:32])
        self.input[0,1,:,:32] = torch.from_numpy (add[112:144,:32])
        """
        CNN has some resolutions it has learnt on.
        we change the input resolution accordingly and then crop if we get more than
        """
        ##
        ret = self.clf (self.input)
        idx = ret.argmax(1)

        return idx == 1

