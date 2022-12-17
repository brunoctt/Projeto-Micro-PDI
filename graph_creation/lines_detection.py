import matplotlib.pyplot as plt
import numpy as np
import cv2


def show_image(image):
    plt.imshow(image, cmap="gray")
    plt.show()


def binary_thresh(image):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    blur = cv2.GaussianBlur(gray, (5, 5), 0)
    _, binary = cv2.threshold(blur, 40, 255, cv2.THRESH_BINARY_INV)
    return binary


def get_contours(binary_image):
    contours, _ = cv2.findContours(binary_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    # Since all lines are connected they will be in the same contour, so just need to get the biggest contour
    idx = np.argmax([c.size for c in contours])
    contours = contours[idx]  # [x, y]
    mask = np.zeros((1600, 901, 3), dtype=np.uint8)
    image_contours = cv2.drawContours(mask, contours, -1, (255, 255, 255), 30)
    # dilate = cv2.dilate(mask, kernel, iterations=8)
    # image_contours = cv2.erode(dilate, kernel, iterations=10)
    return image_contours


def hough_image(binary_image, kernel, thresh=150, min_line=50, max_line=100):
    eroded_binary = cv2.erode(binary_image, kernel, iterations=4)
    lines = cv2.HoughLinesP(eroded_binary, 1, np.pi / 180, thresh, minLineLength=min_line, maxLineGap=max_line)
    hough_img = cv2.cvtColor(eroded_binary, cv2.COLOR_GRAY2BGR)
    for hl in lines:
        x1, y1, x2, y2 = hl[0]
        cv2.line(hough_img, (x1, y1), (x2, y2), (255, 0, 0), 3)
    return hough_img


def merge_hough_contours(image_contours, image_hough, kernel_rect):
    image_merged = cv2.bitwise_and(image_contours, image_hough)
    image_merged = cv2.cvtColor(image_merged, cv2.COLOR_RGB2GRAY)
    image_merged = cv2.erode(image_merged, kernel_rect, iterations=3)
    return image_merged


def get_hough_lines_from_image(image, thresh=150, min_line=50, max_line=100):
    binary_img = binary_thresh(image)
    contours_img = get_contours(binary_img)
    kernel_rect = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
    hough = hough_image(binary_img, kernel_rect, thresh, min_line, max_line)
    merged_img = merge_hough_contours(contours_img, hough, kernel_rect)
    hough_lines = cv2.HoughLinesP(merged_img, 1, np.pi / 180, thresh, minLineLength=min_line, maxLineGap=max_line)
    return hough_lines
