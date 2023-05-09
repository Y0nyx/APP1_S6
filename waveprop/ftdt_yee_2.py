# GPL3, Copyright (c) Max Hofheinz, UdeS, 2021

import fiddle
import numpy as np
from subprocess import Popen, PIPE

def curl_E(E):
    process = Popen(["./curls_functions"], stdin=PIPE, stdout=PIPE, text=True)
    input_str = "0\n" + f"{E.shape[0]} {E.shape[1]} {E.shape[2]}\n"
    input_str += " ".join(str(x) for x in E.flatten())

    output_str, _ = process.communicate(input_str)
    output_values = np.fromstring(output_str, sep=" ").reshape(E.shape)

    return output_values

def curl_H(H):
    process = Popen(["./curls_functions"], stdin=PIPE, stdout=PIPE, text=True)
    input_str = "1\n" + f"{H.shape[0]} {H.shape[1]} {H.shape[2]}\n"
    input_str += " ".join(str(x) for x in H.flatten())

    output_str, _ = process.communicate(input_str)
    output_values = np.fromstring(output_str, sep=" ").reshape(H.shape)

    return output_values

# The rest of your Python code...



def timestep(E, H, courant_number, source_pos, source_val):
    E += courant_number * curl_H(H)
    E[source_pos] += source_val
    H -= courant_number * curl_E(E)
    return E, H


class WaveEquation:
    def __init__(self, s, courant_number, source):
        s = s + (3,)
        self.E = np.zeros(s)
        self.H = np.zeros(s)
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
        self.E, self.H = timestep(self.E, self.H, self.courant_number, source_pos, source_index)

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

    def source(index):
        return ([n // 3], [n // 3], [n // 2],[0]), 0.1*np.sin(0.1 * index)
    
    #w = WaveEquation((n, n, n), courant_number, source)

    #for i in range(0, 5) :
    #    source_pos, source_index = source(i)
    #    w.E, w.H = timestep(w.E, w.H, courant_number, source_pos, source_index)
    #    print(w.E)

    w = WaveEquation((n, n, n), 0.1, source)
    fiddle.fiddle(w, [('field component',{'Ex':0,'Ey':1,'Ez':2, 'Hx':3,'Hy':4,'Hz':5}),('slice',{'XY':2,'YZ':0,'XZ':1}),('slice index',0,n-1,n//2,1)], update_interval=0.01)


