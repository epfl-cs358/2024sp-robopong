import cv2
import numpy as np

aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)

marker_id = 1 # change the marker id to generate 1, 2, 3, and 4
marker_size = 200
marker_image = np.zeros((marker_size, marker_size), dtype=np.uint8)
marker_image = cv2.aruco.generateImageMarker(aruco_dict, marker_id, marker_size, marker_image, 1)

cv2.imwrite("marker.png", marker_image)
