from sys import stdin
if __name__ == '__main__':
    lines = []
    for line in stdin:
        lines.append(line.strip('\n'))
    lines.sort()
    for line in lines:
        print(line)

