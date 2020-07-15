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

class VGG11A (nn.Module):
    def __init__ (self, idx=300, chanin=1, num_classes=2):
        super (VGG11A, self).__init__ ()
        # 
        self.name  = "VGG11A"
        self.idx   = idx
        self.features = nn.Sequential (
            nn.Conv2d (chanin, 64, kernel_size=3, padding=1),
            nn.ReLU (inplace=True),
            nn.MaxPool2d (kernel_size=2, stride=2),
            nn.Conv2d (64, 128, kernel_size=3, padding=1),
            nn.ReLU (inplace=True),
            nn.MaxPool2d (kernel_size=2, stride=2),
            nn.Conv2d (128, 256, kernel_size=3, padding=1),
            nn.ReLU (inplace=True),
            nn.Conv2d (256, 256, kernel_size=3, padding=1),
            nn.ReLU (inplace=True),
            nn.MaxPool2d (kernel_size=2, stride=2),
            nn.Conv2d (256, 512, kernel_size=3, padding=1),
            nn.ReLU (inplace=True),
            nn.Conv2d (512, 512, kernel_size=3, padding=1),
            nn.ReLU (inplace=True),
            nn.MaxPool2d (kernel_size=2, stride=2),
            nn.Conv2d (512, 512, kernel_size=3, padding=1),
            nn.ReLU (inplace=True),
            nn.Conv2d (512, 512, kernel_size=3, padding=1),
            nn.ReLU (inplace=True),
            nn.MaxPool2d (kernel_size=2, stride=2)
        )
        self.avgpool  = nn.AdaptiveAvgPool2d ((7,7))
        self.classifier = nn.Sequential (
            nn.Linear (512 * 7 * 7, 4096),
            nn.ReLU (True),
            nn.Dropout (), 
            nn.Linear (4096, 4096),
            nn.ReLU (True),
            nn.Dropout(),
            nn.Linear (4096, num_classes)
        )
    def forward (self, x):
        x = self.features (x)
        x = self.avgpool (x)
        x = t.flatten (x, 1)
        x = self.classifier (x)
        return x

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
        self.c1 = nn.Conv2d (2, 10, kernel_size=3)
        # no padding because our thing is right in the dead center
        self.c2 = nn.Conv2d (10, 10, kernel_size=3)
        #
        self.b1 = nn.BatchNorm2d (10)
        #
        self.c3 = nn.Conv2d (10, 5, kernel_size=3, stride=2)
        #
        self.m1 = nn.MaxPool2d (5)
        #
        self.c4 = nn.Conv2d (5, 1, kernel_size=5, stride=3, dilation=2)
        # 169 FC
        self.fc1 = nn.Linear (36, 8)
        self.fc2 = nn.Linear (8, 2)

    def forward (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = self.b1 (x)
        x = F.relu (self.c3 (x))
        x = self.m1 (x)
        x = F.relu (self.c4 (x))
        x = x.view (-1, 36)
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

class GomuGomu (nn.Module):
    """
    GomuGomu Convolutional Autoencoder

    Input 
    (2, 64, 64) 
    -> (4, 16, 16) 
    -> (8, 4,  4)
    -> FC latent space <-
    -> (4, 16, 16)
    (2, 64, 64)
    """
    def __init__ (self, latent_dim=16, idx=100):
        super (GomuGomu, self).__init__ ()
        self.name = "GomuGomu"
        self.idx  = idx
        # encode 
        self.c1  = nn.Conv2d (2, 6, kernel_size=3)
        self.c2  = nn.Conv2d (6, 8, kernel_size=5, stride=3)
        self.bn1 = nn.BatchNorm2d (8)
        self.c3  = nn.Conv2d (8, 10, kernel_size=5, stride=5)
        ## reshape
        self.before_fc1 = [10,4,4]
        # lowest dim
        self.fc1 = nn.Linear (160, latent_dim)
        self.fc2 = nn.Linear (latent_dim, 160)
        # decode
        self.d3  = nn.ConvTranspose2d (10, 8, kernel_size=5, stride=5, output_padding=0)
        self.bn2 = nn.BatchNorm2d (8)
        self.d2  = nn.ConvTranspose2d (8, 6, kernel_size=5, stride=3,output_padding=0)
        self.d1  = nn.ConvTranspose2d (6, 2, kernel_size=3)
        ##

    def encode (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = self.bn1 (x)
        x = F.relu (self.c3 (x))
        x = x.view([x.size(0), -1])
        x = F.relu (self.fc1 (x))
        # 160
        return x
    
    def decode (self, x):
        x = F.relu (self.fc2(x))
        x = x.view ([x.size(0), 10, 4, 4])
        x = F.relu (self.d3 (x))
        x = self.bn2 (x)
        x = F.relu (self.d2 (x))
        x = F.relu (self.d1 (x))
        x = t.sigmoid (x)
        return x

    def forward (self, x):
        c = self.encode (x)
        y = self.decode (c)
        return y,c

