import numpy as np
import cv2
import matplotlib.pyplot as plt
import tables
import json



    

def search_cards(img, centers, status, y_range=100, x_range=150, step=2):
    """
    Searches for the status cards on each table.

    Args:
        img: Image to search.
        centers: Center coordinates of each table.
        status: Card status of each table.
        y_range: Range to search from central point on y axis. Defaults to 100.
        x_range: Range to search from central point on x axis. Defaults to 150.
        step: Steps of range to search. Defaults to 2.

    Returns:
        status: New status of each card.
    """    
    for table in centers.items():
        for i in range(table[1]['y_center'] - y_range, 
                       table[1]['y_center'] + y_range, step):
            for j in range(table[1]['x_center'] - x_range, 
                           table[1]['x_center'] + x_range, step):
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

    
    img = cv2.imread('exemplo_mesas3.png')
    
    # contours = tables.find_tables(img)
    # centers = tables.center_of_table(contours)
    # tables.show_tables(img, contours)
    
    # img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    # status = search_cards(img_hsv, centers, status)
    # print(status)
    
    video = cv2.VideoCapture(0)
  
    while(True):
        
        _, frame = video.read()
    
        # Displaying footage
        cv2.imshow('Webcam', frame)
        
        # Need to set up tables
        # contours = tables.find_tables(frame, min_table=100)
        # centers = tables.center_of_table(contours)
        # tables.show_tables(frame, contours)
        
        # Press 'q' to quit
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    # After the loop release the cap object
    video.release()
    # Destroy all the windows
    cv2.destroyAllWindows()
