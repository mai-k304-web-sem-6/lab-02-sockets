#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <locale.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <stdint.h>

// Функция переворачивания строки
void invert_str(char *client_response) {
    int len = strlen(client_response);
    char message[len];
    for (int i = len - 1; i >= 0; i--)
        message[len - (i + 1)] = client_response[i];   
    message[len] = '\0';
    strcpy(client_response, message);
    return;
}

// Функция выведения в log файл времени
void printTimeLog(FILE *logFile) {
    long int ttime;
	ttime = time(NULL);
    char *timeChar;
    timeChar = ctime(&ttime); 
    if (timeChar[strlen(timeChar) - 1] == '\n')
        timeChar[strlen(timeChar) - 1] = '\0';
    fprintf(logFile, "%s", timeChar);
    printf("%s", timeChar);
}

int main() {

    FILE *log_file = fopen("server.log", "a+"); // Log файл

    int listenfd = 0, connfd = 0;

    listenfd = socket(AF_INET, SOCK_STREAM, 0); // Создаём неименовоного сокет типа IPv4 и с протоколом TCP

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET; // Указания тип IP адреса
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Указание самого IP адреса
    serv_addr.sin_port = htons(5000); // Указание порта

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); // Создание сокета используя параметры из структуры 

    listen(listenfd, 5); // Указание максимального кол-ва подключений

    // Логгирование времени запуска сервера
    printTimeLog(log_file);
    fprintf (log_file, ", the server started listening\n");
    printf(", the server started listening\n");
    fflush(log_file);

    while (1) {
        char buf[1024];
        buf[0] = 0;
        struct sockaddr_in peer; // Для информации о клиенте
        int peerLen = sizeof(peer);

        // Принять соединение
        if((connfd = accept(listenfd, (struct sockaddr*)&peer, &peerLen)) == -1) {
            printTimeLog(log_file);
            fprintf (log_file, ", Failed to connect \n");
            printf(", Failed to connect\n");
            fflush(log_file);
            return -1;
        }

        // Сохранение информации о клиенте
        inet_ntop(AF_INET, &peer.sin_addr, buf, sizeof(buf));

        // Логгирование времени присоединения клиента
        printTimeLog(log_file);
        fprintf (log_file, ", a client with the address %s:%d has joined\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
        printf( ", a client with the address %s:%d has joined\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
        fflush(log_file);

        sleep (2); // Имитация загрузки сервера и маршрута

        pid_t id=fork();// Создаем дочерний процесс
        if(id<0)
        {
            perror("fork");
            return -1;
        }
        else if(id==0)
        {//child
            close(listenfd);// Дочернему процессу не нужен sock так близко
            if(fork()==0)// Форк в дочернем процессе для создания дочернего процесса
            { //grand_child
                while(1)
                {
                    ssize_t s=recv(connfd,buf,sizeof(buf), 0);
                    if(s>0)
                    {
                        buf[s]=0;
                        // Логгирование времени получения сообщения
                        printTimeLog(log_file);
                        fprintf (log_file, ", message received: %s\n", buf);
                        printf(", message received: %s\n", buf);
                        fflush(log_file);
                        
                        invert_str(buf);
                        strcat(buf, ". ");
                        strcat(buf, " the server was written by alex_pyslar");
                        buf[strlen(buf)] = '\0';

                        // Отослать сообщение обратно
                        if(send(connfd, buf, strlen(buf), 0) == -1) {
                            printTimeLog(log_file);
                            fprintf (log_file, ", Failed to send message\n");
                            printf(", Failed to send message\n");
                            fflush(log_file);
                            return -1;
                        }

                        // Логгирование времени отправки сообщения сообщения
                        printTimeLog(log_file);
                        fprintf (log_file, ", a message has been sent: %s\n", buf);
                        printf (", a message has been sent: %s\n", buf);
                        fflush(log_file);
                    } else {
                        //Отключение клиента от сервера
                        recv(connfd, &buf, sizeof(buf), 0);

                        // Логгирование времени отключения клиента от сервера
                        printTimeLog(log_file);
                        fprintf (log_file, ", the client with %s:%d the address is disabled \n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
                        printf (", the client with %s:%d the address is disabled \n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
                        fflush(log_file);
                        close(connfd);
                        break;
                    }
                }
            }
            exit(0);// Дочерний процесс завершается напрямую
        }
        else
        {//father
            close(connfd);
            waitpid(id,NULL,0);
        }
    }
    // Закрытие сокета и логфайла
    close(listenfd); 
    close(log_file);
    
	return 0;
}