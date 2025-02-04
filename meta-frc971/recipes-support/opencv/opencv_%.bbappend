# Ignore Circular dependency path that causes a pseudo abort()
PSEUDO_IGNORE_PATHS:append = ",${WORKDIR}/contrib"

# Yocto doesn't like us(linking) when you don't include the example files for some reason
EXTRA_OECMAKE:append = " \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DWITH_TBB=ON \
    -DENABLE_FAST_MATH=1 \
    -DCUDA_FAST_MATH=1 \
    -DWITH_CUBLAS=1 \
    -DWITH_CUDA=ON \
    -DBUILD_opencv_cudacodec=OFF \
    -DWITH_CUDNN=ON \
    -DOPENCV_DNN_CUDA=ON \
    -DWITH_V4L=ON \
    -DCUDA_ARCH_BIN=8.7 \
    -DWITH_QT=OFF \
    -DWITH_GSTREAMER=ON \
    -DOPENCV_PC_FILE_NAME=opencv.pc \
    -DOPENCV_ENABLE_NONFREE=ON \
    -DINSTALL_PYTHON_EXAMPLES=OFF \
    -DINSTALL_C_EXAMPLES=ON \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_opencv_world=OFF \
    "

