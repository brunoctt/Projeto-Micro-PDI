import math
import numpy as np

global MIN_DISTANCE
global MIN_ANGLE


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

    return min(
        distance_point_to_line(line1[:2], line2),
        distance_point_to_line(line1[2:], line2),
        distance_point_to_line(line2[:2], line1),
        distance_point_to_line(line2[2:], line1)
    )


def check_is_line_different(line_1, groups):
    for group in groups:
        for line_2 in group:
            if get_distance(line_2, line_1) < MIN_DISTANCE:
                orientation_1 = get_orientation(line_1)
                orientation_2 = get_orientation(line_2)
                if abs(orientation_1 - orientation_2) < MIN_ANGLE:
                    group.append(line_1)
                    return False
    return True


def merge_lines_into_groups(lines_list):
    groups = list()
    # first line will create new group every time
    groups.append([lines_list[0]])
    # if line is different from existing groups, create a new group
    for line_new in lines_list[1:]:
        if check_is_line_different(line_new, groups):
            groups.append([line_new])

    return groups


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


def process_lines(all_lines, min_dist=10, min_ang=10):
    lines_horizontal = []
    lines_vertical = []
    global MIN_DISTANCE
    global MIN_ANGLE
    MIN_DISTANCE = min_dist
    MIN_ANGLE = min_ang

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
            groups = merge_lines_into_groups(g)
            merged_lines = []
            for group in groups:
                merged_lines.append(merge_line_segments(group))
            merged_lines_all.extend(merged_lines)

    return merged_lines_all
