#include <iostream>
#include <vector>
#include <omp.h>
#include <cstring>

#include <iostream>
#include <vector>
#include <omp.h>
#include <cstring>

static const int MATRIX_SIZE = 100;

void curl_E(double* E, double* curl_E_out, int nx, int ny, int nz) {
    int i, j, k;
    #pragma omp parallel for private(i, j, k) shared(E, curl_E_out)
        for(i = 0; i < nx; i++){
            for(j = 0; j < ny - 1; j++){
                for(k = 0; k < nz; k++){
                    curl_E_out[i*ny*nz*3 + j*nz*3 + k*3] += E[i*ny*nz*3 + (j+1)*nz*3 + k*3 + 2] - E[i*ny*nz*3 + j*nz*3 + k*3 + 2];
                }
            }
        }

        for(i = 0; i < nx; i++){
            for(j = 0; j < ny; j++){
                for(k = 0; k < nz - 1; k++){
                    curl_E_out[i*ny*nz*3 + j*nz*3 + k*3] -= E[i*ny*nz*3 + j*nz*3 + (k+1)*3 + 1] - E[i*ny*nz*3 + j*nz*3 + k*3 + 1];
                }
            }
        }

        for(i = 0; i < nx; i++){
            for(j = 0; j < ny; j++){
                for(k = 0; k < nz - 1; k++){
                    curl_E_out[i*ny*nz*3 + j*nz*3 + k*3 + 1] += E[i*ny*nz*3 + j*nz*3 + (k+1)*3] - E[i*ny*nz*3 + j*nz*3 + k*3];
                }
            }
        }

        for(i = 0; i < nx - 1; i++){
            for(j = 0; j < ny; j++){
                for(k = 0; k < nz; k++){
                    curl_E_out[i*ny*nz*3 + j*nz*3 + k*3 + 1] -= E[(i+1)*ny*nz*3 + j*nz*3 + k*3 + 2] - E[i*ny*nz*3 + j*nz*3 + k*3 + 2];
                }
            }
        }

        for(i = 0; i < nx - 1; i++){
            for(j = 0; j < ny; j++){
                for(k = 0; k < nz; k++){
                    curl_E_out[i*ny*nz*3 + j*nz*3 + k*3 + 2] += E[(i+1)*ny*nz*3 + j*nz*3 + k*3 + 1] - E[(i-1)*ny*nz*3 + j*nz*3 + k*3 + 1];
                }
            }
        }

        for(i = 0; i < nx; i++){
            for(j = 0; j < ny - 1; j++){
                for(k = 0; k < nz; k++){
                    curl_E_out[i*ny*nz*3 + j*nz*3 + k*3 + 2] += E[i*ny*nz*3 + (j+1)*nz*3 + k*3] - E[i*ny*nz*3 + (j-1)*nz*3 + k*3];
                }
            }
        }
}


void curl_H(double* H, double* curl_H_out, int nx, int ny, int nz) {
    int i, j, k;
    #pragma omp parallel for private(i, j, k) shared(H, curl_H_out)
        for(i = 0; i < nx; i++){
            for(j = 1; j < ny; j++){
                for(k = 0; k < nz; k++){
                    curl_H_out[i*ny*nz*3 + j*nz*3 + k*3] += H[i*ny*nz*3 + j*nz*3 + k*3 + 2] - H[i*ny*nz*3 + (j-1)*nz*3 + k*3 + 2];
                }
            }
        }

        for(i = 0; i < nx; i++){
            for(j = 0; j < ny; j++){
                for(k = 1; k < nz; k++){
                    curl_H_out[i*ny*nz*3 + j*nz*3 + k*3] -= H[i*ny*nz*3 + j*nz*3 + k*3 + 1] - H[i*ny*nz*3 + j*nz*3 + (k-1)*3 + 1];
                }
            }
        }

        for(i = 0; i < nx; i++){
            for(j = 0; j < ny; j++){
                for(k = 1; k < nz; k++){
                    curl_H_out[i*ny*nz*3 + j*nz*3 + k*3 + 1] += H[i*ny*nz*3 + j*nz*3 + k*3] - H[i*ny*nz*3 + j*nz*3 + (k-1)*3];
                }
            }
        }

        for(i = 1; i < nx; i++){
            for(j = 0; j < ny; j++){
                for(k = 0; k < nz; k++){
                    curl_H_out[i*ny*nz*3 + j*nz*3 + k*3 + 1] -= H[i*ny*nz*3 + j*nz*3 + k*3 + 2] - H[(i-1)*ny*nz*3 + j*nz*3 + k*3 + 2];
                }
            }
        }

        for(i = 1; i < nx; i++){
            for(j = 0; j < ny; j++){
                for(k = 0; k < nz; k++){
                    curl_H_out[i*ny*nz*3 + j*nz*3 + k*3 + 2] += H[i*ny*nz*3 + j*nz*3 + k*3 + 1] - H[(i-1)*ny*nz*3 + j*nz*3 + k*3 + 1];
                }
            }
        }

        for(i = 0; i < nx; i++){
            for(j = 1; j < ny; j++){
                for(k = 0; k < nz; k++){
                    curl_H_out[i*ny*nz*3 + j*nz*3 + k*3 + 2] += H[i*ny*nz*3 + j*nz*3 + k*3] - H[i*ny*nz*3 + (j-1)*nz*3 + k*3];
                }
            }
        }
}


int main() {
    int function_choice;
    std::cin >> function_choice;

    std::vector<double> input_data(MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE * 3);
    std::vector<double> output_data(MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE * 3);

    for (size_t i = 0; i < input_data.size(); ++i) {
        std::cin >> input_data[i];
    }

    if (function_choice == 0) {
        curl_E(input_data.data(), output_data.data(), MATRIX_SIZE, MATRIX_SIZE, MATRIX_SIZE);
    } else {
        curl_H(input_data.data(), output_data.data(), MATRIX_SIZE, MATRIX_SIZE, MATRIX_SIZE);
    }

    for (const auto &value : output_data) {
        std::cout << value << ' ';
    }

    return 0;
}
