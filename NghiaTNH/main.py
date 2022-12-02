from typing import *
import numpy as np
import matplotlib.pyplot as plt
from time import time

class DilateSolver():

    def __init__(self, input: np.ndarray, K: int) -> None:
        self.dirs = [
                        [-1,-1],[0,-1],[1,-1],
                        [-1, 0],       [1, 0],
                        [-1, 1],[0, 1],[1, 1]
                    ]
            # dirs = [[0,-1],[-1,0],[1,0],[0,1]]
        
        self.input = input
        self.M = input.shape[0]
        self.N = input.shape[1]
        self.K = K

    def set_size(self, M: int, N: int):
        self.M = M
        self.N = N


    def is_valid(self, x: int, y: int) -> bool:
        return 0 <= x <= self.M - 1 and 0 <= y <= self.N - 1

    def get_position(self, pos1d: int) -> Tuple[int, int]:
        return [pos1d // self.N, pos1d % self.N]

    def to_position_from_list(self, pos2d: List[int]) -> int:
        return pos2d[0] * self.N + pos2d[1]

    def to_position(self, x: int, y: int) -> int:
        return x * self.N + y

    def search_areas(self, input: List[List[int]]) -> List[List[int]]:
        areas = dict()
        checked = [[False for x in range(self.N)] for y in range(self.M)]
        for i in range(self.M):
            for j in range(self.N):
                if input[i][j] != 0 and not checked[i][j]:
                    check_points = [(i, j)]
                    points = []
                    while check_points:
                        (x, y) = check_points.pop()
                        if not self.is_valid(x, y) or checked[x][y]: continue
                        if input[x][y] == 0:
                            checked[x][y] = True
                            continue

                        if input[x][y] == input[i][j]:
                            checked[x][y] = True
                            count = 0
                            for dir in self.dirs:
                                if self.is_valid(x + dir[0], y + dir[1]):
                                    if input[x+ dir[0]][y + dir[1]] == 0:
                                        break
                                    else: count += 1

                            if count < len(self.dirs):
                                points.append(self.to_position(x, y))
                                for dir in self.dirs:
                                    check_points.append((x + dir[0], y + dir[1]))

                    areas[self.to_position(i, j)] = points

        return areas

    def add_candidate(self, area: int, point: int, candidates: dict, removed_candidates: Set[int], input: List[List[int]]):
        position = self.get_position(point)
        for dir in self.dirs:
            if self.is_valid(position[0] + dir[0], position[1] + dir[1]):
                candidate_point = self.to_position(position[0] + dir[0], position[1] + dir[1])
                if input[position[0] + dir[0]][position[1] + dir[1]] != 0: continue
                if (candidate_point in removed_candidates): continue
                if candidates.get(candidate_point, area) == area:
                    candidates[candidate_point] = area
                else:
                    removed_candidates.add(candidate_point)
                    candidates.pop(candidate_point)

    def add_candidates(self, areas: List[List[int]], input: List[List[int]], removed_candidates: Set[int]) -> List[List[int]]:
        candidates = dict()
        for area in areas:
            for point in areas[area]:
                self.add_candidate(area, point, candidates, removed_candidates, input)

        return candidates

    def dilate(self) -> List[List[int]]:
        output = [[self.input[i][j] for j in range(self.N)] for i in range(self.M)]
        removed_candidates = set()
        t1 = time()
        areas = self.search_areas(output)
        t2 = time()
        print("len areas = {}".format(np.sum([len(areas[x]) for x in areas])))
        print("Find areas time = {}".format((t2 - t1)*1e6))
        for k in range(self.K):
            t1 = time()
            candidates = self.add_candidates(areas, output, removed_candidates)
            t2 = time()
            print("k = {}, time = {}".format(k, (t2 - t1) * 1e6))
            if len(candidates) == 0:
                break
            
            for point in candidates:
                position = self.get_position(point)
                root_position = self.get_position(candidates[point])
                output[position[0]][position[1]] = output[root_position[0]][root_position[1]]

            areas.clear()        
            for point in candidates:
                if areas.get(candidates[point], None) != None:
                    areas[candidates[point]].append(point)
                else:
                    areas[candidates[point]] = [point]

        self.output = np.array(output)
        return self.output

import tifffile as tiff
input = tiff.imread('./NghiaTNH/Input/00.tif')
# input = np.array([
#     [0,0,0,0,0,0,0,0],
#     [0,1,1,0,0,0,0,0],
#     [0,1,0,0,0,0,0,0],
#     [0,0,0,2,2,0,0,0],
#     [0,0,0,0,0,4,0,0],
#     [0,3,0,0,4,4,0,0],
#     [0,3,0,0,0,0,0,0],
#     [0,0,0,0,0,0,0,0],
#     [0,0,0,0,0,0,0,0],
#     ])
solver = DilateSolver(input, 1000)
t1 = time()
output = solver.dilate()
t2 = time()
print(f'time: {(t2 - t1) * 1e6}')
mat= np.matrix(output)
with open('./NghiaTNH/output_nghia.txt','wb') as f:
    for line in mat:
        np.savetxt(f, line, fmt='%d')