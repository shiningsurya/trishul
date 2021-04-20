
Our ML principle is inspired by FETCH/VLITE-Fast and all.

Inputs are 
- Bowtie plane (Ndm x Ntime)
- De-dispersed filterbank (Nfreq x Ntime)

For simplicity sake, 
Ndm = Ntime = Nfreq = 256

DM-axis: 256 DM trials, [DM-128, DM+128] where DDM is DM delay units
Freq-axis: fs-crunched to [256], so it is assumed that its a nice-number (power of two) and atleast 256.
Time-axis: max (1, Width/2) time resolution where width is pow(2,boxcar-width).

The minimum number of channels required for ML is 256.
TSIZE = 256
DSIZE = 256
FSIZE = 256


"LABEL"
 -1 --> unclassified
 0  --> RFI/noise shit
 1  --> gold
