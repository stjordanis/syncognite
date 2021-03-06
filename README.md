# syncognite - A neural network library inspired by Stanford's CS231n course

[![Build Status](https://travis-ci.org/domschl/syncognite.svg?branch=master)](https://travis-ci.org/domschl/syncognite)
[![License](http://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)](LICENSE)

A neural network library for convolutional, fully connected nets and RNNs in C++.

This library implements some of the assignments from Stanfords's [CS231n](http://cs231n.stanford.edu/index.html) 2016 course by Andrej Karpathy, Fei-Fei Li, Justin Johnson and [CS224d](http://cs224d.stanford.edu/index.html) by Richard Socher as C++ framework.

Current state: **alpha**

### Features

* Fully connected networks
* Convolutional layers
* Recurrent nets (RNNs)
* Long-term short-term memory nets (LSTMs)
* ReLu, Sigmoid, TanH, and SELU<sup>(1)</sup> nonlinearities
* BatchNorm, SpatialBatchNorm, Dropout layers
* Softmax, SVM loss
* TemporalAffine and TemporalSoftmax layers for RNNs

[1]: "scaled exponential linear units" (SELUs), https://arxiv.org/abs/1706.02515

## Sample

### Model

Example: C++ definition of a deep convolutional net with batch-norm, dropout and fully connected layers:

```cpp
LayerBlock lb(R"({"name":"DomsNet","bench":false,"init":"orthonormal"})"_json);

lb.addLayer("Convolution", "cv1", R"({"inputShape":[1,28,28],"kernel":[48,5,5],"stride":1,"pad":2})",{"input"});
lb.addLayer("BatchNorm","sb1","{}",{"cv1"});
lb.addLayer("Relu","rl1","{}",{"sb1"});
lb.addLayer("Dropout","doc1",R"({"drop":0.8})",{"rl1"});
lb.addLayer("Convolution", "cv2", R"({"kernel":[48,3,3],"stride":1,"pad":1})",{"doc1"});
lb.addLayer("Relu","rl2","{}",{"cv2"});
lb.addLayer("Convolution", "cv3", R"({"kernel":[64,3,3],"stride":2,"pad":1})",{"rl2"});
lb.addLayer("BatchNorm","sb2","{}",{"cv3"});
lb.addLayer("Relu","rl3","{}",{"sb2"});
lb.addLayer("Dropout","doc2",R"({"drop":0.8})",{"rl3"});
lb.addLayer("Convolution", "cv4", R"({"kernel":[64,3,3],"stride":1,"pad":1})",{"doc2"});
lb.addLayer("Relu","rl4","{}",{"cv4"});
lb.addLayer("Convolution", "cv5", R"({"kernel":[128,3,3],"stride":2,"pad":1})",{"rl4"});
lb.addLayer("BatchNorm","sb3","{}",{"cv5"});
lb.addLayer("Relu","rl5","{}",{"sb3"});
lb.addLayer("Dropout","doc3",R"({"drop":0.8})",{"rl5"});
lb.addLayer("Convolution", "cv6", R"({"kernel":[128,3,3],"stride":1,"pad":1})",{"doc3"});
lb.addLayer("Relu","rl6","{}",{"cv6"});

lb.addLayer("Affine","af1",R"({"hidden":1024})",{"rl6"});
lb.addLayer("BatchNorm","bn1","{}",{"af1"});
lb.addLayer("Relu","rla1","{}",{"bn1"});
lb.addLayer("Dropout","do1",R"({"drop":0.7})",{"rla1"});
lb.addLayer("Affine","af2",R"({"hidden":512})",{"do1"});
lb.addLayer("BatchNorm","bn2","{}",{"af2"});
lb.addLayer("Relu","rla2","{}",{"bn2"});
lb.addLayer("Dropout","do2",R"({"drop":0.7})",{"rla2"});
lb.addLayer("Affine","af3",R"({"hidden":10})",{"do2"});
lb.addLayer("Softmax","sm1","{}",{"af3"});
```

### Training

```cpp
json jo(R"({"verbose":true,"shuffle":true,"lr_decay":0.95,"epsilon":1e-8})"_json);
jo["epochs"]=(floatN)40.0;
jo["batch_size"]=50;
jo["learning_rate"]=(floatN)5e-4;
jo["regularization"]=(floatN)1e-8;

lb.train(X, y, Xv, yv, "Adam", jo);

floatN train_err, val_err, test_err;
train_err=lb.test(X, y, jo.value("batch_size", 50));
val_err=lb.test(Xv, yv, jo.value("batch_size", 50));
test_err=lb.test(Xt, yt, jo.value("batch_size", 50));

cerr << "Final results on MNIST after " << jo.value("epochs",(floatN)0.0) << " epochs:" << endl;
cerr << "      Train-error: " << train_err << " train-acc: " << 1.0-train_err << endl;
cerr << " Validation-error: " << val_err <<   "   val-acc: " << 1.0-val_err << endl;
cerr << "       Test-error: " << test_err <<  "  test-acc: " << 1.0-test_err << endl;
```

see [mnisttest](cpmnist/) or [cifar10test](cpcifar10/) for complete examples.

### A model that generates text via LSTMs can be defined with:

```cpp
json j0;
string oName{"OH0"};
j0["inputShape"]=vector<int>{T};
j0["V"]=VS;
lb.addLayer("OneHot",oName,j0,{"input"});

int layer_depth=4;
string nName;
json j1;
j1["inputShape"]=vector<int>{VS,T};
j1["N"]=BS;
j1["H"]=H;
j1["forgetgateinitones"]=true;
j1["forgetbias"]=1.0;
j1["clip"]=clip;
for (auto l=0; l<layer_depth; l++) {
	nName="lstm"+std::to_string(l);
	lb.addLayer(rnntype,nName,j1,{oName});
	oName=nName;
}

json j11;
j11["inputShape"]=vector<int>{VS,T};
lb.addLayer("TemporalSoftmax","sm1",j11,{"af1"});
```

see [rnnreader](rnnreader/) for a complete example.

## Dependencies:

* C++ 11 compiler (on Linux (tested: clang, gcc, Intel icpc) or Mac (clang), ARM(gcc))
* CMake build system.
* [Hdf5](https://support.hdfgroup.org/HDF5/) [C++ API](https://support.hdfgroup.org/HDF5/doc/cpplus_RM/) for model saving and sample data, `hdf5` or `libhdf5-dev`.

### Optional dependencies:

* Cuda, OpenCL, ViennaCL (experimental, optional for BLAS speedups)

### External libraries that are included in the source tree:

* [Eigen](http://eigen.tuxfamily.org/) v3.3 `eigen3`, already (in default configuration) included in the source tree as submodule. Note: clang-4 requires current dev-branch [3.3.90] of eigen for tensor libs to compile.
* [nlohmann_json](https://github.com/nlohmann/json), already included in source tree (cpneural/nlohmann_json).

## Build

syncognite uses the CMake build system.

Clone the repository:

```bash
git clone git://github.com/domschl/syncognite
git submodule init
git submodule update    # This gets the in-tree Eigen3
```

Create a ```Build``` directory within the syncognite directory and configure the build:

```bash
# in sycognite/Build:
cmake ..
# optionally use ccmake to configure options and paths:
ccmake ..
```

Build the project:

```bash
make
```

## History

* 2018-03-02: Removed faulty RAN layer, switched to official eigen3 github-mirror at: [Github eigen3](https://github.com/eigenteam/eigen-git-mirror), fixes for eigen-dev stricted type-checking.

## Subprojects:

Things that should work:

* [testneural](cptest/) (cptest subproject, consistency tests for all layers using testdata and numerical differentials)
* [bench](bench/) (benchmark subproject, benchmarks for all layers)
* [mnisttest](cpmnist/) (cpmnist subproject, MNIST handwritten digit recognition with a convolutional network, requires [dataset download](datasets/).)
* [cifar10test](cpcifar10/) (cpcifar10 subproject, cifar10 image recognition with a convolutional network, requires [dataset download](datasets/).)
* [rnnreader](rnnreader/) (rnnreader subproject, text generation via RNN/LSTMs, similar to char-rnn.)
