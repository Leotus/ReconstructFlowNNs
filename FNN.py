import torch
import torch.nn as nn
import torch.nn.functional as F
import torchvision
from torch.utils.data import Dataset, DataLoader,random_split
import numpy as np
import math
import sys

############## TENSORBOARD ########################
import sys
import torch.nn.functional as F
from torch.utils.tensorboard import SummaryWriter
# default `log_dir` is "runs" - we'll be more specific here
###################################################

datapath_str = {
    "10":"./data/Group10-data-search2.txt",
    "30":"./data/Group30-data-search2.txt",
    "50":"./data/Group50-data-search3.txt",
    "100":"./data/Group100-data-search4.txt",
}

# Device configuration
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

# Hyper-parameters
decode_num = 50 # every group has decode_num datas
input_size = 6 * decode_num # 6 * decode_num
hidden_size = 5 # FNN hidden size
target_size = 3 # 3 * 1
num_epochs = 30
batch_size = 100
learning_rate = 0.001
data_path = datapath_str[str(decode_num)] # data path
writer = SummaryWriter(f'runs/group_{decode_num}_cnn')

print(data_path)

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

class FlowDataset(Dataset):
    def __init__(self,datapath="",transform=None,decodenum=0):
        # Initialize data
        # read with numpy or pandas
        xy = np.loadtxt(data_path, delimiter=' ', dtype=np.float32)
        # here the last three column is the class label, the rest are the features
        self.xy_max = np.max(xy,axis=0)
        self.xy_min = np.min(xy,axis=0)
        self.x_max = self.xy_max[0:6]
        self.x_min = self.xy_min[0:6]
        self.y_max = self.xy_max[6:9]
        self.y_min = self.xy_min[6:9]
        self.y_data = torch.from_numpy(xy[::decode_num, 6:9]) # size [n_samples, n_targets]
        self.n_samples = self.y_data.shape[0]
        x_data = xy[:,0:6]
        x_data = np.array(np.vsplit(x_data,self.n_samples))
        self.x_data = torch.from_numpy(x_data) # size [n_samples, n_features]
        self.transform = transform
        self.data_path = data_path
    # support indexing such that dataset[i] can be used to get i-th sample
    def __getitem__(self, index):
        sample = self.x_data[index],self.y_data[index]
        if self.transform:
            sample = self.transform(sample,self.x_max,self.x_min,self.y_max,self.y_min)
        return sample
    # we can call len(dataset) to return the size
    def __len__(self):
        return self.n_samples


# create dataset
dataset = FlowDataset(datapath=data_path,transform=Normalize(),decodenum=decode_num)

test_size = int(len(dataset) * 0.2)
train_size = len(dataset) - test_size
train_ds, test_ds = random_split(dataset, [train_size, test_size])
print(len(train_ds),len(test_ds))

# Load whole dataset with DataLoader
# shuffle: shuffle data, good for training
# num_workers: faster loading with multiple subprocesses
train_loader = DataLoader(dataset=train_ds,
                          batch_size=batch_size,
                          shuffle=True)
test_loader = DataLoader(dataset=test_ds,
                          batch_size=batch_size,
                          shuffle=False)

examples = iter(test_loader)
example_data, example_targets = examples.next()
print(example_data)
print(example_targets)

# Fully connected neural network with one hidden layer
class NeuralNet(nn.Module):
    def __init__(self, input_size, hidden_size, target_size):
        super(NeuralNet, self).__init__()
        self.input_size = input_size
        self.l1 = nn.Linear(input_size, hidden_size)
        self.relu = nn.ReLU()
        self.l2 = nn.Linear(hidden_size, target_size)

    def forward(self, x):
        out = self.l1(x)
        out = self.relu(out)
        out = self.l2(out)
        return out

# Conv net
class ConvNet(nn.Module):
    def __init__(self):
        super(ConvNet, self).__init__()
        self.conv1 = nn.Conv2d(1,3,(5,3))
        h = decode_num - 5 + 1
        w = 6 - 3 + 1
        self.pool = nn.MaxPool2d(2,2)
        h = int(h / 2)
        w = int(w / 2)
        self.conv2 = nn.Conv2d(3,6,(3,1))
        h = h - 3 + 1
        h = int(h / 2)
        w = int(w / 2)
        self.fc1 = nn.Linear(6*h*w,10)
        self.fc2 = nn.Linear(10,5)
        self.fc3 = nn.Linear(5,3)
        self.convh = h
        self.convw = w

    def forward(self,x):
        x = self.pool(F.relu(self.conv1(x)))
        x = self.pool(F.relu(self.conv2(x)))
        x = x.view(-1,6*self.convh*self.convw)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)
        return x

# model = NeuralNet(input_size, hidden_size, target_size).to(device)
model = ConvNet().to(device)

# Loss and optimizer
criterion = nn.MSELoss()
optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate)

############## TENSORBOARD ########################
# writer.add_graph(model, example_data.reshape(-1, 6*decode_num).to(device))
writer.add_graph(model, example_data.unsqueeze(1).to(device))
# writer.close()
# sys.exit()
###################################################

# Train the model
running_loss = 0.0
n_total_steps = len(train_loader)
for epoch in range(num_epochs):
    for i, (inputs, targets) in enumerate(train_loader):
        # FNN needs reshape
        # origin shape: [100, 6, decode_num]
        # resized: [100, 60]
        # inputs = inputs.reshape(-1, 6*decode_num).to(device)

        # CNN do not need reshape
        inputs = inputs.unsqueeze(1).to(device)
        targets = targets.to(device)

        # Forward pass
        outputs = model(inputs)
        loss = criterion(outputs, targets)

        # Backward and optimize
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        running_loss += loss.item()

        if (i+1) % 500 == 0:
            print (f'Epoch [{epoch+1}/{num_epochs}], Step [{i+1}/{n_total_steps}], Loss: {loss.item():.4f}')
            # print (f"inputs {inputs},targets {targets},outputs {outputs}")
            ############## TENSORBOARD ########################
            writer.add_scalar('training loss', running_loss / 500, epoch * n_total_steps + i)
            running_loss = 0
            ###################################################

print("Finished Training")
# Test the model
with torch.no_grad():
    n_l1_loss = 0
    n_mse_loss = 0
    n_samples = 0
    i = 0
    for inputs, targets in test_loader:
        # inputs = inputs.reshape(-1, 6*decode_num).to(device)
        inputs = inputs.unsqueeze(1).to(device)
        targets = targets.to(device)
        outputs = model(inputs)
        i+=1
        if (i+1) % 100 == 0:
             print(f"Index {i+1} : targets {targets[0]}")
             print(f"Index {i+1} : outputs {outputs[0]}")
        # max returns (value ,index)
        n_samples += targets.size(0)
        criterionL1 = nn.L1Loss()
        lossL1 = criterionL1(outputs, targets)
        n_l1_loss += lossL1
        criterionMSE = nn.MSELoss()
        lossMSE = criterionMSE(outputs, targets)
        n_mse_loss += lossMSE

    avl1loss = n_l1_loss / n_samples
    avmseloss = n_mse_loss / n_samples
    print(f'L1Loss of the network on the test data: {avl1loss} ')
    print(f'MSELoss of the network on the test data: {avmseloss} ')

torch.save(model, f"./model/model-group-{decode_num}-cnn.pth")
writer.close()