"""
Deep learning models

Named after doujistus of Naruto series

All CNNs work on Bowtie plane (256x256) and yield a probability.
"""
# torch imports
import torch    as t
import torch.nn as nn
import torch.nn.functional as F

MODELS = [
        "Byakugan", "b", "B",
        "Sharingan", "s", "S",
        "Rinnegan", "r", "R"
    ]

class Byakugan (nn.Module):
    """
    Byakugan class CNN

    Input
    (256,256) --> (256,256,2) --> (64,64) --> (16,16,2) --> (4,4) --> FC
    """
    def __init__ (self, idx=100):
        super (Byakugan, self).__init__ ()
        self.name  = "Byakugan"
        self.idx   = idx
        #
        self.c1 = nn.Conv2d (1, 5, kernel_size=5)
        # no padding because our thing is right in the dead center
        self.c2 = nn.Conv2d (5, 5, kernel_size=3)
        #
        self.b1 = nn.BatchNorm2d (5)
        #
        self.c3 = nn.Conv2d (5, 2, kernel_size=3, stride=2)
        #
        self.m1 = nn.MaxPool2d (2)
        #
        self.c4 = nn.Conv2d (2, 1, kernel_size=3, stride=3, dilation=2)
        # 169 FC
        self.fc1 = nn.Linear (400, 16)
        self.fc2 = nn.Linear (16, 2)

    def forward (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = self.b1 (x)
        x = F.relu (self.c3 (x))
        x = self.m1 (x)
        x = F.relu (self.c4 (x))
        x = x.view (-1, 400)
        x = F.relu (self.fc1 (x))
        x = F.relu (self.fc2 (x))
        # x = t.softmax (x)
        return x

class Sharingan (nn.Module):
    """
    Byakugan class CNN

    Input
    (256,256) --> (256,256,2) --> (64,64) --> (16,16,2) --> (4,4) --> FC
    """
    def __init__ (self, idx=100):
        super (Sharingan, self).__init__ ()
        self.name  = "Sharingan"
        self.idx   = idx
        #
        self.c1 = nn.Conv2d (1, 5, kernel_size=5)
        # no padding because our thing is right in the dead center
        self.c2 = nn.Conv2d (5, 5, kernel_size=3)
        #
        self.b1 = nn.BatchNorm2d (5)
        #
        self.c3 = nn.Conv2d (5, 2, kernel_size=3, stride=2)
        #
        self.m1 = nn.MaxPool2d (2)
        #
        self.c4 = nn.Conv2d (2, 1, kernel_size=3, stride=3, dilation=2)
        # 169 FC
        self.fc1 = nn.Linear (400, 16)
        self.fc2 = nn.Linear (16, 2)

    def forward (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = self.b1 (x)
        x = F.relu (self.c3 (x))
        x = self.m1 (x)
        x = F.relu (self.c4 (x))
        x = x.view (-1, 400)
        x = F.relu (self.fc1 (x))
        x = F.relu (self.fc2 (x))
        # x = t.sigmoid (x)
        return x
