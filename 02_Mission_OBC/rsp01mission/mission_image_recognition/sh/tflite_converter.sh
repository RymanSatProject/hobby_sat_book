#!/usr/bin/env bash

for h5_file_path in ./ml/models/*.h5; do
    tflite_filename=`basename $h5_file_path .h5`
    tflite_file_path="./ml/models/${tflite_filename}.tflite"
    tflite_convert --keras_model_file=$h5_file_path --output_file=$tflite_file_path
done
