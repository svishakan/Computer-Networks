#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int findParityBits(int msg_bits);
int *putParityBits(int *data, int data_bits);
int *flipABit(int *data, int bits);
int *detectError(int *array, int bits, int parity_bits);
int *getMessage(int *hammed_data, int bits, int parity_bits);
void printMessage(int *data, int bits);
long convertBinToDec(long bin_value);
int *reverseArray(int *array, int len);



int findParityBits(int msg_bits){
    //Calculates the number of parity bits required
    int i = 0;

    //No. of redundant bits are always power of 2 that is greater than the whole message size
    while(pow(2, i) < msg_bits + i + 1){
        i += 1;
    }

    return i;
}

int *putParityBits(int *data, int data_bits){
    //Places the required parity bits on a given array of data
    //Even parity is assumed here
    int i = 0, j = 0, k = 0, parity_bits = 0, total_bits = 0;
    int *hammed_data, bit_value = 0, parity_pos = 0;

    parity_bits = findParityBits(data_bits);
    total_bits = parity_bits + data_bits;
    hammed_data = (int *)malloc(sizeof(int) * total_bits);

    data = reverseArray(data, data_bits);

    for(i = 0; i < total_bits; i++){
        //Insert empty parity bits at the correct position
        if(i == ((1 << j) - 1)){    // 1 << i == 2^i in binary
            hammed_data[i] = 0;
            j++;
        }

        else{
            hammed_data[i] = data[k];
            k++;
        }
    }

    for(i = 0; i < total_bits; i++){
        //Calculate the parity values for each parity bit

        for(j = 0; j < parity_bits; j++){
            bit_value = ((i + 1) & (1 << j));

            if(bit_value){
                //Implies the index value is not a power of 2, thus a data bit is at 'i'
                parity_pos = (1 << j) - 1;
                hammed_data[parity_pos] = hammed_data[parity_pos] ^ hammed_data[i];
            }
        }
    }

    return reverseArray(hammed_data, total_bits);
}

int *flipABit(int *data, int bits){
    //Flips a random bit in the data to simulate error
    int pos;

    pos = rand() % bits;
    data[pos] = (data[pos] + 1) % 2;

    return data;
}

int *detectError(int *data, int bits, int parity_bits){
    //Detects the error (and corrects, if possible)
    int i = 0, j = 0, parity_pos, bit_value;
    int new_parity = 0, error_pos = 0, *new_data;

    data = reverseArray(data, bits);

    for(j = parity_bits - 1; j >= 0; j--){
        //Calculate the parity values again for each parity bit
        //and compare

        new_parity = 0;

        for(i = 0; i < bits; i++){
            bit_value = ((i + 1) & (1 << j));

            if(bit_value){
                //Implies the index value is not a power of 2, thus a data bit is at 'i'
                new_parity = new_parity ^ data[i];
            }
        }

        error_pos += new_parity * (1 << j);
    }

    if(error_pos){
        printf("\nError found at bit %d.", bits - error_pos + 1);
        data[error_pos - 1] = (data[error_pos - 1] + 1) % 2;
    }

    else{
        printf("\nNo Error was found.");
    }

    return data;
}

int *getMessage(int *hammed_data, int bits, int parity_bits){
    //Retrieves the original message from the Hamming encoded data
    int *data, i = 0, j = 0, k = 0;

    data = (int *)malloc(sizeof(int) * (bits - parity_bits));

    for(i = 0; i < bits; i++){
        if(i != ((1 << j) - 1)){
            data[k++] = hammed_data[i];
        }
        else{
            j++;
        }
    }

    return reverseArray(data, bits - parity_bits);
}

void printMessage(int *data, int bits){
    //Prints a given array of data
    int i = 0;

    for(i = 0; i < bits; i++){
        printf("%d", data[i]);
    }

    return;
}

long convertBinToDec(long bin_value){
    //Converts a given binary value to decimal
    int dec_value = 0, i = 0, rem = 0;

    while(bin_value != 0){
        rem = bin_value % 10;
        bin_value /= 10;
        dec_value += rem * pow(2, i);
        i++;
    }

    return dec_value;
}

int *reverseArray(int *array, int len){
    //Reverses the given array of data
    int *rev, i = 0;
    rev = (int *)malloc(sizeof(int) * len);

    for(i = 0; i < len; i++){
        rev[i] = array[(len - 1) - i];
    }

    return rev;
}