import numpy as np
import cv2
import matplotlib.pyplot as plt
import tables
import json



    

def search_cards(img, centers, status, y_range=80, x_range=80, step=2):
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
        
        # Stores all results obtained for the table
        all_status = []
        # print(table)
        # print('-------------')
        for i in range(max(table[1]['y_center'] - y_range, 0), 
                       min(table[1]['y_center'] + y_range, img.shape[0]), step):
            for j in range(max(table[1]['x_center'] - x_range, 0), 
                           min(table[1]['x_center'] + x_range, img.shape[1]), step):
                # if img[i, j, 1] > 120 and img[i, j, 2] > 140:
                if img[i, j, 0] >= 175 :  # Red
                    all_status.append('red')
                    # print(table[0], i, j, img[i, j, 0], 'Red')
                    break
                elif 70 <= img[i, j, 0] <= 90:  # Green
                    all_status.append('green')
                    # print(table[0], i, j, img[i, j, 0], 'Green')
                    break 
        
        if all_status:
            # Gets the result that occured the most 
            status[table[0]] = max(set(all_status), key=all_status.count)
                
    return status


if __name__ == '__main__': 
    
    # Loading tables data
    with open('test_centers.json') as file:
        centers = json.load(file)
        
    # Creating status for each table and setting card color to red
    status = {mesa: None for mesa in centers.keys()}    

    
    """img = cv2.imread('test_cards.jpeg')
    
    # contours = tables.find_tables(img, 0, 10000000)
    # centers = tables.center_of_table(contours)
    # tables.show_tables(img, contours)
    
    img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    # plt.imshow(img_hsv)
    # plt.show()
    status = search_cards(img_hsv, centers, status)
    print(status)"""
    
    

    video = cv2.VideoCapture(0)
    # Setting higher resolution
    video.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
    video.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
  
    while(True):
        
        _, frame = video.read()
    
        # Displaying footage
        cv2.imshow('Webcam', frame)
        
        # Need to set up tables
        # contours = tables.find_tables(frame, 0, 10000000)
        # centers = tables.center_of_table(contours)
        # tables.show_tables(frame, contours)
        
        img_hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)        
        status = search_cards(img_hsv, centers, status, step=3)       
        plt.imshow(img_hsv)
        plt.show()
        print(status)

        
        # Press 'q' to quit
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    # After the loop release the cap object
    video.release()

    
    # Destroy all the windows
    cv2.destroyAllWindows()
