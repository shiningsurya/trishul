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
    def __init__ (self, idx=100, latent_dim=8, ):
        super (GomuGomu, self).__init__ ()
        self.name = "GomuGomu"
        self.idx  = idx
        # encode 
        self.c1  = nn.Conv2d (2, 6, kernel_size=3)
        self.c2  = nn.Conv2d (6, 8, kernel_size=3,)
        self.bn1 = nn.MaxPool2d (3,)
        self.c3  = nn.Conv2d (8, 10, kernel_size=3,)
        ## reshape
        self.before_fc = [10,7,7]
        # lowest dim
        self.fc1 = nn.Linear (490, 32)
        self.fc2 = nn.Linear (32, latent_dim)
        self.cf2 = nn.Linear (latent_dim, 64)
        self.cf1 = nn.Linear (64, 507)
        # decode
        self.d3  = nn.ConvTranspose2d (3, 6, kernel_size=3, stride=2, output_padding=1,)
        self.d2  = nn.ConvTranspose2d (6, 4, kernel_size=3, stride=1, output_padding=0)
        self.d1  = nn.ConvTranspose2d (4, 2, kernel_size=3, stride=1)
        ##

    def encode (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = self.bn1 (x)
        x = F.relu (self.c3 (x))
        x = x.view([x.size(0), -1])
        x = F.relu (self.fc1 (x))
        x = F.relu (self.fc2 (x))
        # 160
        return x
    
    def decode (self, x):
        x = F.relu (self.cf2(x))
        x = F.relu (self.cf1(x))
        x = x.view ([-1, 3, 13, 13])
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.d1 (x)
        x = t.sigmoid (x)
        return x

    def forward (self, x):
        c = self.encode (x)
        y = self.decode (c)
        return y,c

    def decode_conv (self, x):
        print (x.shape)
        x = F.relu (self.d3 (x))
        print (x.shape)
        x = F.relu (self.d2 (x))
        print (x.shape)
        x = self.d1 (x)
        return x 

class BaraBara (nn.Module):
    """
    A bit higher depth than GomuGomu

    Input 
    (2, 32, 32) 
    -> (4, 16, 16) 
    -> (8, 4,  4)
    -> FC latent space <-
    -> (4, 16, 16)
    (2, 32, 32)
    """
    def __init__ (self, latent_dim=8, idx=100):
        super (BaraBara, self).__init__ ()
        self.name = "BaraBara"
        self.idx  = idx
        # encode 
        self.c1  = nn.Conv2d (2, 6, kernel_size=3)
        self.c2  = nn.Conv2d (6, 8, kernel_size=3,)
        self.bn1 = nn.BatchNorm2d (8)
        self.c3  = nn.Conv2d (8, latent_dim, kernel_size=5, stride=1)
        ## reshape
        self.before_fc1 = [8,24,24]
        # lowest dim
        self.fc1 = nn.Linear (4608, 512)
        self.fc2 = nn.Linear (512, latent_dim)
        self.cf1 = nn.Linear (latent_dim, 512)
        self.cf2 = nn.Linear (512, 4608)
        # decode
        self.d3  = nn.ConvTranspose2d (latent_dim, 8, kernel_size=3, stride=1, output_padding=0)
        self.d2  = nn.ConvTranspose2d (8, 6, kernel_size=3, output_padding=0)
        self.bn2 = nn.BatchNorm2d (6)
        self.d1  = nn.ConvTranspose2d (6, 2, kernel_size=5)
        ##

    def encode (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = self.bn1 (x)
        x = F.relu (self.c3 (x))
        x = x.view([x.size(0), -1])
        x = F.relu (self.fc1 (x))
        x = t.sigmoid (self.fc2 (x))
        # 160
        return x
    
    def decode (self, x):
        x = F.relu (self.cf1(x))
        x = F.relu (self.cf2(x))
        x = x.view ([-1, 8, 24, 24])
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.bn2 (x)
        x = self.d1 (x)
        x = t.sigmoid (x)
        return x

    def decode_conv(self, x):
        print ('input.shape=', x.shape)
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.bn2 (x)
        x = self.d1 (x)
        x = t.sigmoid (x)
        print ('output.shape=', x.shape)
        return x

    def forward (self, x):
        c = self.encode (x)
        y = self.decode (c)
        return y,c

class YomiYomi(nn.Module):
    """
    YomiYomi Convolutional Autoencoder

    Input 
    (2, 32, 32) 
    -> (16, 16, 16) 
    -> (32, 8,  8)
    -> FC latent space <-
    -> (, 16, 16)
    (2, 32, 32)
    """
    def __init__ (self, idx=100, latent_dim=8, ):
        super (YomiYomi, self).__init__ ()
        self.name = "YomiYomi"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.c1  = nn.Conv2d (2, 8, kernel_size=3, stride=2)
        self.c2  = nn.Conv2d (8, 16, kernel_size=3, stride=2)
        self.c3  = nn.Conv2d (16, 32, kernel_size=3, stride=2)
        ## reshape
        self.before_fc = [32,3,3]
        # lowest dim
        self.fc1 = nn.Linear (288, 32)
        self.fc2 = nn.Linear (32, latent_dim)
        self.cf2 = nn.Linear (latent_dim, 32)
        self.cf1 = nn.Linear (32, 288)
        # decode
        self.d3  = nn.ConvTranspose2d (32, 16, kernel_size=3, stride=2, output_padding=0,)
        self.d2  = nn.ConvTranspose2d (16, 8, kernel_size=3, stride=2, output_padding=0)
        self.d1  = nn.ConvTranspose2d (8, 2, kernel_size=3, stride=2)
    def encode (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = F.relu (self.c3 (x))
        x = x.view([x.size(0), -1])
        x = F.relu (self.fc1 (x))
        x = F.relu (self.fc2 (x))
        return x
    
    def decode (self, x):
        x = F.relu (self.cf2(x))
        x = F.relu (self.cf1(x))
        x = x.view ([-1, 32, 3, 3])
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.d1 (x, output_size=(x.size(0), 2, 32, 32))
        x = t.sigmoid (x)
        return x

    def forward (self, x):
        c = self.encode (x)
        y = self.decode (c)
        return y,c

    def decode_conv (self, x):
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.d1 (x, output_size=(x.size(0), 2, 32, 32))
        return x 
    
class GareGare(nn.Module):
    """
    GareGare Convolutional Autoencoder

    Input 
    (2, 32, 32) 
    -> (16, 16, 16) 
    -> (32, 8,  8)
    -> FC latent space <-
    -> (, 16, 16)
    (2, 32, 32)
    """
    def __init__ (self, idx=100, latent_dim=32, ):
        super (GareGare, self).__init__ ()
        self.name = "GareGare"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.c1  = nn.Conv2d (2, 64, kernel_size=2, stride=2)
        self.c2  = nn.Conv2d (64, 96, kernel_size=2, stride=2)
        self.c3  = nn.Conv2d (96, 128, kernel_size=2, stride=2)
        ## reshape
        self.before_fc = [128,4,4]
        # lowest dim
        self.fc1 = nn.Linear (2048, 512)
        self.fc2 = nn.Linear (512, latent_dim)
        self.cf2 = nn.Linear (latent_dim, 512)
        self.cf1 = nn.Linear (512, 2048)
        # decode
        self.d3  = nn.ConvTranspose2d (128, 96, kernel_size=2, stride=2, output_padding=0,)
        self.d2  = nn.ConvTranspose2d (96, 64, kernel_size=2, stride=2, output_padding=0)
        self.d1  = nn.ConvTranspose2d (64, 2, kernel_size=2, stride=2)
        ##
    def encode (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = F.relu (self.c3 (x))
        x = x.view([x.size(0), -1])
        x = F.relu (self.fc1 (x))
        x = t.sigmoid (self.fc2 (x))
        return x

    def decode (self, x):
        x = F.relu (self.cf2(x))
        x = F.relu (self.cf1(x))
        x = x.view ([-1, 128, 4, 4])
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.d1 (x)
        x = t.sigmoid (x)
        return x

    def forward (self, x):
        c = self.encode (x)
        y = self.decode (c)
        return y,c

    def decode_conv (self, x):
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.d1 (x, output_size=(x.size(0), 2, 32, 32))
        return x 

class SoruSoru(nn.Module):
    """
    SoruSoru Convolutional Autoencoder

    Input 
    (2, 32, 32) 
    -> (16, 16, 16) 
    -> (32, 8,  8)
    -> FC latent space <-
    -> (, 16, 16)
    (2, 32, 32)
    """
    def __init__ (self, idx=100, latent_dim=8, ):
        super (SoruSoru, self).__init__ ()
        self.name = "SoruSoru"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.c1  = nn.Conv2d (2, 16, kernel_size=2, stride=2)
        self.c2  = nn.Conv2d (16, 32, kernel_size=2, stride=2)
        self.c3  = nn.Conv2d (32, 64, kernel_size=2, stride=2)
        ## reshape
        self.before_fc = [64,4,4]
        # lowest dim
        self.fc1 = nn.Linear (1024, 64)
        self.fc2 = nn.Linear (64, latent_dim)
        self.cf2 = nn.Linear (latent_dim, 64)
        self.cf1 = nn.Linear (64, 1024)
        # decode
        self.d3  = nn.ConvTranspose2d (64, 32, kernel_size=2, stride=2, output_padding=0,)
        self.d2  = nn.ConvTranspose2d (32, 16, kernel_size=2, stride=2, output_padding=0)
        self.d1  = nn.ConvTranspose2d (16, 2, kernel_size=2, stride=2)
        ##
    def encode (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = F.relu (self.c3 (x))
        x = x.view([x.size(0), -1])
        x = F.relu (self.fc1 (x))
        x = t.sigmoid (self.fc2 (x))
        return x

    def decode (self, x):
        x = F.relu (self.cf2(x))
        x = F.relu (self.cf1(x))
        x = x.view ([-1, 64, 4, 4])
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.d1 (x)
        x = t.sigmoid (x)
        return x

    def forward (self, x):
        c = self.encode (x)
        y = self.decode (c)
        return y,c

    def decode_conv (self, x):
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.d1 (x, output_size=(x.size(0), 2, 32, 32))
        return x 

class Please2(nn.Module):
    """Ryujin Jakka """
    def __init__ (self, idx=100, latent_dim=256, num_classes=2):
        super (Please2, self).__init__ ()
        self.name = "Please2"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.c1  = nn.Conv2d (2, 16,    kernel_size=2, stride=2)
        self.c2  = nn.Conv2d (16, 32,   kernel_size=2, stride=2)
        self.bn1 = nn.BatchNorm2d (32)
        self.c3  = nn.Conv2d (32, 64,   kernel_size=2, stride=2)
        self.c4  = nn.Conv2d (64, 128,  kernel_size=2, stride=2)
        self.bn2 = nn.BatchNorm2d (128)
        self.c5  = nn.Conv2d (128, 256, kernel_size=2, stride=2)
        ## reshape
        self.before_fc = [256,1,1]
        ##
        self.d5  = nn.ConvTranspose2d (256, 128, kernel_size=2, stride=2, output_padding=0,)
        self.d4  = nn.ConvTranspose2d (128, 64, kernel_size=2, stride=2, output_padding=0)
        self.nb2 = nn.BatchNorm2d (64)
        self.d3  = nn.ConvTranspose2d (64, 32, kernel_size=2, stride=2, output_padding=0,)
        self.d2  = nn.ConvTranspose2d (32, 16, kernel_size=2, stride=2, output_padding=0)
        self.nb1 = nn.BatchNorm2d (16)
        self.d1  = nn.ConvTranspose2d (16, 2, kernel_size=2, stride=2)

    def encode (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = self.bn1 (x)
        x = F.relu (self.c3 (x))
        x = F.relu (self.c4 (x))
        x = self.bn2 (x)
        x = t.sigmoid (self.c5 (x))
        return x

    def decode (self, x):
        x = F.relu (self.d5 (x))
        x = F.relu (self.d4 (x))
        x = self.nb2 (x)
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.nb1 (x)
        x = self.d1 (x)
        x = t.sigmoid (x)
        return x

    def forward (self, x):
        c = self.encode (x)
        y = self.decode (c)
        c = c.view ([c.size(0), -1])
        return y,c

class Please(nn.Module):
    """Ryujin Jakka """
    def __init__ (self, idx=100, latent_dim=256, num_classes=2):
        super (Please, self).__init__ ()
        self.name = "Please"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.c1  = nn.Conv2d (2, 16,    kernel_size=2, stride=2)
        self.c2  = nn.Conv2d (16, 32,   kernel_size=2, stride=2)
        self.c3  = nn.Conv2d (32, 64,   kernel_size=2, stride=2)
        self.c4  = nn.Conv2d (64, 128,  kernel_size=2, stride=2)
        self.c5  = nn.Conv2d (128, 256, kernel_size=2, stride=2)
        ## reshape
        self.before_fc = [256,1,1]
        ##
        self.d5  = nn.ConvTranspose2d (256, 128, kernel_size=2, stride=2, output_padding=0,)
        self.d4  = nn.ConvTranspose2d (128, 64, kernel_size=2, stride=2, output_padding=0)
        self.d3  = nn.ConvTranspose2d (64, 32, kernel_size=2, stride=2, output_padding=0,)
        self.d2  = nn.ConvTranspose2d (32, 16, kernel_size=2, stride=2, output_padding=0)
        self.d1  = nn.ConvTranspose2d (16, 2, kernel_size=2, stride=2)

    def encode (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = F.relu (self.c3 (x))
        x = F.relu (self.c4 (x))
        x = t.sigmoid (self.c5 (x))
        return x

    def decode (self, x):
        x = F.relu (self.d5 (x))
        x = F.relu (self.d4 (x))
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.d1 (x)
        x = t.sigmoid (x)
        return x

    def forward (self, x):
        c = self.encode (x)
        y = self.decode (c)
        return y,c

class Ryujin (nn.Module):
    """Ryujin Jakka """
    def __init__ (self, idx=100, latent_dim=16, num_classes=2):
        super (Ryujin, self).__init__ ()
        self.name = "Ryujin"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.c1  = nn.Conv2d (2, 16,    kernel_size=2, stride=2)
        self.c2  = nn.Conv2d (16, 32,   kernel_size=2, stride=2)
        self.c3  = nn.Conv2d (32, 64,   kernel_size=2, stride=2)
        self.c4  = nn.Conv2d (64, 128,  kernel_size=2, stride=2)
        self.c5  = nn.Conv2d (128, 256, kernel_size=2, stride=2)
        ## reshape
        self.before_fc = [256,1,1]
        # lowest dim
        self.fc1 = nn.Linear (256, 64)
        self.fc2 = nn.Linear (64, latent_dim)
        ## reverse
        self.cf2 = nn.Linear (latent_dim, 64)
        self.cf1 = nn.Linear (64, 256)
        ##
        self.d5  = nn.ConvTranspose2d (256, 128, kernel_size=2, stride=2, output_padding=0,)
        self.d4  = nn.ConvTranspose2d (128, 64, kernel_size=2, stride=2, output_padding=0)
        self.d3  = nn.ConvTranspose2d (64, 32, kernel_size=2, stride=2, output_padding=0,)
        self.d2  = nn.ConvTranspose2d (32, 16, kernel_size=2, stride=2, output_padding=0)
        self.d1  = nn.ConvTranspose2d (16, 2, kernel_size=2, stride=2)
    def encode (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = F.relu (self.c3 (x))
        x = F.relu (self.c4 (x))
        x = F.relu (self.c5 (x))
        x = x.view([x.size(0), -1])
        x = F.relu (self.fc1 (x))
        x = t.sigmoid (self.fc2 (x))
        return x

    def decode (self, x):
        x = F.relu (self.cf2(x))
        x = F.relu (self.cf1(x))
        x = x.view ([-1, 256, 1, 1])
        x = F.relu (self.d5 (x))
        x = F.relu (self.d4 (x))
        x = F.relu (self.d3 (x))
        x = F.relu (self.d2 (x))
        x = self.d1 (x)
        x = t.sigmoid (x)
        return x

    def forward (self, x):
        c = self.encode (x)
        y = self.decode (c)
        return y,c

class YareYare(nn.Module):
    """
    YareYare Convolutional Autoencoder

    Input 
    (2, 32, 32) 
    -> (8, 16, 16) 
    -> (16, 8,  8)
    -> FC latent space <-
    -> (16, 8, 8)
    (2, 32, 32)
    """
    def __init__ (self, idx=100, latent_dim=8, ):
        super (YareYare, self).__init__ ()
        self.name = "YareYare"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.c1  = nn.Conv2d (2, 8, kernel_size=2, stride=2)
        self.c2  = nn.Conv2d (8, 16, kernel_size=2, stride=2)
        ## reshape
        self.before_fc = [16,8,8]
        # lowest dim
        self.fc1 = nn.Linear (1024, latent_dim)
        self.cf1 = nn.Linear (latent_dim, 1024)
        # decode
        self.d2  = nn.ConvTranspose2d (16, 8, kernel_size=2, stride=2, output_padding=0,)
        self.d1  = nn.ConvTranspose2d (8, 2, kernel_size=2, stride=2, output_padding=0)
        ##

    def encode (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = x.view([x.size(0), -1])
        x = t.sigmoid (self.fc1 (x))
        return x

    def decode (self, x):
        x = F.relu (self.cf1(x))
        x = x.view ([-1, 16, 8, 8])
        x = F.relu (self.d2 (x))
        x = self.d1 (x)
        x = t.sigmoid (x)
        return x

    def forward (self, x):
        c = self.encode (x)
        y = self.decode (c)
        return y,c

class OiOi(nn.Module):
    """
    OiOi Convolutional Autoencoder

    Input 
    (2, 32, 32) 
    -> (4, 16, 16) 
    -> (6, 8,  8)
    -> FC latent space <-
    -> (6, 8, 8)
    -> (4, 16,16)
    (2, 32, 32)
    """
    def __init__ (self, idx=100, latent_dim=8, ):
        super (OiOi, self).__init__ ()
        self.name = "OiOi"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.c1  = nn.Conv2d (2, 4, kernel_size=2, stride=2)
        self.c2  = nn.Conv2d (4, 6, kernel_size=2, stride=2)
        ## reshape
        self.before_fc = [6,8,8]
        # lowest dim
        self.fc1 = nn.Linear (384, latent_dim)
        self.cf1 = nn.Linear (latent_dim, 384)
        # decode
        self.d2  = nn.ConvTranspose2d (6, 4, kernel_size=2, stride=2, output_padding=0,)
        self.d1  = nn.ConvTranspose2d (4, 2, kernel_size=2, stride=2, output_padding=0)
        ##

    def encode (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = x.view([x.size(0), -1])
        x = t.sigmoid (self.fc1 (x))
        return x

    def decode (self, x):
        x = F.relu (self.cf1(x))
        x = x.view ([-1, 6, 8, 8])
        x = F.relu (self.d2 (x))
        x = self.d1 (x)
        x = t.sigmoid (x)
        return x

    def forward (self, x):
        c = self.encode (x)
        y = self.decode (c)
        return y,c

class SenbonSakura (nn.Module):
    """
    The classifier taking code
    """
    def __init__ (self, idx=60, nin=8, num_classes=2):
        super (SenbonSakura, self).__init__()
        self.name = "SenbonSakura"
        self.idx  = idx
        self.clf = nn.Sequential (
            nn.Linear (nin, 4),
            nn.ReLU (True),
            nn.Linear (4, 2),
            nn.Sigmoid()
        )

    def forward (self, x):
        x = self.clf (x)
        return x
    
class RegaRega (nn.Module):
    """
    The classifier taking code
    """
    def __init__ (self, idx=60, nin=256, num_classes=2):
        super (RegaRega, self).__init__()
        self.name = "RegaRega"
        self.idx  = idx
        self.clf = nn.Sequential (
            nn.Linear (nin, 32),
            nn.LeakyReLU(),
            nn.Dropout(),
            nn.Linear (32, 16),
            nn.LeakyReLU(),
            nn.Dropout(),
            nn.Linear (16, num_classes),
            #nn.Softmax()
        )
    def forward (self, x):
        return self.clf (x)

class Zangetsu(nn.Module):
    """
    The classifier taking code
    """
    def __init__ (self, idx=60, nin=8, num_classes=2):
        super (Zangetsu, self).__init__()
        self.name = "Zangetsu"
        self.idx  = idx
        self.clf = nn.Sequential (
            nn.Linear (nin, 6),
            nn.ReLU (True),
            nn.Linear (6, 4),
            nn.ReLU (True),
            nn.Linear (4, 2),
            nn.Sigmoid()
        )

    def forward (self, x):
        x = self.clf (x)
        return x

class Zabimaru(nn.Module):
    """
    The classifier taking code
    """
    def __init__ (self, idx=60, nin=8, num_classes=2):
        super (Zabimaru, self).__init__()
        self.name = "Zabimaru"
        self.idx  = idx
        self.clf = nn.Sequential (
            nn.Linear (nin, 8),
            nn.ReLU (True),
            nn.Linear (8, 8),
            nn.ReLU (True),
            nn.Linear (8, 8),
            nn.ReLU (True),
            nn.Dropout(),
            nn.Linear (8, 8),
            nn.ReLU (True),
            nn.Linear (8, 8),
            nn.ReLU (True),
            nn.Dropout(),
            nn.Linear (8, 8),
            nn.ReLU (True),
            nn.Linear (8, num_classes),
            nn.Sigmoid()
        )

    def forward (self, x):
        x = self.clf (x)
        return x

class GearSecond (nn.Module):
    """
    A wholesome CNN network

    Convolutional Autoencoder
    Zabimaru fc mlp clf

    """
    def __init__ (self, idx=100, latent_dim=32, num_classes=2):
        super (GearSecond, self).__init__ ()
        self.name = "GearSecond"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.encode = nn.Sequential (
            nn.Conv2d (2, 16, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.Conv2d (16, 32, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.BatchNorm2d (32),
            nn.Conv2d (32, 64, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.Conv2d (64, 128, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.BatchNorm2d (128),
            nn.Conv2d (128, 256, kernel_size=2, stride=2),
            nn.LeakyReLU(),

        )
        ## reshape
        self.before_fc = [256,1,1]
        # lowest dim
        self.fc =  nn.Sequential (
            nn.Linear (256, 64),
            nn.LeakyReLU(),
            nn.Dropout(),
            nn.Linear (64, latent_dim),
            nn.LeakyReLU(),
            nn.Dropout(),
            nn.BatchNorm1d (latent_dim)
        )
        ## clf
        self.clf = nn.Sequential (
            nn.Linear (latent_dim, 32),
            nn.LeakyReLU (),
            nn.Dropout(),
            nn.Linear (32, 32),
            nn.LeakyReLU (),
            nn.Dropout(),
            nn.Linear (32, 16),
            nn.LeakyReLU (),
            nn.Dropout(),
            nn.Linear (16, 8),
            nn.LeakyReLU (),
            nn.Dropout(),
            nn.Linear (8, num_classes),
            #nn.Sigmoid()
        )
        ##
    def forward (self, x):
        x = self.encode (x)
        x = x.view([x.size(0), -1])
        x = self.fc (x)
        return self.clf (x)

class ZebelBlast(nn.Module):
    """
    A wholesome CNN network

    """
    def __init__ (self, idx=100, latent_dim=32, num_classes=2):
        super (ZebelBlast, self).__init__ ()
        self.name = "ZebelBlast"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.encode = nn.Sequential (
            nn.Conv2d (2, 16, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.Conv2d (16, 32, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.BatchNorm2d (32),
            nn.Conv2d (32, 64, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.Conv2d (64, 128, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.BatchNorm2d (128),
            nn.Conv2d (128, 256, kernel_size=2, stride=2),
            nn.LeakyReLU(),

        )
        ## reshape
        self.before_fc = [256,1,1]
        # lowest dim
        self.fc =  nn.Sequential (
            nn.Linear (256, 64),
            nn.LeakyReLU(),
            nn.Dropout(),
            nn.Linear (64, latent_dim),
            nn.LeakyReLU(),
            nn.Dropout(),
            nn.BatchNorm1d (latent_dim)
        )
        ## clf
        self.clf = nn.Sequential (
            nn.Linear (latent_dim, 32),
            nn.LeakyReLU (),
            nn.Dropout(),
            nn.Linear (32, 32),
            nn.LeakyReLU (),
            nn.Dropout(),
            nn.Linear (32, 16),
            nn.LeakyReLU (),
            nn.Dropout(),
            nn.Linear (16, 8),
            nn.LeakyReLU (),
            nn.Dropout(),
            nn.Linear (8, num_classes),
            #nn.Sigmoid()
        )
        ##
    def forward (self, x):
        x = self.encode (x)
        x = x.view([x.size(0), -1])
        x = self.fc (x)
        return self.clf (x)

class Battousai(nn.Module):
    """
    A wholesome CNN network

    """
    def __init__ (self, idx=100, latent_dim=32, num_classes=2):
        super (Battousai, self).__init__ ()
        self.name = "Battousai"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.encode = nn.Sequential (
            nn.Conv2d (2, 16, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.Conv2d (16, 32, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.BatchNorm2d (32),
            nn.Conv2d (32, 64, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.Conv2d (64, 128, kernel_size=2, stride=2),
            nn.LeakyReLU(),
            nn.Dropout2d (),
            nn.BatchNorm2d (128),
            nn.Conv2d (128, 256, kernel_size=2, stride=2),
            nn.LeakyReLU(),

        )
        ## reshape
        self.before_fc = [256,1,1]
        # lowest dim
        self.fc =  nn.Sequential (
            nn.Linear (256, 64),
            nn.LeakyReLU(),
            nn.Dropout(),
            nn.Linear (64, latent_dim),
            nn.LeakyReLU(),
            nn.Dropout(),
            nn.BatchNorm1d (latent_dim)
        )
        ## clf
        self.clf = nn.Sequential (
            nn.Linear (latent_dim, 16),
            nn.LeakyReLU (),
            nn.Dropout(),
            nn.Linear (16, 8),
            nn.LeakyReLU (),
            nn.Dropout(),
            nn.Linear (8, num_classes),
            #nn.Sigmoid()
        )
        ##
    def forward (self, x):
        x = self.encode (x)
        x = x.view([x.size(0), -1])
        x = self.fc (x)
        return self.clf (x)

class GearFourth (nn.Module):
    """
    A wholesome CNN network

    SoruSoru Convolutional Autoencoder
    Zabimaru fc mlp clf
    

    Input 
    (2, 32, 32) 
    -> (16, 16, 16) 
    -> (32, 8,  8)
    -> (64,4,4)
    -> FC 1024 <-
    -> FC 64 <-
    -> FC 8 <-
    ## clf

    """
    def __init__ (self, idx=100, latent_dim=16, num_classes=2):
        super (GearFourth, self).__init__ ()
        self.name = "GearFourth"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.c1  = nn.Conv2d (2, 16,    kernel_size=2, stride=2)
        self.c2  = nn.Conv2d (16, 32,   kernel_size=2, stride=2)
        self.c3  = nn.Conv2d (32, 64,   kernel_size=2, stride=2)
        self.c4  = nn.Conv2d (64, 128,  kernel_size=2, stride=2)
        self.c5  = nn.Conv2d (128, 256, kernel_size=2, stride=2)
        ## reshape
        self.before_fc = [256,1,1]
        # lowest dim
        self.fc1 = nn.Linear (256, 64)
        self.fc2 = nn.Linear (64, latent_dim)
        ## clf
        self.clf = nn.Sequential (
            nn.Linear (latent_dim, 8),
            nn.LeakyReLU (),
            nn.Linear (8, 8),
            nn.LeakyReLU (),
            nn.Linear (8, 8),
            nn.LeakyReLU (),
            nn.Linear (8, 8),
            nn.LeakyReLU (),
            nn.Linear (8, num_classes),
            #nn.Sigmoid()
        )
        ##
    def forward (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = F.relu (self.c3 (x))
        x = x.view([x.size(0), -1])
        x = F.relu (self.fc1 (x))
        x = t.sigmoid (self.fc2 (x))
        return self.clf (x)

class GearThird (nn.Module):
    """
    A wholesome CNN network

    SoruSoru Convolutional Autoencoder
    Zabimaru fc mlp clf
    

    Input 
    (2, 32, 32) 
    -> (16, 16, 16) 
    -> (32, 8,  8)
    -> (64,4,4)
    -> FC 1024 <-
    -> FC 64 <-
    -> FC 8 <-
    ## clf

    """
    def __init__ (self, idx=100, latent_dim=8, num_classes=2):
        super (GearThird, self).__init__ ()
        self.name = "GearThird"
        self.idx  = idx
        self.input_shape = [2, 32, 32]
        # encode 
        self.c1  = nn.Conv2d (2, 16, kernel_size=3, stride=1)
        self.c2  = nn.Conv2d (16, 32, kernel_size=3, stride=1)
        self.c3  = nn.Conv2d (32, 64, kernel_size=3, stride=1)
        ## reshape
        self.before_fc = [64,26,26]
        # lowest dim
        self.fc1 = nn.Linear (43264, 512)
        self.fc2 = nn.Linear (512, num_classes)
        ## clf
        ##
    def forward (self, x):
        x = F.relu (self.c1 (x))
        x = F.relu (self.c2 (x))
        x = F.relu (self.c3 (x))
        x = x.view([x.size(0), -1])
        x = F.relu (self.fc1 (x))
        x = t.sigmoid (self.fc2 (x))
        return x

class Zanka(nn.Module):
    """
    The classifier taking code
    """
    def __init__ (self, idx=60, nin=8, num_classes=2):
        super (Zanka, self).__init__()
        self.name = "Zanka"
        self.idx  = idx
        self.clf = nn.Sequential (
            nn.Linear (nin, 8),
            nn.Sigmoid(),
            nn.Dropout(),
            nn.Linear (8, 4),
            nn.Sigmoid(),
            nn.Dropout(),
            nn.Linear (4, num_classes),
            # nn.Sigmoid()
        )

    def forward (self, x):
        x = self.clf (x)
        return x
