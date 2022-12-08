#%%
import matplotlib.pyplot as plt
import numpy as np
import cv2
#%%

def show_image(img):
    plt.imshow(img, cmap="gray")
    plt.show()
    
def canny(image):
    gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    blur = cv2.GaussianBlur(gray, (5, 5), 0)
    canny = cv2.Canny(blur,  50, 150)
    return canny
#%%
img = cv2.imread('foto_mesa.jpeg')
cinza = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
borrado = cv2.GaussianBlur(cinza, (5, 5), 0)
binario = cv2.threshold(borrado, 40, 255, cv2.THRESH_BINARY_INV)
contornos, _ = cv2.findContours(binario[1], cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
img_contornos = img.copy()

img_contornos = cv2.drawContours(img_contornos, contornos, -1, (255, 0, 0), 10)
show_image(img_contornos)
#%%

# bordas = canny(img)
# show_image(bordas)

#%%
# img_limiar = cv2.threshold(img, 50, 255, cv2.THRESH_BINARY_INV)  # Linhas claras
# show_image(img_limiar[1])
# thresh = 50
# min_line = 5
# max_line = 100
# linhas = cv2.HoughLinesP(bordas, 1, np.pi / 180, thresh, minLineLength=min_line, maxLineGap=max_line)
# bordas_copia = cv2.cvtColor(bordas, cv2.COLOR_GRAY2BGR)
# for linha in linhas:
#     x1, y1, x2, y2 = linha[0] # Retorna apenas dois pontos
#     cv2.line(bordas_copia, (x1, y1), (x2, y2), (0, 0, 255), 3)
    
# show_image(bordas_copia)

# %%
