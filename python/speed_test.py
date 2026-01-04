import torch
import time

from torch.utils.data import DataLoader
from model.ghostnet3d import GhostNet3D



device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(f"현재 사용 중인 디바이스: {device}")



batch_size = 8
epochs = 100
learning_rate = 0.003
image_width = 128
image_height = 64
image_channel = 3
class_num = 27
frames = 16
weight_decay = 0.0001

dataset_path = "C://github//dataset//jester"  
weight_path = "C://github//HGR//python//results//weights.pth"
onnx_model_path = "C://github//HGR//python//results//model.onnx"
onnx_quant_model_path =  "C://github//HGR//python//results//quant_model.onnx"


dummy_input = torch.randn(size=(3, image_channel, frames, image_height, image_width)).to(device)
model = GhostNet3D(in_channels=image_channel, class_num=class_num).to(device)


# 워밍업
with torch.no_grad():
    for _ in range(50):
        _ = model(dummy_input)
    if device.type == "cuda":
        torch.cuda.synchronize()

print("측정 시작 (Ctrl+C 종료)")

try:
    with torch.no_grad():
        if device.type == "cuda":
            starter = torch.cuda.Event(enable_timing=True)
            ender   = torch.cuda.Event(enable_timing=True)

            count = 0
            acc_ms = 0.0

            while True:
                starter.record()
                _ = model(dummy_input)
                ender.record()
                torch.cuda.synchronize()

                ms = starter.elapsed_time(ender)  # 1회 inference GPU time (ms)
                acc_ms += ms
                count += 1

                if count % 200 == 0:
                    avg_ms = acc_ms / 200.0
                    fps = 1000.0 / avg_ms
                    print(f"[avg 200] {avg_ms:.3f} ms / infer | {fps:.2f} infer/s")
                    acc_ms = 0.0

        else:
            count = 0
            acc_ms = 0.0

            while True:
                t0 = time.perf_counter()
                _ = model(dummy_input)
                t1 = time.perf_counter()

                ms = (t1 - t0) * 1000.0
                acc_ms += ms
                count += 1

                if count % 200 == 0:
                    avg_ms = acc_ms / 200.0
                    fps = 1000.0 / avg_ms
                    print(f"[avg 200] {avg_ms:.3f} ms / infer | {fps:.2f} infer/s")
                    acc_ms = 0.0

except KeyboardInterrupt:
    print("측정 종료")