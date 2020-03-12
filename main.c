#include <LibPad/PsPad.h>
#include <LibPad/PsMemCard.h>

#include <stdio.h>

int scanUsbDevices();
const char* getUsbDevicePath(int id);
struct PsIoPhy* createUsbPsIoPhy(const char* usbDevicePath);
void destroyUsbPsIoPhy(struct PsIoPhy*);

void printRxBuffer(struct PsIoBuffer* buffer){
    int i;
    for(i = 0; i < buffer->size; ++i){
        printf("0x%02x, ", (int)buffer->data[i]);
    }
    printf("\n");
}

void printBuffer(const char* buffer, int size){
    int i;
    for(i = 0; i < size; ++i){
        printf("0x%02x, ", (int)buffer[i]);
    }
    printf("\n");
}

const char* findUsbDevice(){
    scanUsbDevices();
    return getUsbDevicePath(0);
}

void readFileDescriptors(struct PsMemCard* mc) {
    uint8_t mem[128];
    char filePathBuffer[256];

    for(int i = 0; i <= 15; ++i){
        sprintf(filePathBuffer, "D:/memcard/fileDescriptor_%d.bin", i);
        psMemCardRead(mc, i, mem);

        FILE* file = fopen(filePathBuffer, "wb");
        fwrite(mem, 1, sizeof(mem), file);
        fclose(file);

        printBuffer(mem, sizeof(mem));
    }
}


void readFileBlocks(struct PsMemCard* mc) {
    uint8_t blockMem[1024 * 8];
    char filePathBuffer[256];

    for(int i = 1; i <= 15; ++i){
        int blockOffset = (i * sizeof(blockMem)) / 128;
        sprintf(filePathBuffer, "D:/memcard/fileBlock_%d.bin", i);

        for(int j = 0; j < (sizeof(blockMem) / 128); j++){
            psMemCardRead(mc, blockOffset + j, &blockMem[j * 128]);
        }

        FILE* file = fopen(filePathBuffer, "wb");
        fwrite(blockMem, 1, sizeof(blockMem), file);
        fclose(file);

        printf("Done block %d\n", i);
    }
}

int main() {
    struct PsIoPhy* phy;
    struct PsIoPort port;
    int i;
    const char* usbDevicePath = findUsbDevice();

    phy = createUsbPsIoPhy(usbDevicePath);

    port.id = 1;
    port.phy = phy;
    port.rxBuffer.size = 0;

    struct PsPad pad;
    pad.slot = 1;
    pad.port = &port;

    struct PsIoBuffer* rxBuffer = &pad.port->rxBuffer;


    struct PsMemCard memCard;
    memCard.port = &port;
    memCard.slot = 0x81;

    readFileDescriptors(&memCard);
    readFileBlocks(&memCard);


    return 0;
    psPadRead(&pad);
    psPadConfigEnter(&pad);
    for(i = 0; i < 5; ++i){
        psPadConfigQueryMode(&pad, i);
        printRxBuffer(rxBuffer);
    }

    psPadConfigExit(&pad);

    destroyUsbPsIoPhy(phy);
    return 0;
}
