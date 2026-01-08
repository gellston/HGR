import torch
import os
import cv2
import numpy as np
from collections import deque

from model.ghostnet3d import GhostNet3D
from model.softmax_model import SoftmaxModel

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(f"현재 사용 중인 디바이스: {device}")

image_width = 128
image_height = 64
image_channel = 3
class_num = 27
frames = 16

weight_path = "C://github//HGR//python//results//weights.pth"

model = GhostNet3D(in_channels=image_channel, class_num=class_num).to(device)
if os.path.exists(weight_path):
    state_dict = torch.load(weight_path, map_location=device)
    model.load_state_dict(state_dict)

net = SoftmaxModel(backbone=model).to(device)
net.eval()

# ===== 설정 =====
ACCUM_FRAMES = 25       # 30fps 환경에서 TARGET_FPS=17, frames=16이면 32 이상 권장
TARGET_FPS = 17
STRIDE = 1              # 몇 프레임마다 추론할지
EMA_ALPHA = 0.7        # 0.7~0.95 (클수록 더 안정적, 반응은 느려짐)

GESTURES = [
    "Doing other things","No gesture","Drumming Fingers","Pulling Hand In","Pulling Two Fingers In",
    "Pushing Hand Away","Pushing Two Fingers Away","Rolling Hand Backward","Rolling Hand Forward",
    "Shaking Hand","Sliding Two Fingers Down","Sliding Two Fingers Left","Sliding Two Fingers Right",
    "Sliding Two Fingers Up","Stop Sign","Swiping Down","Swiping Left","Swiping Right","Swiping Up",
    "Thumb Down","Thumb Up","Turning Hand Clockwise","Turning Hand Counterclockwise",
    "Zooming In With Full Hand","Zooming In With Two Fingers","Zooming Out With Full Hand",
    "Zooming Out With Two Fingers",
]

def preprocess(frame_bgr):
    x = cv2.resize(frame_bgr, (image_width, image_height))
    x = cv2.cvtColor(x, cv2.COLOR_BGR2RGB).astype(np.float32) / 255.0
    return x  # (H,W,C)

def sample_by_target_fps(buf_frames_rgb, cam_fps, target_fps, out_len):
    L = len(buf_frames_rgb)
    if L < out_len:
        return None

    if cam_fps is None or cam_fps <= 1:
        idx = np.linspace(0, L - 1, out_len).round().astype(int)
        return [buf_frames_rgb[i] for i in idx]

    step = max(1, int(round(cam_fps / target_fps)))
    need = 1 + (out_len - 1) * step
    if L < need:
        return None

    start = L - need
    idx = start + np.arange(out_len) * step
    return [buf_frames_rgb[i] for i in idx]

@torch.no_grad()
def infer_probs(net, sampled_frames_rgb):
    clip = np.stack(sampled_frames_rgb, axis=0)        # (T,H,W,C)
    clip = np.transpose(clip, (3, 0, 1, 2))            # (C,T,H,W)
    x = torch.from_numpy(clip).unsqueeze(0).to(device) # (1,C,T,H,W)

    probs = net(x)
    probs = probs[0] if isinstance(probs, (list, tuple)) else probs
    return probs.squeeze(0)  # (C,)

# ===== 실행부 =====
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
if not cap.isOpened():
    raise RuntimeError("카메라를 열 수 없습니다. cam_index(0/1/2...)를 바꿔보세요.")

cam_fps = cap.get(cv2.CAP_PROP_FPS)
print(f"Camera FPS: {cam_fps if cam_fps else 'Unknown'}")

buf = deque(maxlen=ACCUM_FRAMES)  # ✅ 한 칸씩 밀리는 슬라이딩 버퍼
ema = None                        # ✅ EMA 상태(확률 벡터)
frame_count = 0

print("웹캠 제스처 테스트 시작 (q 종료)")

while True:
    ok, frame = cap.read()
    if not ok:
        print("프레임을 읽지 못했습니다. 종료합니다.")
        break

    cv2.imshow("Webcam", frame)
    buf.append(preprocess(frame))
    frame_count += 1

    if len(buf) < ACCUM_FRAMES:
        if (cv2.waitKey(1) & 0xFF) == ord('q'):
            break
        continue

    if frame_count % STRIDE == 0:
        sampled = sample_by_target_fps(list(buf), cam_fps, TARGET_FPS, frames)
        if sampled is not None:
            probs = infer_probs(net, sampled)  # (C,)

            # ===== EMA 적용 =====
            if ema is None:
                ema = probs.detach().clone()
            else:
                ema = EMA_ALPHA * ema + (1.0 - EMA_ALPHA) * probs

            pred = int(torch.argmax(ema).item())
            conf = float(torch.max(ema).item())
            name = GESTURES[pred] if pred < len(GESTURES) else str(pred)
            print(f"{name}  conf(EMA)={conf:.3f}")

    if (cv2.waitKey(1) & 0xFF) == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()