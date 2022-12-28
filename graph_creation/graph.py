import numpy as np
from graph_creation.lines_detection import get_hough_lines_from_image, binary_thresh, show_image
from graph_creation.group_lines import HoughBundler
from sympy import Line, Segment, Point2D
from math import degrees
import cv2


class Node:
    _id_generator = (i for i in range(1000))

    def __init__(self, coordinates: Point2D):
        self.id = next(Node._id_generator)
        self.coordinates = coordinates
        self.north = self.south = self.east = self.west = None

    def __repr__(self):
        return f"Node {self.id}"

    @property
    def connected_to(self):
        return {
            'north': self.north,
            'south': self.south,
            'east': self.east,
            'west': self.west
        }

    @property
    def amount_of_connections(self):
        return sum(v is not None for v in self.connected_to.values())

    # noinspection PyTypeChecker
    def coordinate_to_point(self, other):
        segment = Segment(self.coordinates, other).direction.evalf()
        direction_val = max(segment.coordinates, key=abs)
        if direction_val == segment.x:
            if direction_val < 0:
                return 'west'
            return 'east'
        if direction_val > 0:
            return 'south'
        return 'north'


class Graph:
    """
    Class representing a graph where:
        nodes are destination points (connected to only 1 point);
        auxiliary_nodes are points where there are more than one path option (connected to more than 2 points);
        auxiliary_points are point with only one path option (connected to only 2 points);
        lines are the paths connecting every point
    """
    def __init__(self, all_lines: list[Line], intersections: dict[tuple[int, int], Point2D]):
        self._lines = all_lines
        self._intersections = intersections
        self.auxiliary_nodes = []
        self.auxiliary_points = []
        self.destination_nodes = []

        # Create intersection points
        temp_nodes = [Node(_p) for _p in self._intersections.values()]

        # Constructing graph
        self.build_destination_nodes(temp_nodes)
        self.connect_temporary_nodes(temp_nodes)
        self.assign_temporary_nodes(temp_nodes)

    def __repr__(self):
        return f"Graph with {len(self.destination_nodes)} destination nodes"

    @staticmethod
    def find_node_by_coordinate(coord, nodes_list):
        return next(n for n in nodes_list if n.coordinates.coordinates == coord.coordinates)

    def build_destination_nodes(self, temporary_nodes):
        def find_closest_point(ref_point, ref_line):
            dists = {ref_point.distance(_p): _p for _idx, _p in self._intersections.items() if self._lines.index(ref_line) in _idx}
            argmin = min(dists.keys())
            return dists[argmin]

        for line in self._lines:
            possible_points = list(line.copy().points)
            for idx, inter_point in self._intersections.items():
                if self._lines.index(line) not in idx:
                    continue
                d = np.array([inter_point.distance(_p) for _p in possible_points]) < 30
                node = np.where(d == True)[0]
                if len(node) == 0:
                    continue
                del possible_points[node[0]]
                if len(possible_points) == 0:
                    break
            if len(possible_points) > 0:
                for point in possible_points:
                    _n = Node(point)
                    closest = find_closest_point(point, line)
                    c2p = _n.coordinate_to_point(closest)
                    closest = self.find_node_by_coordinate(closest, temporary_nodes)
                    setattr(_n, c2p, closest)
                    setattr(closest, invert_coordinates(c2p), _n)
                    self.destination_nodes.append(_n)

    def connect_temporary_nodes(self, temporary_nodes):
        for idx, point in self._intersections.items():
            _node = self.find_node_by_coordinate(point, temporary_nodes)
            for _idx, _point in self._intersections.items():
                if idx == _idx or not any(i in _idx for i in idx):
                    continue
                direction = _node.coordinate_to_point(_point)
                val = getattr(_node, direction, None)
                if val is not None and _node.coordinates.distance(val.coordinates) < _node.coordinates.distance(_point):
                    continue
                setattr(_node, direction, self.find_node_by_coordinate(_point, temporary_nodes))

    def assign_temporary_nodes(self, temp_nodes):
        for node in temp_nodes:
            if node.amount_of_connections == 1:
                raise AttributeError(f"{node} has 1 connection and was not considered a destination node")
            elif node.amount_of_connections == 2:
                self.auxiliary_points.append(node)
            else:
                self.auxiliary_nodes.append(node)

        # Reordering nodes so destination nodes have lower id's
        i = 0
        for node in self.destination_nodes + self.auxiliary_nodes + self.auxiliary_points:
            node.id = i
            i += 1

    def build_adjacency_matrix(self):
        def connect_aux_point(dest_point, connected_point, _direction, _destination):
            for _d, _n in connected_point.connected_to.items():
                if _n is None or _n == dest_point:
                    continue
                _direction += _d[0].upper()
                _destination = _n.id
                if _n in self.auxiliary_points:
                    _direction, _destination = connect_aux_point(connected_point, _n, _direction, _destination)
            return _direction, _destination

        m_size = min(n.id for n in self.auxiliary_points)
        matrix = np.full((m_size, m_size), np.nan, dtype=object)
        for node in self.destination_nodes + self.auxiliary_nodes:
            for d, n in node.connected_to.items():
                if n is None:
                    continue
                direction = d[0].upper()
                dest = n.id
                # If node is connected to auxiliary point, add direction to other point and extra turn
                if n in self.auxiliary_points:
                    direction, dest = connect_aux_point(node, n, direction, dest)
                matrix[node.id, dest] = direction

        return matrix


def find_intersections(grouped_lines):
    intersections = {}
    for i in range(len(grouped_lines)):
        for j in range(len(grouped_lines)):
            if i == j:
                continue
            if type(grouped_lines[i]) == np.ndarray:
                _l = Line(grouped_lines[i][0][:2], grouped_lines[i][0][2:])
                grouped_lines[i] = _l
            if type(grouped_lines[j]) == np.ndarray:
                grouped_lines[j] = Line(grouped_lines[j][0][:2], grouped_lines[j][0][2:])

            l_i, l_j = grouped_lines[i], grouped_lines[j]
            if degrees(l_i.angle_between(l_j)) > 50:
                _inter = l_i.intersection(l_j)
                if len(_inter) == 0:
                    continue
                _inter = _inter[0]
                if _inter.distance(Segment(*l_i.points)) < 10 and _inter.distance(Segment(*l_j.points)) < 10:
                    if {i, j} not in [set(k) for k in intersections.keys()]:
                        intersections[(i, j)] = _inter
                        continue
    return intersections


def invert_coordinates(coord: str):
    if coord.lower() == 'east':
        return 'west'
    elif coord.lower() == 'west':
        return 'east'
    elif coord.lower() == 'north':
        return 'south'
    elif coord.lower() == 'south':
        return 'north'
    else:
        raise ValueError(f"Invalid input {coord}")


def plot_intersections(in_image, iterable):
    # Plot image with intersections
    font = cv2.FONT_HERSHEY_SIMPLEX
    font_scale = 1.3
    font_color = (50, 170, 50)
    thickness = 5
    line_type = 2
    res_img = in_image.copy()
    for v in iterable:
        vals = [int(round(x)) for x in v.coordinates.evalf().args]
        text = v.__repr__()
        res_img = cv2.circle(res_img, vals, radius=0, color=(0, 0, 255), thickness=35)
        cv2.putText(res_img, text,
                    vals,
                    font,
                    font_scale,
                    font_color,
                    thickness,
                    line_type)
    show_image(res_img)


if __name__ == '__main__':
    """Reading Image"""
    img = cv2.imread('foto_mesa.jpeg')
    binary = binary_thresh(img)
    hough_lines = get_hough_lines_from_image(img)
    bundler = HoughBundler(min_distance=10, min_angle=10)
    processed_lines = bundler.process_lines(hough_lines)
    print(f"{len(processed_lines)} lines")
    # kernel = np.ones((5, 5), np.uint8)
    final_img = cv2.cvtColor(binary, cv2.COLOR_GRAY2BGR)

    for gl in processed_lines:
        _x1, _y1, _x2, _y2 = gl[0]
        cv2.line(final_img, (_x1, _y1), (_x2, _y2), (255, 0, 0), 5)

    # show_image(final_img)

    inter = find_intersections(processed_lines)
    g = Graph(processed_lines, inter)
    mat = g.build_adjacency_matrix()
    plot_intersections(final_img, g.destination_nodes)
    print(f"{len(inter)} intersections:")
    for p in inter.values():
        if not any(Segment(*pl.points).contains(p) for pl in processed_lines):
            raise ValueError(f"Point {p.evalf()} not contained in any line in processed_lines")
        # print(p.evalf())
