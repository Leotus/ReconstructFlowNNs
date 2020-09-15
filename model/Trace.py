import torch
import torch.nn as nn
import torch.nn.functional as F
import torchvision
from torch.utils.data import Dataset, DataLoader,random_split
import numpy as np
import math
import sys
import vtk

from FlowDataset import FlowDataset,Normalize,Unnormalize
from FNN import NeuralNet


# Device configuration
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

# Hyper-parameters
decode_num = 30 # every group has decode_num datas
datapath_str = {
    "10":"./data/Group10-data-search2.txt",
    "30":"./data/Group30-data-search2.txt",
    "50":"./data/Group50-data-search3.txt",
    "100":"./data/Group100-data-search4.txt",
}

data_path = datapath_str[str(decode_num)] # data path
FILE = f"./models/model_{decode_num}_fnn.pth"

dataset = FlowDataset(datapath=data_path,decodenum=decode_num)

# Test the model
model = torch.load(FILE)
model.to(device)
model.eval()

# test model
with torch.no_grad():
    i = 1
    for inputs, targets in dataset:
        if(i == 1):
            i += 1
            continue
        inputs = inputs.reshape(-1, 6*decode_num).to(device)
        # inputs = inputs.unsqueeze(1).to(device)
        targets = targets.to(device)
        print(f"targets {targets}")
        traced_net = torch.jit.script(model,inputs)
        outputs = traced_net(inputs)
        print(f"outputs {outputs}")
        traced_net.save(f"./models/model_{decode_num}_fnn.pt")
        print("模型序列化导出成功")
        sys.exit()





