import numpy as np
import cv2
# import matplotlib.pyplot as plt
import json
import serial



    

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
        green_status: Name of the table with green card, or None.
        # green_queue: Queue with all tables having a green card.
    """    
    
    green_queue = []
    
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
                    break
                elif 70 <= img[i, j, 0] <= 90:  # Green
                    all_status.append('green')
                    break 
        
        if all_status:
            # Gets the result that occured the most 
            status[table[0]] = max(set(all_status), key=all_status.count)
            # Not using green_queue for now
            # # If result is green, try to add to queue
            # if status[table[0]] == 'green':
            #     # Verifying if not already in queue
            #     if table[0] not in green_queue:
            #         green_queue.insert(0, table[0])
                    
            # # In case of red, will remove from queue
            # elif status[table[0]] == 'red':
            #     # Verifying if present in queue
            #     if table[0] in green_queue:
            #         green_queue.remove(table[0])
            
    green_status = [key for key, value in status.items() if value == 'green']
    
    if len(green_status) == 0:
        return status, '0'
    elif len(green_status) == 1:
        return status, green_status[0][5]
    
                
    print('There should be either 0 or 1 table with green signal.')
    return status, green_status[0][5]


if __name__ == '__main__': 
    
    # Loading tables data
    with open('test_centers.json') as file:
        centers = json.load(file)
        
    # Creating status for each table and setting card color to red
    status = {mesa: None for mesa in centers.keys()}    
    
    # # Queue of all tables with green status
    # queue = []      

    video = cv2.VideoCapture(0)
    # Setting higher resolution
    video.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
    video.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
    
    ser = serial.Serial('COM7', 9600)  # open serial port
    if not ser.is_open:
        ser.open()
        
    previous = '0'
  
    while(True):
        
        _, frame = video.read()
    
        # Displaying footage
        cv2.imshow('Webcam', frame)
        
        # Need to set up tables
        # contours = tables.find_tables(frame, 0, 10000000)
        # centers = tables.center_of_table(contours)
        # tables.show_tables(frame, contours)
        
        img_hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)        
        status, queue = search_cards(img_hsv, centers, status, step=3)       
        print(status)
        print(queue)
        
        if queue != previous:
            print('send')
            ser.write(str.encode(queue)) 
            result = None    
            while result != "Arrived":
                result = ser.readline()
                print(result)
            
        previous =  queue

        # Press 'q' to quit
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    # After the loop release the cap object
    video.release()

    
    # Destroy all the windows
    cv2.destroyAllWindows()
    
    ser.close()
