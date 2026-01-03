import torch
import math


class GhostModule(torch.nn.Module):
    def __init__(self, 
                 in_channels, 
                 out_channels, 
                 kernel_size=1, 
                 ratio=2, 
                 dw_size=3, 
                 stride=1, 
                 use_activation=True, 
                 activation=torch.nn.ReLU):
        super(GhostModule, self).__init__()
        self.oup = out_channels
        init_channels = math.ceil(out_channels / ratio)
        new_channels = init_channels*(ratio-1)

        self.primary_conv = torch.nn.Sequential(
            torch.nn.Conv2d(in_channels, init_channels, kernel_size, stride, kernel_size//2, bias=False),
            torch.nn.BatchNorm2d(init_channels),
            activation(inplace=True) if use_activation else torch.nn.Sequential(),
        )

        self.cheap_operation = torch.nn.Sequential(
            torch.nn.Conv2d(init_channels, new_channels, dw_size, 1, dw_size//2, groups=init_channels, bias=False),
            torch.nn.BatchNorm2d(new_channels),
            activation(inplace=True) if use_activation else torch.nn.Sequential(),
        )

    def forward(self, x):
        x1 = self.primary_conv(x)
        x2 = self.cheap_operation(x1)
        out = torch.cat([x1, x2], dim=1)
        return out[:,:self.oup,:,:]