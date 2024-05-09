#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "serial.h"

#define FILENAME "example.txt" // Ҫ���͵��ļ���

// ��������
int sendFileToLinux(const char *filename, PORT com_port);

int main() {
    PORT com_port;
    int ret;

    // ��ʼ������
    com_port = serial_init(1, 115200, 8, ONESTOPBIT, NOPARITY);
    if (com_port == NULL) {
        printf("Failed to open serial port.\n");
        return 1;
    }

    // �����ļ������ļ��ֽ����� Linux ������
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", FILENAME);
        ClosePort(com_port);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    ret = SendData(com_port, FILENAME, strlen(FILENAME));
    if (ret != strlen(FILENAME)) {
        printf("Failed to send file name.\n");
        ClosePort(com_port);
        fclose(file);
        return 1;
    }

    char size_buffer[20];
    snprintf(size_buffer, sizeof(size_buffer), "%ld", file_size);
    ret = SendData(com_port, size_buffer, strlen(size_buffer));
    if (ret != strlen(size_buffer)) {
        printf("Failed to send file size.\n");
        ClosePort(com_port);
        fclose(file);
        return 1;
    }

    // �����ļ����ݵ� Linux ������
    ret = sendFileToLinux(FILENAME, com_port);
    if (ret != 0) {
        printf("Failed to send file to Linux board.\n");
    } else {
        printf("File sent successfully.\n");
    }

    // �رմ��ں��ļ�
    ClosePort(com_port);
    fclose(file);

    return 0;
}

// �������壺�����ļ��� Linux ������
int sendFileToLinux(const char *filename, PORT com_port) {
    FILE *file;
    char buffer[1024];
    size_t bytes_read;
    int bytes_sent;

    // ���ļ�
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", filename);
        return 1;
    }

    // ��ȡ�ļ����ݲ����͵�����
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        bytes_sent = SendData(com_port, buffer, bytes_read);
        if (bytes_sent != bytes_read) {
            printf("Failed to send file data.\n");
            fclose(file);
            return 1;
        }
        // �ȴ�һ��ʱ�䣬�Ա� Linux �������ܹ�������յ�����
        Sleep(10);
    }

    // �ر��ļ�
    fclose(file);

    return 0;
}
