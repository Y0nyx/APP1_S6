#include <iostream>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <cstring>

#include <vector>

static const int MATRIX_SIZE = 50;
static const int SHAPE = MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE * 3 * sizeof(double);

char buffer_[SHAPE];

class Array4D {
public:
    Array4D() {
        data_.resize(MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE * 3);
    }

    double& operator()(int i, int j, int k, int c) {
        return data_[i + j * MATRIX_SIZE + k * MATRIX_SIZE * MATRIX_SIZE + c * MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE];
    }

    const double& operator()(int i, int j, int k, int c) const {
        return data_[i + j * MATRIX_SIZE + k * MATRIX_SIZE * MATRIX_SIZE + c * MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE];
    }

    double* data() {
        return data_.data();
    }

    size_t size() const {
        return data_.size();
    }

private:
    std::vector<double> data_;
};

Array4D input_array(double* mtx) {
    Array4D arr;
    int count = 0;
    for (int c = 0; c < MATRIX_SIZE; c++) {
        for (int k = 0; k < MATRIX_SIZE; k++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                for (int i = 0; i < 3; i++) {
                    arr(i, j, k, c) = static_cast<float>(mtx[count]);
                    count++;
                }
            }
        }
    }
    return arr;
}

Array4D curl_H(const Array4D H) {
    Array4D curl_h;
    
    // curl_H[:,1:,:,0] += H[:,1:,:,2] - H[:,:-1,:,2]
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 99; j++) {
            for(int k = 0; k < 100; k++) {
                curl_h(i, j, k, 0) += H(i, j+1, k, 2) - H(i, j, k, 2);
            }
        }
    }

    // curl_H[:,:,1:,0] -= H[:,:,1:,1] + H[:,:,:-1,1]
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++) {
            for(int k = 0; k < 99; k++) {
                curl_h(i, j, k, 0) -= H(i, j, k+1, 1) + H(i, j, k, 1);
            }
        }
    }

    // curl_H[:,:,1:,1] += H[:,:,1:,0] - H[:,:,:-1,0]
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++) {
            for(int k = 0; k < 99; k++) {
                curl_h(i, j, k, 1) += H(i, j, k+1, 0) - H(i, j, k, 0);
            }
        }
    }

    // curl_H[1:,:,:,1] -= H[1:,:,:,2] - H[:-1,:,:,2]
    for(int i = 1; i < 100; i++){
        for(int j = 0; j < 100; j++) {
            for(int k = 0; k < 100; k++) {
                curl_h(i-1, j, k, 1) -= H(i, j, k, 2) - H(i-1, j, k, 2);
            }
        }
    }
    // curl_H[1:,:,:,2] += H[1:,:,:,1] - H[:-1,:,:,1]
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            for (int k = 0; k < 99; k++) {
                curl_h(i, j, k, 2) += H(i+1, j, k, 1) - H(i, j, k, 1);
            }
        }
    }

    // curl_H[:,1:,:,2] -= H[:,1:,:,0] - H[:,:-1,:,0]
    for (int i = 0; i < 100; i++) {
        for (int j = 1; j < 100; j++) {
            for (int k = 0; k < 100; k++) {
                curl_h(i, j, k, 2) -= H(i, j, k, 0) - H(i, j-1, k, 0);
            }
        }
    }

    return curl_h;
}

Array4D wait_signal()
{
    std::string msg;
    std::getline(std::cin, msg);
    std::cerr << "curl-H: Got signal : " << msg.data() << std::endl;
}

void ack_signal()
{
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2)
    {
        std::cerr << "curl-H: Error : no shared file provided as argv[1]" << std::endl;
        return -1;
    }
    
    wait_signal();

    std::cerr << "curl-H:  try to open file" << std::endl;
    FILE* shm_f = fopen(argv[1], "w");
    if (shm_f == nullptr) {
        std::cerr << "curl-H:  File nullptr" << std::endl;
    }

    memset(buffer_, 0, SHAPE);
    fwrite(buffer_, sizeof(char), SHAPE, shm_f);

    // On signale que le fichier est pret
    std::cerr << "curl-H:  File ready." << std::endl;
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
        std::cerr << "curl_H: execute curl_H" << std::endl;

        Array4D output_array = curl_H(input_array(mtx));
        std::memcpy(mtx, output_array.data(), output_array.size() * sizeof(double));


        // On signale que le travail est terminé.
        std::cerr << "curl_H: Work done." << std::endl;
        ack_signal();
    }

    munmap(shm_mmap, SHAPE);
    return 0;
}