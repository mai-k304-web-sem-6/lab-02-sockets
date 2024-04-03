import socket
import time

log_file = open("client.log", "w") # Log файл
config_server = open("client.cfg", "r") # Конфигурационый файл

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Создаём неименовоного сокет типа IPv4 и с протоколом TCP

# Считывание ip и порта с файла, подключение и печать в логфайл
ip_server = str(config_server.readline().rstrip()) # Считывание ip адреса из конфигурационого файла
port_server = int(config_server.readline()) # Считывание порта из конфигурационого файла
sock.connect((ip_server, port_server)) # Подключение к сокету
log_file.write(time.ctime() + " connecting_to " + '\t\t' + ip_server + " " + str(port_server) + '\n') # Вывод подключение в log файл
print("Подключение установлено")

time.sleep(3) # Имитация расстояния

# Отправка строки серверу, печать в логфайл
send_to_server = str(config_server.readline()) # Формирования текста письма на отправку
sock.send(send_to_server.encode()) # Отправка сообщения по сокету
log_file.write(time.ctime() + " it_was_sent " + '\t\t' + send_to_server + '\n') # Вывод результата в log файл

# Получение ответа от сервера, печать в логфайл
print("Ждёт ответа")
time.sleep(2)
data = sock.recv(1024).decode('cp1251') # Получения ответа от сервера
log_file.write(time.ctime() + " it_was_received " + '\t' + data + '\n') # печать резальтата получения ответа в log файл
print("Ответ получен")
print("Ответ: ", data) # Печать полученого ответа в консоль

sock.close() #Отключение от сервера

# Закрытие файлов
log_file.close()
config_server.close()