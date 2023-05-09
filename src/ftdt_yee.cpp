#include <iostream>
#include <vector>

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

private:
    std::vector<float> data_;
};

Array4D curl_E(const Array4D& E, int separation) {
    Array4D curl_E;
    
    if (separation == 0) {
        // curl_E[:, :-1, :, 0] += E[:, 1:, :, 2] - E[:, :-1, :, 2]
        for(int i = 0; i < 100; i++){
            for(int j = 0; j < 99; j++) {
                for(int k = 0; k < 100; k++) {
                    curl_E(i, j, k, 0) += E(i, j+1, k, 2) - E(i, j, k, 2);
                }
            }
        }

        // curl_E[:, :, :-1, 0] -= E[:, :, 1:, 1] - E[:, :, :-1, 1]
        for(int i = 0; i < 100; i++){
            for(int j = 0; j < 100; j++) {
                for(int k = 0; k < 99; k++) {
                    curl_E(i, j, k, 0) += E(i, j, k+1, 1) - E(i, j, k, 1);
                }
            }
        }
    } else if (separation == 1) {
        // curl_E[:, :, :-1, 1] += E[:, :, 1:, 0] - E[:, :, :-1, 0]
        for(int i = 0; i < 100; i++){
            for(int j = 0; j < 100; j++) {
                for(int k = 0; k < 99; k++) {
                    curl_E(i, j, k, 1) += E(i, j, k+1, 0) - E(i, j, k, 0);
                }
            }
        }

        // curl_E[:-1, :, :, 1] -= E[1:, :, :, 2] - E[:-1, :, :, 2]
        for(int i = 1; i < 100; i++){
            for(int j = 0; j < 100; j++) {
                for(int k = 0; k < 100; k++) {
                    curl_E(i-1, j, k, 1) -= E(i, j, k, 2) - E(i-1, j, k, 2);
                }
            }
        }
    } else if (separation == 2) {
        // curl_E[:-1, :, :, 2] += E[1:, :, :, 1] - E[:-1, :, :, 1]
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                for (int k = 0; k < 99; k++) {
                    curl_E(i, j, k, 2) += E(i+1, j, k, 1) - E(i, j, k, 1);
                }
            }
        }

        // curl_E[:, :-1, :, 2] -= E[:, 1:, :, 0] - E[:, :-1, :, 0]
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 99; j++) {
                for (int k = 0; k < 100; k++) {
                    curl_E(i, j, k, 2) -= E(i, j+1, k, 0) - E(i, j, k, 0);
                }
            }
        }
    }

    return curl_E;
}

Array4D curl_H(const Array4D& H) {
    Array4D curl_H;
    
    // curl_H[:,1:,:,0] += H[:,1:,:,2] - H[:,:-1,:,2]
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 99; j++) {
            for(int k = 0; k < 100; k++) {
                curl_H(i, j, k, 0) += H(i, j+1, k, 2) - H(i, j, k, 2);
            }
        }
    }

    // curl_H[:,:,1:,0] -= H[:,:,1:,1] + H[:,:,:-1,1]
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++) {
            for(int k = 0; k < 99; k++) {
                curl_H(i, j, k, 0) -= H(i, j, k+1, 1) + H(i, j, k, 1);
            }
        }
    }

    // curl_H[:,:,1:,1] += H[:,:,1:,0] - H[:,:,:-1,0]
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++) {
            for(int k = 0; k < 99; k++) {
                curl_H(i, j, k, 1) += H(i, j, k+1, 0) - H(i, j, k, 0);
            }
        }
    }

    // curl_H[1:,:,:,1] -= H[1:,:,:,2] - H[:-1,:,:,2]
    for(int i = 1; i < 100; i++){
        for(int j = 0; j < 100; j++) {
            for(int k = 0; k < 100; k++) {
                curl_H(i-1, j, k, 1) -= H(i, j, k, 2) - H(i-1, j, k, 2);
            }
        }
    }
    // curl_H[1:,:,:,2] += H[1:,:,:,1] - H[:-1,:,:,1]
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            for (int k = 0; k < 99; k++) {
                curl_H(i, j, k, 2) += H(i+1, j, k, 1) - H(i, j, k, 1);
            }
        }
    }

    // curl_H[:,1:,:,2] -= H[:,1:,:,0] - H[:,:-1,:,0]
    for (int i = 0; i < 100; i++) {
        for (int j = 1; j < 100; j++) {
            for (int k = 0; k < 100; k++) {
                curl_H(i, j, k, 2) -= H(i, j, k, 0) - H(i, j-1, k, 0);
            }
        }
    }

    return curl_H;
}

void timestep(Array4D& E, Array4D& H, float courant_number, int sourcePos[4]) {

}

int main(int argc, char** argv)
{
    float courant_number = 0.1;
    int sourcePos[4] = {int(SIZE/3), int(SIZE/3), int(SIZE/2), 0};

    Array4D E;
    Array4D H;

    curl_e = curl_E()

    for(int i = 0; i < 50; i++) {
        timestep(E, H, courant_number, sourcePos)
    }
}



