
class Point:
    def __init__(self):
        self.x = 1
        self.y = 1

points_box = [Point() for _ in range(0, 10)]
to_remove_box = [None for _ in range(0, 10)]

to_remove_box[2] = points_box[2]
print(to_remove_box[2])
# to_remove_box[2] = None
points_box.remove(points_box[2])
print(to_remove_box[2])

