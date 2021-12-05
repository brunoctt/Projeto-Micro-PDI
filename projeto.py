import numpy as np
import cv2
import matplotlib.pyplot as plt
import tables


if __name__ == '__main__': 
    img = cv2.imread('exemplo_mesas.png')
    img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    table_contours = tables.find_tables(img)
    tables.show_tables(img, table_contours)
    print(tables.center_of_table(table_contours))
    
    # plt.imshow(img_rgb)
    # plt.show()

