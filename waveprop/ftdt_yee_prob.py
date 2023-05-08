# GPL3, Copyright (c) Max Hofheinz, UdeS, 2021
import numpy as np
from subprocess import Popen, PIPE
import mmap
import time

# DEFINE
FNAME_1       = "GIF642-problematique_1"
FNAME_2       = "GIF642-problematique_2"
MATRIX_SIZE = 100
SHAPE = (MATRIX_SIZE, MATRIX_SIZE, MATRIX_SIZE, 3)

def subp():
    subproc1 = Popen(["./curl_E", FNAME_1], stdin=PIPE, stdout=PIPE, stderr=open('err1.log', 'w'))
    subproc2 = Popen(["./curl_H", FNAME_2], stdin=PIPE, stdout=PIPE, stderr=open('err2.log', 'w'))
    return (subproc1, subproc2)


def signal_and_wait(subprocs):
    subprocs[0].stdin.write("START\n".encode())
    subprocs[0].stdin.flush()

    #subprocs[1].stdin.write("START\n".encode())
    #subprocs[1].stdin.flush()
    
    H_res = subprocs[0].stdout.readline()
    #E_res = subprocs[1].stdout.readline()


def timestep(E, H, courant_number, source_pos, source_val, curl_E, curl_H):
    E += courant_number * curl_H
    E[source_pos] += source_val
    H -= courant_number * curl_E
    return E, H


class WaveEquation:
    def __init__(self, courant_number, source):
        
        self.E = np.zeros(shape=SHAPE)
        self.H = np.zeros(shape=SHAPE)
        self.curl_H = np.ndarray(shape=SHAPE, dtype=np.float64, buffer=shm_mm_1)
        self.curl_E = np.ndarray(shape=SHAPE, dtype=np.float64, buffer=shm_mm_2)
        self.courant_number = courant_number
        self.source = source
        self.index = 0

    def __call__(self, figure, field_component, slice, slice_index, initial=False):
        if field_component < 3:
            field = self.E
        else:
            field = self.H
            field_component = field_component % 3
        if slice == 0:
            field = field[slice_index, :, :, field_component]
        elif slice == 1:
            field = field[:, slice_index, :, field_component]
        elif slice == 2:
            field = field[:, :, slice_index, field_component]
        source_pos, source_index = source(self.index)
        self.E, self.H = timestep(self.E, self.H, self.courant_number, source_pos, source_index, self.curl_E, self.curl_H)
        self.curl_H = self.H
        self.curl_E = self.E

        if initial:
            axes = figure.add_subplot(111)
            self.image = axes.imshow(field, vmin=-1e-2, vmax=1e-2)
        else:
            self.image.set_data(field)
        self.index += 1


if __name__ == "__main__":
    n = 100
    r = 0.01
    l = 30
    courant_number = 0.1

    subprocs = subp()

    signal_and_wait(subprocs)

    E = np.zeros(shape=SHAPE)
    H = np.zeros(shape=SHAPE)

    shm_f1 = open(FNAME_1, "r+b")
    shm_mm_1 = mmap.mmap(shm_f1.fileno(), 0)
    #shm_f2 = open(FNAME_2, "r+b")
    #shm_mm_2 = mmap.mmap(shm_f2.fileno(), 0)

    curl_H = np.ndarray(shape=SHAPE, dtype=np.float64, buffer=shm_mm_1)
    #curl_E = np.ndarray(shape=SHAPE, dtype=np.float64, buffer=shm_mm_2)

    curl_H = H
    curl_E = E

    def source(index):
        return ([n // 3], [n // 3], [n // 2],[0]), 0.1*np.sin(0.1 * index)

    for i in range(0, 50) :
        signal_and_wait(subprocs)
        
        source_pos, source_index = source(i)
        E_return, H_return = timestep(E, H, courant_number, source_pos, source_index, curl_E, curl_H)
        curl_H = H_return
        curl_E = E_return
    
    for sub in subprocs:
        sub.kill()

    shm_mm1.close()
    shm_mm2.close()
