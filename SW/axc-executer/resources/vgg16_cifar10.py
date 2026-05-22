############################################################
# Copyright 2022
# Author: David Cordero Chavarría <dcorderoch@ieee.org>
# Supervisor: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

"""
This module runs VGG16 on the CIFAR10 dataset, using Keras
"""

import json

import h5py

import matplotlib.pyplot as plt
import numpy as np

import tensorflow as tf

from tensorflow.keras import datasets, layers, models
from tensorflow.keras.applications.vgg16 import VGG16, preprocess_input
from tensorflow.keras.utils import to_categorical

# make all runs use the same random seed, to get deterministic results
tf.random.set_seed(1)


# def main() -> None:
(x_train, y_train), (x_test, y_test) = datasets.cifar10.load_data()
print(f"x_train.shape: {x_train.shape}")
print(f"x_test.shape: {x_test.shape}")

# x_ are images
# x[N] -> N is an index
# y_ are indices
# y[N] are indices of the classes list, with the label for the image

classes = [
    "airplane",
    "automobile",
    "bird",
    "cat",
    "deer",
    "dog",
    "frog",
    "horse",
    "ship",
    "truck",
]
print(f"classes: {classes}")

# switch from array(List[List[int]],dtype=uint8)
# to array(List[int],dtype=uint8)
# to simplify getting the text label for an image
print(f"y_train.shape: {y_train.shape}")
y_train = y_train.reshape(-1)
y_test = y_test.reshape(-1)
print(f"y_train.shape: {y_train.shape}")

NUM_CLASSES = 10  # CIFAR10 => 10 classes

train_labels = to_categorical(
    y_train,
    num_classes=NUM_CLASSES,
)
test_labels = to_categorical(
    y_test,
    num_classes=NUM_CLASSES,
)

print(f"train_labels: {train_labels}")
print(f"test_labels: {test_labels}")

train_ds = preprocess_input(x_train)
test_ds = preprocess_input(x_test)

BATCH_SIZE = 64 * 4
EPOCHS = 30 // 6
VALIDATION_SPLIT = 0.2

fname = './model_vgg16_cifar10.h5'

reconstructed_model = models.load_model(fname)

reconstructed_model.fit(
    train_ds,
    train_labels,
    epochs=EPOCHS,
    validation_split=VALIDATION_SPLIT,
    batch_size=BATCH_SIZE,
)

# use test_labels, since we used a reshape(-1)
score = reconstructed_model.evaluate(x_test, test_labels, verbose=0)

score_loss, score_accuracy = score

print(f"Test loss: {score[0]}")
print(f"Test accuracy: {score[1]}")

print(f"y_test[0]: {y_test[0]}")

weights = reconstructed_model.get_weights()

for i in range(len(weights)):
    print(f"{i}-th layer: {weights[i].shape}")

# 10 samples of the test data
SAMPLE_SLICE_SIZE = 10

_samples = y_test[:SAMPLE_SLICE_SIZE]
print(f"_samples: {_samples}")

_sample_imgs = x_test[:SAMPLE_SLICE_SIZE]

_sample_label_indices = y_test[:SAMPLE_SLICE_SIZE]
_sample_labels = [classes[s] for s in _samples]
print(f"_sample_label_indices: {_sample_label_indices}")
print(f"_sample_labels: {_sample_labels}")

# 10000, 32, 32, 3
print(f"x_test.shape:{x_test.shape}")
# 32, 32, 3
print(f"x_test[0].shape:{x_test[0].shape}")
# 32, 32
print(f"x_test[0][:,:,0].shape:{x_test[0][:,:,0].shape}")

# NOTE: this is for visual confirmation, in a CLI-only environment, can't use
# matplotlib to show a window, so a PNG file is saved instead
for i, zipped in enumerate(zip(_sample_imgs, _sample_labels)):
    img, label = zipped
    plt.imsave(
        f"r_img-{i}_label-{label}.png", img, pil_kwargs={"aspect": "equal"}
    )
    print(f"\nlabel: {label}")

    ym = reconstructed_model(np.array([img.reshape(-1, 32, 3)]))
    print(f"ym: {ym}")
    prediction = np.argmax(ym)
    print(f"prediction: {prediction}")
    predicted_label = classes[prediction]
    print(f"prediction label: {predicted_label}")

with h5py.File(fname, "r") as f:
    print(f"{f.keys()=}")
    a_group_key = list(f.keys())[-1]
    print(a_group_key)

    print(f"{f['model_weights'].keys()=}")
    # print(f"{f['model_weights']['dense_2']['dense_2']['bias:0'][:]=}")
    configs = json.loads(f.attrs["model_config"])
    print(f"{configs['config']['layers']=}")
