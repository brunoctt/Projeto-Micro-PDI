from graph_creation.lines_detection import get_hough_lines_from_image, binary_thresh
from graph_creation.graph import Graph, find_intersections, plot_intersections
from graph_creation.group_lines import HoughBundler
from serial import Serial
from asyncio import sleep
import numpy as np
import cv2





if __name__ == '__main__':
    img = cv2.imread('foto_mesa.jpeg')
    binary = binary_thresh(img)
    hough_lines = get_hough_lines_from_image(img)
    bundler = HoughBundler(min_distance=10, min_angle=10)
    processed_lines = bundler.process_lines(hough_lines)
    final_img = cv2.cvtColor(binary, cv2.COLOR_GRAY2BGR)
    for gl in processed_lines:
        _x1, _y1, _x2, _y2 = gl[0]
        cv2.line(final_img, (_x1, _y1), (_x2, _y2), (255, 0, 0), 5)
    inter = find_intersections(processed_lines)
    g = Graph(processed_lines, inter)
    mat = g.build_adjacency_matrix()

    # plot_intersections(final_img, g.destination_nodes+g.auxiliary_nodes, save=True)

    ser = Serial('COM3', 9600)
    ser.timeout = 60
    if not ser.is_open:
        ser.open()

    def read_serial():
        r = ser.readline()
        print(r)

    print("Connected")
    res = ser.readline()
    ser.timeout = 7
    while True:
        if len(res) < 1:
            break
        else:
            print(res)
        res = ser.readline()
    ser.write(str.encode(f'{len(g.destination_nodes)}'))
    print(len(g.destination_nodes))
    print(ser.readline())
    ser.write(str.encode(f'{len(g.auxiliary_nodes)}'))
    print(len(g.auxiliary_nodes))
    print(ser.readline())
    print(ser.readline())
    first = True
    for i in range(mat.shape[0]):
        for j in range(mat.shape[0]):
            if not isinstance(mat[i][j], str):
                continue
            read_serial()
            if not first:
                ser.write(str.encode(f'y'))
                print('y')
                read_serial()
            else:
                first = False
            # sleep(1)
            ser.write(str.encode(f'{i} {j} {mat[i][j]}'))
            print(f'{i} {j} {mat[i][j]}')
            read_serial()

    read_serial()
    ser.write(str.encode(f'n'))
    print('n')

    while True:
        res = ser.readline()
        if len(res) > 1:
            print(ser.readline())
        else:
            break

    ser.write(str.encode(f'0'))
    print('0')

    # ser.timeout = 20

    while True:

        while True:
            res = ser.readline()
            if len(res) > 1:
                print(ser.readline())
            else:
                break

        dest = input("Enter destination (q to quit)\n")
        if dest == 'q':
            break
        ser.write(str.encode(f'{dest}'))
        print(dest)
