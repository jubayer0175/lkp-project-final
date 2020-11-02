# -*- coding: utf-8 -*-
"""
Created on Sun Nov  1 20:10:00 2020
@author: Jubayer, Virginia Tech
@Description:
    
"""


import os, sys
import numpy as np
import math
import matplotlib.pyplot as plt
#from scipy.interpolate import interp1d
import seaborn as sns
import re

import matplotlib
font = {'family' : 'sans-serif',
        'weight' : 'bold',
        'size'   : 14}
font = {'weight':'bold', 'size':'14'}

with open('debug_msg.txt', 'r') as f:
    p = f.readlines()
    a=[]

min_cyc =[]
var = []
for i in range(500):
    a.append(p[i].strip('\n'))
    c= a[i]
    min_cyc.append(int(c[-3:]))
    var.append(int(c[34:36]))

fig, ax = plt.subplots()
ax.plot(min_cyc, label = 'Minimum cycles overhead', color = 'gray')
ax.set_xlabel('Measurement run', **font)
ax.set_ylabel('# cycles', **font)
plt.savefig('min.pdf', transparent=True)


fig2, ax2 = plt.subplots()
plt.hist(var, label = 'Variance', color = 'grey', linewidth = 3)
plt.xlabel('Variance (Cycle)', **font)
plt.ylabel(' Measurement run', **font)
plt.savefig('var.pdf', transparent=True)



