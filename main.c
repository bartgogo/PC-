#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "serial.h"

#define FILENAME "example.txt" // Ĭ��Ҫ���͵��ļ���

// ��������
int receiveFileFromLinux(const char *outputFilename, PORT com_port);
int sendFileToLinux(const char *filename, PORT com_port);

int main(int argc, char *argv[]) {
    // ����������
    if (argc != 2) {
        printf("Usage: %s <send | receive>\n", argv[0]);
        return 1;
    }

    // ���ݲ���ѡ����ճ�����ͳ���
    if (strcmp(argv[1], "send") == 0) {
        // ���ͳ���
        char filename[256];
        printf("Enter the file name to send: ");
        scanf("%s", filename);
        PORT com_port = serial_init(1, 115200, 8, ONESTOPBIT, NOPARITY);
        if (com_port == NULL) {
            printf("Failed to open serial port.\n");
            return 1;
        }
        int ret = sendFileToLinux(filename, com_port);
        if (ret != 0) {
            printf("Failed to send file to Linux board.\n");
        } else {
            printf("File sent successfully.\n");
        }
        ClosePort(com_port);
    } else if (strcmp(argv[1], "receive") == 0) {
        // ���ճ���
        PORT com_port = serial_init(1, 115200, 8, ONESTOPBIT, NOPARITY);
        if (com_port == NULL) {
            printf("Failed to open serial port.\n");
            return 1;
        }
        int ret = receiveFileFromLinux("received_file.txt", com_port);
        if (ret != 0) {
            printf("Failed to receive file from Linux board.\n");
            ClosePort(com_port);
            return 1;
        } else {
            printf("File received successfully.\n");
        }
        ClosePort(com_port);
    } else {
        printf("Invalid option: %s\n", argv[1]);
        return 1;
    }

    return 0;
}

// �������壺�����ļ��� Linux ������
int receiveFileFromLinux(const char *outputFilename, PORT com_port) {
    FILE *file;
    char buffer[1024];
    size_t bytes_read;
    int bytes_written;

    // ���ļ�
    file = fopen(outputFilename, "wb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", outputFilename);
        return 1;
    }

    // ��ȡ�ļ���
    bytes_read = ReciveData(com_port, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        printf("Failed to receive filename.\n");
        fclose(file);
        return 1;
    }
    buffer[bytes_read] = '\0'; // Add null terminator
    printf("Received filename: %s\n", buffer);

    // ��ȡ�ļ���С
    bytes_read = ReciveData(com_port, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        printf("Failed to receive file size.\n");
        fclose(file);
        return 1;
    }
    buffer[bytes_read] = '\0'; // Add null terminator
    long file_size = strtol(buffer, NULL, 10);
    printf("Received file size: %ld\n", file_size);

    // �����ļ����ݲ�д���ļ�
    long bytes_received = 0;
    while (bytes_received < file_size) {
        bytes_read = ReciveData(com_port, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
            printf("Failed to receive file data.\n");
            fclose(file);
            return 1;
        }
        fwrite(buffer, 1, bytes_read, file);
        bytes_received += bytes_read;
    }

    // �ر��ļ�
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

    // �����ļ����� Linux ������
    bytes_sent = SendData(com_port, filename, strlen(filename));
    if (bytes_sent <= 0) {
        printf("Failed to send filename.\n");
        fclose(file);
        return 1;
    }

    // ��ȡ�ļ���С
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // �����ļ���С�� Linux ������
    char size_buffer[20];
    snprintf(size_buffer, sizeof(size_buffer), "%ld", file_size);
    bytes_sent = SendData(com_port, size_buffer, strlen(size_buffer));
    if (bytes_sent <= 0) {
        printf("Failed to send file size.\n");
        fclose(file);
        return 1;
    }

    // �����ļ����ݵ� Linux ������
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        bytes_sent = SendData(com_port, buffer, bytes_read);
        if (bytes_sent != bytes_read) {
            printf("Failed to send file data.\n");
            fclose(file);
            return 1;
        }
    }

    // �ر��ļ�
    fclose(file);

    return 0;
}
