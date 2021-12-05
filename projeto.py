import numpy as np
import cv2
import matplotlib.pyplot as plt

def find_tables(img, min_table=1000, max_table=60000):
    """
    Detects tables from image using contours and minimum and maximum size for the contours.

    Args:
        img (numpy.ndarray): Image to detect tables on.
        min_table (int, optional): Minimum size for the table contours. Defaults to 1000.
        max_table (int, optional):  Maximum size for the table contours. Defaults to 60000.

    Returns:
        good_contours (list): List with all found contours inside permitted range.
    """  
    
    # Converting to grayscale, using gaussian blur and binary treshold
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(img_gray, (5, 5), 0)
    binary = cv2.threshold(blurred, 200, 255, cv2.THRESH_BINARY)

    # Obtaining all available contours from image
    contours, _ = cv2.findContours(binary[1], cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    good_contours = []

    # Storing only the contours inside permitted range.
    for contour in contours:
        area = cv2.contourArea(contour)
        if min_table < area < max_table:
            good_contours.append(contour)
            
    return good_contours


def center_of_table(image, contours):
    centers = []
    for i in contours:
        M = cv2.moments(i)
        if M['m00'] != 0:
            cx = int(M['m10']/M['m00'])
            cy = int(M['m01']/M['m00'])
            # cv2.drawContours(image, [i], -1, (0, 255, 0), 2)
            # cv2.circle(image, (cx, cy), 7, (0, 0, 255), -1)
            # cv2.putText(image, "center", (cx - 20, cy - 20),
            #         cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2)
        # print(f"x: {cx} y: {cy}")
        centers.append((cx, cy))
    
    return centers



def show_tables(img, contours):
    """
    Shows found table contours on top of image.

    Args:
        img (numpy.ndarray): Image to draw on top of.
        contours (list): Contours to be drawn on top of the image.
    """    
    img_contours = cv2.drawContours(img, contours, -1, (255, 0, 0), 10)
    plt.imshow(img_contours)
    plt.show()




if __name__ == '__main__': 
    img = cv2.imread('exemplo_mesas.png')
    img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    tables = find_tables(img)
    center_of_table(img, tables)
    show_tables(img, tables)
    print(center_of_table(img, tables))
    
    # plt.imshow(img_rgb)
    # plt.show()

