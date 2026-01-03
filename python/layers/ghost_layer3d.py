import torch
import math


class GhostLayer3D(torch.nn.Module):
    def __init__(self,
                 in_channels: int,
                 out_channels: int,
                 ratio: int = 2,
                 stride=1, #T, H ,W 
                 use_activation: bool = True,
                 activation=torch.nn.ReLU):
        super().__init__()

        if ratio < 1:
            raise ValueError("ratio must be >= 1")
        self.oup = out_channels


        if isinstance(stride, int):
            stride_3d = (stride, stride, stride)
        else:
            if len(stride) != 3:
                raise ValueError("stride must be int or tuple of length 3")
            stride_3d = tuple(int(s) for s in stride)

        init_channels = int(math.ceil(out_channels / ratio))
        new_channels = init_channels * (ratio - 1)

        # 시간과 공간만 비비기
        self.primary_conv = torch.nn.Sequential(
            torch.nn.Conv3d(
                in_channels=in_channels,
                out_channels=init_channels,
                kernel_size=(3, 3, 3),
                stride=stride_3d,
                padding=(1, 1, 1),
                bias=False
            ),
            torch.nn.BatchNorm3d(init_channels),
            activation(inplace=True) if use_activation else torch.nn.Identity(),
        )

        # 공간만 비비기
        if new_channels > 0:
            self.cheap_operation = torch.nn.Sequential(
                torch.nn.Conv3d(
                    in_channels=init_channels,
                    out_channels=new_channels,
                    kernel_size=(1, 3, 3),
                    stride=(1, 1, 1),
                    padding=(0, 1, 1),
                    groups=init_channels,     # depthwise over channels
                    bias=False
                ),
                torch.nn.BatchNorm3d(new_channels),
                activation(inplace=True) if use_activation else torch.nn.Identity(),
            )
        else:
            self.cheap_operation = None

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x1 = self.primary_conv(x) 
        if self.cheap_operation is None:
            out = x1
        else:
            x2 = self.cheap_operation(x1) 
            out = torch.cat([x1, x2], dim=1) 

        return out[:, :self.oup, :, :, :]