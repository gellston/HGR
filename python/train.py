import torch
import os

from torch.utils.data import DataLoader
from datasets.jester_loader import JesterDataset
from model.ghostnet3d import GhostNet3D
from model.softmax_model import SoftmaxModel


from onnxruntime.quantization.shape_inference import quant_pre_process


device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(f"현재 사용 중인 디바이스: {device}")



batch_size = 8
epochs = 100
learning_rate = 0.0003
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

train_ds = JesterDataset(dataset_path, 
                         split="train", 
                         num_frames=16, 
                         image_width=image_width, 
                         image_height=image_height, 
                         training=True)
train_dl = DataLoader(train_ds, batch_size=batch_size, shuffle=True, drop_last=True)

valid_ds = JesterDataset(dataset_path, split="validation", 
                         num_frames=16, 
                         image_width=image_width, 
                         image_height=image_height, 
                         training=False)
valid_dl = DataLoader(valid_ds, batch_size=batch_size, shuffle=True, drop_last=True)


model = GhostNet3D(in_channels=image_channel, class_num=class_num).to(device)
if os.path.exists(weight_path):
    state_dict = torch.load(weight_path, map_location=device)
    model.load_state_dict(state_dict)



loss_fn = torch.nn.CrossEntropyLoss()
optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate, weight_decay=weight_decay)

best_valid_acc = -1.0

for epoch in range(epochs):


    model.train()
    train_correct = 0
    train_total = 0
    train_loss_sum = 0.0

    for x, y in train_dl:
        x = x.to(device) 
        y = y.to(device) 

        optimizer.zero_grad()

        logits = model(x)          
        loss = loss_fn(logits, y)

        loss.backward()
        optimizer.step()

        train_loss_sum += loss.item() * x.size(0)
        preds = logits.argmax(dim=1)
        train_correct += (preds == y).sum().item()
        train_total += x.size(0)

    train_loss = train_loss_sum / max(1, train_total)
    train_acc = train_correct / max(1, train_total)


    model.eval()
    valid_correct = 0
    valid_total = 0
    valid_loss_sum = 0.0


    for x, y in valid_dl:
        x = x.to(device, non_blocking=True)
        y = y.to(device, non_blocking=True)

        logits = model(x)
        loss = loss_fn(logits, y)

        valid_loss_sum += loss.item() * x.size(0)
        preds = logits.argmax(dim=1)
        valid_correct += (preds == y).sum().item()
        valid_total += x.size(0)

    valid_loss = valid_loss_sum / max(1, valid_total)
    valid_acc = valid_correct / max(1, valid_total)

    print(f"[Epoch {epoch+1:03d}/{epochs}] "
          f"train_loss={train_loss:.4f} train_acc={train_acc:.4f} | "
          f"valid_loss={valid_loss:.4f} valid_acc={valid_acc:.4f}")

    if valid_acc > best_valid_acc:
        best_valid_acc = valid_acc

        torch.save(model.state_dict(), weight_path)
        print(f"Saved best weights (valid_acc={best_valid_acc:.4f}): {weight_path}")

        onnx_model = SoftmaxModel(backbone=model)
        torch.onnx.export(
            onnx_model,                 # 실행할 모델
            dummy_input,                # 모델 입력 예시
            onnx_model_path,            # 저장 파일명
            export_params=True,         # 모델 파일 안에 학습된 파라미터 저장
            opset_version=11,           # Bilinear 연산을 안정적으로 지원하는 버전
            do_constant_folding=True,   # 상수 폴딩 최적화 (속도 향상)
            input_names=['input'],      # 입력 노드 이름 (C++에서 호출 시 사용)
            output_names=['output'],    # 출력 노드 이름
        )

        quant_pre_process(input_model_path=onnx_model_path,
                          output_model_path=onnx_quant_model_path,
                          skip_symbolic_shape=True,
                          skip_onnx_shape=False,
                          skip_optimization=False)

print("Training done. Best valid_acc =", best_valid_acc)