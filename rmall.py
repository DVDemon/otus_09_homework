import os

with open('output.txt', 'r') as file:
    data = file.read()


for f in data.split('\n'):
    if os.path.exists(f):
        print(f)
        os.remove(f)
    else:
        print(f'File not found: {f}')