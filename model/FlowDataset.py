import torch
import torch.nn as nn
import torch.nn.functional as F
import torchvision
from torch.utils.data import Dataset, DataLoader,random_split
import numpy as np
import vtk
import math
import sys

class FlowDataset(Dataset):
    def __init__(self,datapath="",transform=None,decodenum=0):
        # Initialize data
        # read with numpy or pandas
        xy = np.loadtxt(datapath, delimiter=' ', dtype=np.float32)
        # here the last three column is the class label, the rest are the features
        self.xy_max = np.max(xy,axis=0)
        self.xy_min = np.min(xy,axis=0)
        self.x_max = self.xy_max[0:6]
        self.x_min = self.xy_min[0:6]
        self.y_max = self.xy_max[6:9]
        self.y_min = self.xy_min[6:9]
        self.y_data = torch.from_numpy(xy[::decodenum, 6:9]) # size [n_samples, n_targets]
        self.n_samples = self.y_data.shape[0]
        x_data = xy[:,0:6]
        x_data = np.array(np.vsplit(x_data,self.n_samples))
        self.x_data = torch.from_numpy(x_data) # size [n_samples, n_features]
        self.transform = transform
        self.data_path = datapath
    # support indexing such that dataset[i] can be used to get i-th sample
    def __getitem__(self, index):
        sample = self.x_data[index],self.y_data[index]
        if self.transform:
            sample = self.transform(sample,self.x_max,self.x_min,self.y_max,self.y_min)
        return sample
    # we can call len(dataset) to return the size
    def __len__(self):
        return self.n_samples
    def getYRange(self):
        return self.y_max,self.y_min

class Normalize:
    # Convert range to -1~1
    def __call__(self, sample, xmax, xmin, ymax, ymin):
        inputs, targets = sample
        kx = (1-(-1))/(xmax-xmin)
        ky = (1-(-1))/(ymax-ymin)
        with torch.no_grad():
            xmin = np.tile(xmin,(len(inputs),1))
            kx = np.tile(kx,(len(inputs),1))
            inputs = (-1) + np.multiply(inputs - xmin,kx)
            targets = (-1) + np.multiply(targets - ymin,ky)
        return inputs,targets

class Unnormalize:
    # Convert range to normal
    def call(self,sample,ymax,ymin):
        targets = sample
        ky = (ymax-ymin)/(1-(-1))
        with torch.no_grad():
            targets = np.multiply((targets + 1),ky ) + ymin
        return targets
