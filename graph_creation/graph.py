from graph_creation.lines_detection import get_hough_lines_from_image, binary_thresh, show_image
from graph_creation.group_lines import HoughBundler
from sympy import Line, Segment
from math import degrees
import cv2


def find_intersections(grouped_lines):
    intersections = []
    for i in range(len(grouped_lines)):
        for j in range(len(grouped_lines)):
            if i == j:
                continue
            l_i, l_j = Line(grouped_lines[i][0][:2], grouped_lines[i][0][2:]), Line(grouped_lines[j][0][:2], grouped_lines[j][0][2:])
            if degrees(l_i.angle_between(l_j)) > 50:
                _inter = l_i.intersection(l_j)
                if len(_inter) == 0:
                    continue
                _inter = _inter[0]
                if _inter.distance(Segment(*l_i.points)) < 10 and _inter.distance(Segment(*l_j.points)) < 10:
                    if {i, j} not in intersections:
                        intersections.append({i, j})
                        # print(l_i, l_j)
                        continue
    return intersections


if __name__ == '__main__':
    """Reading Image"""
    img = cv2.imread('foto_mesa.jpeg')
    binary = binary_thresh(img)
    hough_lines = get_hough_lines_from_image(img)
    bundler = HoughBundler(min_distance=10, min_angle=10)
    processed_lines = bundler.process_lines(hough_lines)
    print(len(processed_lines))
    # kernel = np.ones((5, 5), np.uint8)
    final_img = cv2.cvtColor(binary, cv2.COLOR_GRAY2BGR)

    for gl in processed_lines:
        _x1, _y1, _x2, _y2 = gl[0]
        cv2.line(final_img, (_x1, _y1), (_x2, _y2), (255, 0, 0), 5)

    show_image(final_img)

    inter = find_intersections(processed_lines)
    print(inter)
