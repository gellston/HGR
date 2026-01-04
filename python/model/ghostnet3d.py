import torch

from layers.ghost_layer3d import GhostLayer3D

class GhostNet3D(torch.nn.Module):
    def __init__(self, 
                 in_channels, 
                 class_num,
                 ):
        super(GhostNet3D, self).__init__()


        # 시간축 frame 16으로 시작
        # 공간 크기 128x64
        self.stem = torch.nn.Sequential(
            GhostLayer3D(in_channels=in_channels, out_channels=16)
        )

        # 시간축 frame 8로 압축
        # 공간 크기 64x32로 압축
        self.layer1 = torch.nn.Sequential(
            GhostLayer3D(in_channels=16, out_channels=24, stride=2),
            GhostLayer3D(in_channels=24, out_channels=24)
        )


        # 시간축 frame 8로 유지
        # 공간 크기 32x16
        self.layer2 = torch.nn.Sequential(
            GhostLayer3D(in_channels=24, out_channels=40, stride=(1,2,2)),
            GhostLayer3D(in_channels=40, out_channels=40),
        )


        # 시간축 frame 8로 유지
        # 공간 크기 16x8
        self.layer3 = torch.nn.Sequential(
            GhostLayer3D(in_channels=40, out_channels=80, stride=(1,2,2)),
            GhostLayer3D(in_channels=80, out_channels=80),
            GhostLayer3D(in_channels=80, out_channels=80),
            GhostLayer3D(in_channels=80, out_channels=80),
            GhostLayer3D(in_channels=80, out_channels=112),
            GhostLayer3D(in_channels=112, out_channels=112),
        )

        # 시간축 frame 8로 유지
        # 공간 크기 8x4
        self.layer4 = torch.nn.Sequential(
            GhostLayer3D(in_channels=112, out_channels=160, stride=(1,2,2)),
            GhostLayer3D(in_channels=160, out_channels=160),
            GhostLayer3D(in_channels=160, out_channels=160),
            GhostLayer3D(in_channels=160, out_channels=160),
            GhostLayer3D(in_channels=160, out_channels=160),
        )

        self.gap = torch.nn.AdaptiveAvgPool3d((1,1,1))
        self.fc = torch.nn.Linear(160, class_num)
        

    def forward(self, x):

        ## 디버깅용 시간축 16에서 시작 확인용
        B, C, T, H, W = x.shape

        x = self.stem(x)
        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)
        x = self.layer4(x)

        x = self.gap(x)
        x = x.view(x.size(0), -1)
        x = self.fc(x)

        return x
    