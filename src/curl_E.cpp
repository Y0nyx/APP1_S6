#include <iostream>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <vector>

static const int MATRIX_SIZE = 100;
static const int SHAPE = MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE * 3 * sizeof(double);

char buffer_[SHAPE];

class Array4D {
public:
    Array4D() {
        data_.resize(100 * 100 * 100 * 3);
    }

    float& operator()(int i, int j, int k, int c) {
        return data_[i + j * 100 + k * 100 * 100 + c * 100 * 100 * 100];
    }

    const float& operator()(int i, int j, int k, int c) const {
        return data_[i + j * 100 + k * 100 * 100 + c * 100 * 100 * 100];
    }

    double* data() {
        return reinterpret_cast<double*>(data_.data());
    }

    size_t size() const {
        return data_.size();
    }

private:
    std::vector<float> data_;
};

Array4D input_array(double* mtx, int dim1, int dim2, int dim3, int dim4) {
    Array4D arr;
    int count = 0;
    for (int c = 0; c < dim4; c++) {
        for (int k = 0; k < dim3; k++) {
            for (int j = 0; j < dim2; j++) {
                for (int i = 0; i < dim1; i++) {
                    arr(i, j, k, c) = static_cast<float>(mtx[count]);
                    count++;
                }
            }
        }
    }
    return arr;
}

Array4D curl_E(const Array4D E) {
    Array4D curl_e;
    
    // curl_E[:, :-1, :, 0] += E[:, 1:, :, 2] - E[:, :-1, :, 2]
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 99; j++) {
            for(int k = 0; k < 100; k++) {
                curl_e(i, j, k, 0) += E(i, j+1, k, 2) - E(i, j, k, 2);
            }
        }
    }

    // curl_E[:, :, :-1, 0] -= E[:, :, 1:, 1] - E[:, :, :-1, 1]
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++) {
            for(int k = 0; k < 99; k++) {
                curl_e(i, j, k, 0) += E(i, j, k+1, 1) - E(i, j, k, 1);
            }
        }
    }

        // curl_E[:, :, :-1, 1] += E[:, :, 1:, 0] - E[:, :, :-1, 0]
        for(int i = 0; i < 100; i++){
            for(int j = 0; j < 100; j++) {
                for(int k = 0; k < 99; k++) {
                    curl_e(i, j, k, 1) += E(i, j, k+1, 0) - E(i, j, k, 0);
                }
            }
        }

        // curl_E[:-1, :, :, 1] -= E[1:, :, :, 2] - E[:-1, :, :, 2]
        for(int i = 1; i < 100; i++){
            for(int j = 0; j < 100; j++) {
                for(int k = 0; k < 100; k++) {
                    curl_e(i-1, j, k, 1) -= E(i, j, k, 2) - E(i-1, j, k, 2);
                }
            }
        }

        // curl_E[:-1, :, :, 2] += E[1:, :, :, 1] - E[:-1, :, :, 1]
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                for (int k = 0; k < 99; k++) {
                    curl_e(i, j, k, 2) += E(i+1, j, k, 1) - E(i, j, k, 1);
                }
            }
        }

    // curl_E[:, :-1, :, 2] -= E[:, 1:, :, 0] - E[:, :-1, :, 0]
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 99; j++) {
            for (int k = 0; k < 100; k++) {
                curl_e(i, j, k, 2) -= E(i, j+1, k, 0) - E(i, j, k, 0);
            }
        }
    }

    return curl_e;
}

Array4D wait_signal()
{
    std::string msg;
    std::cin >> msg;
    std::cerr << "curl-E: Got signal." << std::endl;
}

void ack_signal()
{
    // Répond avec un message vide.
    std::cout << "" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2)
    {
        std::cerr << "curl-E: Error : no shared file provided as argv[1]" << std::endl;
        return -1;
    }
    
    wait_signal();

    memset(buffer_, 0, SHAPE);
    FILE* shm_f = fopen(argv[1], "w");
    if (shm_f == nullptr) {
        std::cerr << "curl-E:  File nullptr" << std::endl;
    }
    fwrite(buffer_, sizeof(char), SHAPE, shm_f);


    fclose(shm_f);

    // On signale que le fichier est pret
    std::cerr << "curl-E:  File ready." << std::endl;
    ack_signal();

    // On ré-ouvre le fichier et le passe à mmap(...). Le fichier peut ensuite
    // être fermé sans problèmes (mmap y a toujours accès, jusqu'à munmap.)
    int shm_fd = open(argv[1], O_RDWR);
    void* shm_mmap = mmap(NULL, SHAPE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    close(shm_fd);

    if (shm_mmap == MAP_FAILED) {
        std::cerr << "ERROR SHM\n";
        perror(NULL);
        return -1;
    }

    // Pointeur format double qui représente la matrice partagée:
    double* mtx = (double*)shm_mmap;

    while(true) {
        wait_signal();
        std::cerr << "curl_E: execute curl_E" << std::endl;

        Array4D output_array = curl_E(input_array(mtx, 100, 100, 100, 3));
        mtx = output_array.data();

        // On signale que le travail est terminé.
        std::cerr << "curl_E: Work done." << std::endl;
        ack_signal();
    }

    munmap(shm_mmap, SHAPE);
    return 0;
}
