import torch
import torch.nn as nn
import torch.nn.functional as F
import torchvision
from torch.utils.data import Dataset, DataLoader,random_split
import numpy as np
import vtk
import math
import sys

from FlowDataset import FlowDataset,Normalize


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
decode_num = 30 # every group has decode_num datas
input_size = 6 * decode_num # 6 * decode_num
hidden_size = 5 # FNN hidden size
target_size = 3 # 3 * 1
num_epochs = 30
batch_size = 100
learning_rate = 0.001
data_path = datapath_str[str(decode_num)] # data path


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
        self.conv1 = nn.Conv2d(1,3,(5,3),(1,3))
        h = decode_num - 5 + 1
        w = (6 - 3)/3 + 1
        self.pool = nn.MaxPool2d((2,1))
        h = int(h / 2)
        w = int(w / 1)
        self.conv2 = nn.Conv2d(3,6,(3,1))
        h = h - 3 + 1
        h = int(h / 2)
        w = int(w / 1)
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

if __name__ == '__main__':

    print(data_path)
    writer = SummaryWriter(f'runs/group_{decode_num}_fnn')
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
    # print(example_data)
    # print(example_targets)



    model = NeuralNet(input_size, hidden_size, target_size).to(device)
    # model = ConvNet().to(device)

    # Loss and optimizer
    criterion = nn.MSELoss()
    optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate)

    ############## TENSORBOARD ########################
    writer.add_graph(model, example_data.reshape(-1, 6*decode_num).to(device))
    # writer.add_graph(model, example_data.unsqueeze(1).to(device))
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
            inputs = inputs.reshape(-1, 6*decode_num).to(device)
            targets = targets.to(device)

            # CNN do not need reshape
            # inputs = inputs.unsqueeze(1).to(device)
            # targets = targets.to(device)

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
    torch.save(model, f'./models/model_{decode_num}_fnn.pth')
    writer.close()