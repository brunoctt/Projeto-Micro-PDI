import matplotlib.pyplot as plt
from sympy import Line, Segment
import numpy as np
import math
import cv2


X_AXIS = Line((0, 0), (10, 0))
Y_AXIS = Line((0, 0), (0, 10))


def show_image(image):
    plt.imshow(image, cmap="gray")
    plt.show()


def get_orientation(line):
    orientation = math.atan2(abs((line[3] - line[1])), abs((line[2] - line[0])))
    return math.degrees(orientation)


def distance_point_to_line(point, line):
    px, py = point
    x1, y1, x2, y2 = line

    def line_magnitude(x1_, y1_, x2_, y2_):
        line_mag = math.sqrt((x2_ - x1_) ** 2 + (y2_ - y1_) ** 2)
        return line_mag

    l_mag = line_magnitude(x1, y1, x2, y2)
    if l_mag < 0.00000001:
        dist = 9999
        return dist

    u1 = (((px - x1) * (x2 - x1)) + ((py - y1) * (y2 - y1)))
    u = u1 / (l_mag * l_mag)

    if (u < 0.00001) or (u > 1):
        # closest point does not fall within the line segment, take the shorter distance
        # to an endpoint
        ix = line_magnitude(px, py, x1, y1)
        iy = line_magnitude(px, py, x2, y2)
        if ix > iy:
            dist = iy
        else:
            dist = ix
    else:
        # Intersecting point is on the line, use the formula
        ix = x1 + u * (x2 - x1)
        iy = y1 + u * (y2 - y1)
        dist = line_magnitude(px, py, ix, iy)

    return dist


def get_distance(line1, line2):
    dist1 = distance_point_to_line(line1[:2], line2)
    dist2 = distance_point_to_line(line1[2:], line2)
    dist3 = distance_point_to_line(line2[:2], line1)
    dist4 = distance_point_to_line(line2[2:], line1)

    return min(dist1, dist2, dist3, dist4)


class HoughBundler:
    def __init__(self, min_distance=5, min_angle=2):
        self.min_distance = min_distance
        self.min_angle = min_angle

    def check_is_line_different(self, line_1, groups):
        for group in groups:
            for line_2 in group:
                if get_distance(line_2, line_1) < self.min_distance:
                    orientation_1 = get_orientation(line_1)
                    orientation_2 = get_orientation(line_2)
                    if abs(orientation_1 - orientation_2) < self.min_angle:
                        group.append(line_1)
                        return False
        return True

    def merge_lines_into_groups(self, lines_list):
        groups = list()
        # first line will create new group every time
        groups.append([lines_list[0]])
        # if line is different from existing groups, create a new group
        for line_new in lines_list[1:]:
            if self.check_is_line_different(line_new, groups):
                groups.append([line_new])

        return groups

    @staticmethod
    def merge_line_segments(lines):
        orientation = get_orientation(lines[0])

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

    def process_lines(self, all_lines):
        lines_horizontal = []
        lines_vertical = []

        for line in all_lines:
            line = line[0]
            line[::2].sort()
            line[1::2].sort()
            orientation = get_orientation(line)
            # Considering as vertical after passes 45 degrees
            if 45 < orientation <= 90:
                lines_vertical.append(line)
            else:
                lines_horizontal.append(line)

        lines_vertical = sorted(lines_vertical, key=lambda l: l[1])
        lines_horizontal = sorted(lines_horizontal, key=lambda l: l[0])
        merged_lines_all = []

        # for each cluster in vertical and horizontal lines leave only one line
        for g in [lines_horizontal, lines_vertical]:
            if len(g) > 0:
                groups = self.merge_lines_into_groups(g)
                merged_lines = []
                for group in groups:
                    merged_lines.append(self.merge_line_segments(group))
                merged_lines_all.extend(merged_lines)

        return np.asarray(merged_lines_all)


"""Reading Image"""
img = cv2.imread('foto_mesa.jpeg')
# show_image(img)

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
mask = np.zeros((1600, 901, 3), dtype=np.uint8)
contours_img = cv2.drawContours(mask, contours, -1, (255, 255, 255), 30)
# contours_img = cv2.dilate(mask, kernel, iterations=8)
# contours_img = cv2.erode(dilate, kernel, iterations=10)

"""Finding Hough Lines"""
thresh = 150
min_line = 50
max_line = 100
kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
eroded_binary = cv2.erode(binary, kernel, iterations=4)
hough_lines = cv2.HoughLinesP(eroded_binary, 1, np.pi / 180, thresh, minLineLength=min_line, maxLineGap=max_line)
hough_img = cv2.cvtColor(eroded_binary, cv2.COLOR_GRAY2BGR)
for hl in hough_lines:
    _x1, _y1, _x2, _y2 = hl[0]
    cv2.line(hough_img, (_x1, _y1), (_x2, _y2), (255, 0, 0), 3)

"""Merging Hough Lines and Contours"""
merged_img = cv2.bitwise_and(contours_img, hough_img)
merged_img = cv2.cvtColor(merged_img, cv2.COLOR_RGB2GRAY)
merged_img = cv2.erode(merged_img, kernel, iterations=3)

"""Hough Lines on merged_img"""
hough_lines = cv2.HoughLinesP(merged_img, 1, np.pi / 180, thresh, minLineLength=min_line, maxLineGap=max_line)
bundler = HoughBundler(min_distance=10, min_angle=10)
grouped_lines = bundler.process_lines(hough_lines)
print(len(grouped_lines))
kernel = np.ones((5, 5), np.uint8)
final_img = cv2.cvtColor(binary, cv2.COLOR_GRAY2BGR)

for gl in grouped_lines:
    _x1, _y1, _x2, _y2 = gl[0]
    cv2.line(final_img, (_x1, _y1), (_x2, _y2), (255, 0, 0), 5)

show_image(final_img)

intersections = []
for i in range(len(grouped_lines)):
    for j in range(len(grouped_lines)):
        if i == j:
            continue
        l_i, l_j = Line(grouped_lines[i][0][:2], grouped_lines[i][0][2:]), Line(grouped_lines[j][0][:2], grouped_lines[j][0][2:])
        if math.degrees(l_i.angle_between(l_j)) > 50:
            inter = l_i.intersection(l_j)
            if len(inter) == 0:
                continue
            inter = inter[0]
            if inter.distance(Segment(*l_i.points)) < 10 and inter.distance(Segment(*l_j.points)) < 10:
                if {i, j} not in intersections:
                    intersections.append({i, j})
                    print(l_i, l_j)
                    continue

l1 = grouped_lines[2][0]
l2 = grouped_lines[8][0]
sl1 = Line(l1[:2], l1[2:])
sl2 = Line(l2[:2], l2[2:])
p = sl2.intersection(sl1)[0]
