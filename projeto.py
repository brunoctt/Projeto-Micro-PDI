import numpy as np
import cv2
import matplotlib.pyplot as plt
import tables
import json



    

def search_cards(img, centers, status, search_interval=100):
    """
    Searches for the status cards on each table.

    Args:
        img: Image to search.
        centers: Center coordinates of each table.
        status: Card status of each table.

    Returns:
        status: New status of each card.
    """    
    for table in centers.items():
        for i in range(table[1]['y_center'] - search_interval, 
                       table[1]['y_center'] + search_interval):
            for j in range(table[1]['x_center'] - search_interval, 
                           table[1]['x_center'] + search_interval):
                if img[i, j, 1] > 140 and img[i, j, 2] > 150:
                    if img[i, j, 0] == 0:  # Red
                        status[table[0]] = 'red'
                        break
                    if img[i, j, 0] == 60:  # Green
                        status[table[0]] = 'green'
                        break 
                
    return status


if __name__ == '__main__': 
    
    # Loading tables data
    with open('table_centers.json') as file:
        centers = json.load(file)
        
    # Creating status for each table and setting card color to red
    status = {mesa: 'red' for mesa in centers.keys()}    

    
    img = cv2.imread('exemplo_mesas1.png')
    # img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    # status = search_cards(img_hsv, centers, status)
    # print(status)
    
    contours = tables.find_tables(img)
    centers = tables.center_of_table(contours)
    tables.show_tables(img, contours)
