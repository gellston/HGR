# jester_loader.py
import os
import glob
import random
import numpy as np
import pandas as pd
from PIL import Image

import torch
from torch.utils.data import Dataset

class JesterDataset(Dataset):
    """
    root/
      Train/<video_id>/*.jpg
      Validation/<video_id>/*.jpg
      Test/<video_id>/*.jpg
      Train.csv
      Validation.csv
      Test.csv

    CSV(네 파일 기준):
      Train/Validation: video_id,label,frames,label_id,shape,format
      Test:             id,label,frames,label_id,shape,format  (label/label_id 비어있음)

    반환:
      x: (C,T,H,W) float32, 0~1
      y: int (class index) 0~26, 라벨 없거나 unknown이면 -1
    """

    # ✅ 27개 클래스 고정 순서 (label_id 0~26에 대응되는 표준 순서로 사용)
    DEFAULT_CLASS_ORDER = [
        "Doing other things",
        "No gesture",
        "Drumming Fingers",
        "Pulling Hand In",
        "Pulling Two Fingers In",
        "Pushing Hand Away",
        "Pushing Two Fingers Away",
        "Rolling Hand Backward",
        "Rolling Hand Forward",
        "Shaking Hand",
        "Sliding Two Fingers Down",
        "Sliding Two Fingers Left",
        "Sliding Two Fingers Right",
        "Sliding Two Fingers Up",
        "Stop Sign",
        "Swiping Down",
        "Swiping Left",
        "Swiping Right",
        "Swiping Up",
        "Thumb Down",
        "Thumb Up",
        "Turning Hand Clockwise",
        "Turning Hand Counterclockwise",
        "Zooming In With Full Hand",
        "Zooming In With Two Fingers",
        "Zooming Out With Full Hand",
        "Zooming Out With Two Fingers",
    ]

    def __init__(
        self,
        root: str,
        split: str = "train",          # "train" | "validation" | "test"
        num_frames: int = 16,
        image_width: int = 128,
        image_height: int = 68,
        training: bool = True,
        return_id: bool = False,
        class_order=None,             # None이면 DEFAULT_CLASS_ORDER 사용, 리스트 주면 override
        unknown_label_value: int = -1,
    ):
        self.root = root
        self.split = split.lower()
        self.num_frames = int(num_frames)
        self.image_width = int(image_width)
        self.image_height = int(image_height)
        self.training = bool(training)
        self.return_id = bool(return_id)
        self.unknown_label_value = int(unknown_label_value)

        if self.split not in ["train", "validation", "test"]:
            raise ValueError("split must be one of: train | validation | test")

        csv_name = {"train": "Train.csv", "validation": "Validation.csv", "test": "Test.csv"}[self.split]
        folder_name = {"train": "Train", "validation": "Validation", "test": "Test"}[self.split]
        self.videos_dir = os.path.join(self.root, folder_name)

        csv_path = os.path.join(self.root, csv_name)
        if not os.path.isfile(csv_path):
            raise FileNotFoundError(f"CSV not found: {csv_path}")

        df = pd.read_csv(csv_path)

        id_col = "video_id" if "video_id" in df.columns else ("id" if "id" in df.columns else None)
        if id_col is None:
            raise ValueError(f"CSV must contain 'video_id' or 'id'. Got: {df.columns.tolist()}")

        # 샘플 id
        self.ids = df[id_col].astype(str).tolist()

        # 라벨 문자열(없으면 None)
        if "label" in df.columns:
            # NaN은 None 처리
            self.labels_text = df["label"].where(~df["label"].isna(), None).tolist()
        else:
            self.labels_text = [None] * len(self.ids)

        # ✅ 고정 class order / mapping
        self.idx_to_class = list(class_order) if class_order is not None else list(self.DEFAULT_CLASS_ORDER)
        self.class_to_idx = {name: i for i, name in enumerate(self.idx_to_class)}
        self.num_classes = len(self.idx_to_class)  # 항상 27

        # 라벨 인덱스 (label_text 기반, 없거나 모르면 -1)
        self.labels = [
            self.class_to_idx.get(lab, self.unknown_label_value) if lab is not None else self.unknown_label_value
            for lab in self.labels_text
        ]

        # 프레임 경로 캐시
        self._frame_cache = {}

    def __len__(self):
        return len(self.ids)

    def __getitem__(self, idx: int):
        vid = self.ids[idx]
        y = int(self.labels[idx])

        # 프레임 리스트 캐싱
        if vid in self._frame_cache:
            frame_paths = self._frame_cache[vid]
        else:
            vdir = os.path.join(self.videos_dir, vid)
            if not os.path.isdir(vdir):
                raise FileNotFoundError(f"Video folder not found: {vdir}")

            # jpg/jpeg 대소문자까지
            frame_paths = (
                glob.glob(os.path.join(vdir, "*.jpg")) +
                glob.glob(os.path.join(vdir, "*.JPG")) +
                glob.glob(os.path.join(vdir, "*.jpeg")) +
                glob.glob(os.path.join(vdir, "*.JPEG"))
            )
            if len(frame_paths) == 0:
                raise FileNotFoundError(f"No frames found in: {vdir}")

            # "00001.jpg" 같은 숫자 기준 정렬
            frame_paths.sort(key=lambda p: int(os.path.splitext(os.path.basename(p))[0]))
            self._frame_cache[vid] = frame_paths

        T = len(frame_paths)
        n = self.num_frames

        # 시간 샘플링: segment로 균등 분할 후
        # train이면 segment 내부 랜덤, eval이면 중앙
        if T >= n:
            seg = T / n
            indices = []
            for i in range(n):
                start = int(round(i * seg))
                end = int(round((i + 1) * seg)) - 1
                start = max(0, min(start, T - 1))
                end = max(0, min(end, T - 1))
                if end < start:
                    end = start
                if self.training and self.split == "train":
                    indices.append(random.randint(start, end))
                else:
                    indices.append((start + end) // 2)
        else:
            indices = list(range(T)) + [T - 1] * (n - T)

        # 프레임 로드 -> (C,T,H,W), float 0~1
        frames = []
        for fi in indices:
            img = Image.open(frame_paths[fi]).convert("RGB")
            img = img.resize((self.image_width, self.image_height), Image.BILINEAR)
            arr = np.asarray(img, dtype=np.float32) / 255.0  # HWC
            arr = arr.transpose(2, 0, 1)                    # CHW
            frames.append(torch.from_numpy(arr))

        x = torch.stack(frames, dim=0).permute(1, 0, 2, 3).contiguous()  # (C,T,H,W)

        if self.return_id:
            return x, y, vid
        return x, y