import matplotlib.pyplot as plt
from copy import deepcopy
import numpy as np
import math
import cv2


def show_image(img):
    plt.imshow(img, cmap="gray")
    plt.show()


def canny(image):
    gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    blur = cv2.GaussianBlur(gray, (5, 5), 0)
    canny = cv2.Canny(blur,  50, 150)
    return canny


def biggest_distance(p1, p2):
    def euclidian_dist(p):
        return np.sqrt((p[0]-p[2])**2 + (p[1]-p[3])**2)
    final_point = deepcopy(p1)
    dist = euclidian_dist(p1)
    for ind, val in enumerate(p2):
        aux = deepcopy(final_point)
        aux[ind] = val
        new_dist = euclidian_dist(aux)
        if new_dist > dist:
            dist = new_dist
            final_point = aux

    final_point[::2].sort()
    final_point[1::2].sort()
    return final_point


import math


class HoughBundler:
    def __init__(self, min_distance=5, min_angle=2):
        self.min_distance = min_distance
        self.min_angle = min_angle

    def get_orientation(self, line):
        orientation = math.atan2(abs((line[3] - line[1])), abs((line[2] - line[0])))
        return math.degrees(orientation)

    def check_is_line_different(self, line_1, groups):
        for group in groups:
            for line_2 in group:
                if self.get_distance(line_2, line_1) < self.min_distance:
                    orientation_1 = self.get_orientation(line_1)
                    orientation_2 = self.get_orientation(line_2)
                    if abs(orientation_1 - orientation_2) < self.min_angle:
                        group.append(line_1)
                        return False
        return True

    def distance_point_to_line(self, point, line):
        px, py = point
        x1, y1, x2, y2 = line

        def line_magnitude(x1, y1, x2, y2):
            line_magnitude = math.sqrt(math.pow((x2 - x1), 2) + math.pow((y2 - y1), 2))
            return line_magnitude

        lmag = line_magnitude(x1, y1, x2, y2)
        if lmag < 0.00000001:
            distance_point_to_line = 9999
            return distance_point_to_line

        u1 = (((px - x1) * (x2 - x1)) + ((py - y1) * (y2 - y1)))
        u = u1 / (lmag * lmag)

        if (u < 0.00001) or (u > 1):
            # // closest point does not fall within the line segment, take the shorter distance
            # // to an endpoint
            ix = line_magnitude(px, py, x1, y1)
            iy = line_magnitude(px, py, x2, y2)
            if ix > iy:
                distance_point_to_line = iy
            else:
                distance_point_to_line = ix
        else:
            # Intersecting point is on the line, use the formula
            ix = x1 + u * (x2 - x1)
            iy = y1 + u * (y2 - y1)
            distance_point_to_line = line_magnitude(px, py, ix, iy)

        return distance_point_to_line

    def get_distance(self, a_line, b_line):
        dist1 = self.distance_point_to_line(a_line[:2], b_line)
        dist2 = self.distance_point_to_line(a_line[2:], b_line)
        dist3 = self.distance_point_to_line(b_line[:2], a_line)
        dist4 = self.distance_point_to_line(b_line[2:], a_line)

        return min(dist1, dist2, dist3, dist4)

    def merge_lines_into_groups(self, lines):
        groups = list()  ## all lines groups are here
        # first line will create new group every time
        groups.append([lines[0]])
        # if line is different from existing gropus, create a new group
        for line_new in lines[1:]:
            if self.check_is_line_different(line_new, groups):
                groups.append([line_new])

        return groups

    def merge_line_segments(self, lines):
        orientation = self.get_orientation(lines[0])

        if len(lines) == 1:
            return np.block([[lines[0][:2], lines[0][2:]]])

        points = []
        for line in lines:
            points.append(line[:2])
            points.append(line[2:])
        if 45 < orientation <= 90:
            # sort by y
            points = sorted(points, key=lambda point: point[1])
        else:
            # sort by x
            points = sorted(points, key=lambda point: point[0])

        return np.block([[points[0], points[-1]]])

    def process_lines(self, lines):
        lines_horizontal = []
        lines_vertical = []

        for line_i in [l[0] for l in lines]:
            orientation = self.get_orientation(line_i)
            # if vertical
            if 45 < orientation <= 90:
                lines_vertical.append(line_i)
            else:
                lines_horizontal.append(line_i)

        for _l in lines_vertical + lines_horizontal:
            _l[::2].sort()
            _l[1::2].sort()

        lines_vertical = sorted(lines_vertical, key=lambda line: line[1])
        lines_horizontal = sorted(lines_horizontal, key=lambda line: line[0])
        merged_lines_all = []

        # for each cluster in vertical and horizantal lines leave only one line
        for i in [lines_horizontal, lines_vertical]:
            if len(i) > 0:
                groups = self.merge_lines_into_groups(i)
                merged_lines = []
                for group in groups:
                    merged_lines.append(self.merge_line_segments(group))
                merged_lines_all.extend(merged_lines)

        return np.asarray(merged_lines_all)


"""Reading Image"""
img = cv2.imread('foto_mesa.jpeg')
show_image(img)

"""Finding Contours"""
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
blur = cv2.GaussianBlur(gray, (5, 5), 0)
_, binary = cv2.threshold(blur, 40, 255, cv2.THRESH_BINARY_INV)
contours, _ = cv2.findContours(binary, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
# Since all lines are connected they will be in the same contour, so just need to get the biggest contour
idx = np.argmax([c.size for c in contours])
contours = contours[idx]  # [x, y]
img_contours = cv2.drawContours(img.copy(), contours, -1, (255, 0, 0), 10)

"""Refining Contours"""
mask = np.zeros((1600, 901,3), dtype=np.uint8)
contours_img = cv2.drawContours(mask, contours, -1, (255, 255, 255), 30)
kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
# contours_img = cv2.dilate(mask, kernel, iterations=8)
# contours_img = cv2.erode(dilate, kernel, iterations=10)

"""Finding Hough Lines"""
thresh = 150
min_line = 50
max_line = 100
bordas = cv2.erode(binary, kernel, iterations=4)
linhas = cv2.HoughLinesP(bordas, 1, np.pi / 180, thresh, minLineLength=min_line, maxLineGap=max_line)
bordas_copia = cv2.cvtColor(bordas, cv2.COLOR_GRAY2BGR)
for linha in linhas:
    x1, y1, x2, y2 = linha[0] # Retorna apenas dois pontos
    cv2.line(bordas_copia, (x1, y1), (x2, y2), (255, 0, 0), 3)
    
"""Merging Hough Lines and Contours"""
merged = cv2.bitwise_and(contours_img, bordas_copia)
merged = cv2.cvtColor(merged, cv2.COLOR_RGB2GRAY)
merged = cv2.erode(merged, kernel, iterations=3)

"""Hough Lines on merged"""
lines = cv2.HoughLinesP(merged, 1, np.pi / 180, thresh, minLineLength=min_line, maxLineGap=max_line)
bundler = HoughBundler(min_distance=10, min_angle=10)
lines = bundler.process_lines(lines)
print(len(lines))
kernel = np.ones((5, 5), np.uint8)
bordas_copia = cv2.cvtColor(binary, cv2.COLOR_GRAY2BGR)

for linha in lines:
    x1, y1, x2, y2 = linha[0]
    cv2.line(bordas_copia, (x1, y1), (x2, y2), (255, 0, 0), 5)

show_image(bordas_copia)
