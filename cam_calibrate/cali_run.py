import cv2
import numpy as np
import glob
import pickle


calidata = np.load('calibrate.npz')
dist = calidata['dist']
mtx = calidata['mtx']
K = calidata['K']
D = calidata['D']
w = 640
h = 640
p = cv2.fisheye.estimateNewCameraMatrixForUndistortRectify(K,D,(w,h),None)
mapx2,mapy2 = cv2.fisheye.initUndistortRectifyMap(K,D,None,p,(w,h),cv2.CV_32F)
def undistort(rawimg):
    frame_rectified = cv2.remap(rawimg,mapx2,mapy2,interpolation=cv2.INTER_LINEAR,borderMode=cv2.BORDER_CONSTANT)
    return frame_rectified

if __name__ == "__main__":
    images = glob.glob("*.jpg")
    for fname in images:
        img = cv2.imread(fname)
        dst = undistort(img)
        print(dst.shape)
        cv2.imshow('undistort',dst)
        cv2.imshow('org',img)
        cv2.waitKey(5000)