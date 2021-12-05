import numpy as np
import cv2
import matplotlib.pyplot as plt

def find_tables(img, min_table=1000, max_table=60000):
    """
    Detects tables from image using contours and minimum and maximum size for the contours.

    Args:
        img ([type]): [description]
        min_table (int, optional): [description]. Defaults to 1000.
        max_table (int, optional): [description]. Defaults to 60000.

    Returns:
        [type]: [description]
    """  
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(img_gray, (5, 5), 0)
    binary = cv2.threshold(blurred, 200, 255, cv2.THRESH_BINARY)

    contours, _ = cv2.findContours(binary[1], cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    good_contours = []

    for contour in contours:
        area = cv2.contourArea(contour)
        if min_table < area < max_table:
            good_contours.append(contour)
            # print(area)
            
    return good_contours


def show_tables(img, contours):
    img_contours = img.copy()
    img_contours = cv2.drawContours(img_contours, contours, -1, (255, 0, 0), 10)
    plt.imshow(img_contours)
    plt.show()

        

img = cv2.imread('exemplo_mesas.png')
img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
tables = find_tables(img)
show_tables(img, tables)
# plt.imshow(img_rgb)
# plt.show()

