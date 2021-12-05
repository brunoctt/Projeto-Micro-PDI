# a = list(range(21))
a = [(0, 0), (1, 0), (0, 1), (1, 1), (1, 2), (2, 0), (0, 2)]
        
print(a)

for i, val in enumerate(a):
    # if i < 10:
    #     del a(i)
    a.remove(val)
        
print(a)