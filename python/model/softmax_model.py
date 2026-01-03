
import torch


class SoftmaxModel(torch.nn.Module):
    def __init__(self, 
                 backbone:torch.nn.Module
                 ):
        super(SoftmaxModel, self).__init__()

        self.backbone = backbone
        

    def forward(self, x):
        x = self.backbone(x)
        x = torch.softmax(x, dim=1)

        return x
    