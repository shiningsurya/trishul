from . import dedisp as dd
import numpy          as np
from scipy.stats      import skew, kurtosis

class Learnings (object):
  def __init__(self, names, description, formats, comments="#", delimiter=","):
    self.names     = names
    self.desc      = description
    self.formats   = formats
    self.nfeatures = len (self.names)
    #
    self.features  = [ [] for n in range(self.nfeatures) ]
    #
    self.comments  = comments
    self.delimiter = delimiter
    self.newline   = "\n"
    #
    self.strformat = ""
    for i in range (self.nfeatures):
      self.strformat = self.strformat + "{" + str(i) + ":" + self.formats[i] + "}"
      if i != self.nfeatures-1:
        self.strformat = self.strformat + ","
    self.strformat = self.strformat + "\n"
    #
    self.nrows     = 0

  def SetupWrite (self, filename, header):
    '''Sets up writer '''
    self.fo = open(filename, "w+")
    self.fo.write (self.comments + header + self.newline)
    for i in range (self.nfeatures):
      self.fo.write (self.comments + self.names[i] + self.delimiter + self.desc[i] + self.newline)
    self.fo.flush ()

  def Write (self):
    '''Write features'''
    for i in range (self.nrows):
      ff = [self.features[fi][i] for fi in range (self.nfeatures)]
      self.fo.write (self.strformat.format(*ff))
    self.fo.flush()
    for a_ in self.features:
      a_[:] = [] # clearing
  
  def __del__ (self):
    self.fo.close ()


class Lyon (Learnings):
  def __init__ (self, filename, header = "Lyon features"):
    names = [
      "tm", "tv", "ts", "tk", 
      "dm", "dv", "ds", "dk" 
    ]
    desc = [
      "Mean of S/N (t)",
      "Variance of S/N (t)",
      "Skewness of S/N (t)",
      "Kurtosis of S/N (t)",
      "Mean of S/N (DM)",
      "Variance of S/N (DM)",
      "Skewness of S/N (DM)",
      "Kurtosis of S/N (DM)"
    ]
    form = ["5.3f"] * 8
    super (Lyon, self).__init__ (names, desc, form)
    self.SetupWrite (filename, header)

  def Extract (self,x):
    ''' x is fbson
    '''
    # generate bowtie
    dms, delays = dd.DMRanger (x)
    pt = int ( round (x.peak_time / x.tsamp ) )
    dc = int ( round (dms.size // 2 ) )
    bt = dd.BowTie (x, delays)
    # slice
    tsn = bt [dc]
    dsn = bt [:,pt]
    self.features[0].append (np.mean(tsn) )
    self.features[1].append (np.var(tsn) )
    self.features[2].append (skew(tsn) )
    self.features[3].append (kurtosis(tsn) )
    self.features[4].append (np.mean(dsn) )
    self.features[5].append (np.var(dsn) )
    self.features[6].append (skew(dsn) )
    self.features[7].append (kurtosis(dsn) )
    self.nrows = self.nrows + 1

if __name__ == "__main__":
  n = ["test1","test2","test3"]
  d = ["this","is","test"]
  f = ["3.2f","05.2f","3f"]
  l = Learnings (n,d,f)
  l.SetupWrite ("tslearnings.test","alakazam")
  l.features[0].append (1.0)
  l.features[1].append (1.7)
  l.features[2].append (723.232)
  l.Write (1)

