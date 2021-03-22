import threading
import colorama
from telnetlib import Telnet

# Читаю файл с запросами, разделенными ---.
def request_list(filename):
    file = open(filename, 'r')

    request_list = []
    request = ""
    for line in file:
        line = line.strip()
        if line == "---":
            request_list.append(request)
            request = ""
            continue
        line += "\r\n"
        request += line
    return (request_list)

# Использую программу telnet, посылаю простой запрос(нужно потом добавить
# другие запросы для теста), потом читаю по строке
def telnet(host, port, path):
    tl = Telnet(host, port)
    testRequests = request_list("requests")
    print(testRequests)
    tl.write(bytes(testRequests[compare.testNum], "utf-8"))
    response = ""
    while 1:
        try:
            response_line = tl.read_until("\n".encode("ascii"), timeout=1).decode("utf-8")
        except:
            # break
            response_line = ""
        response += response_line
        if (response_line == ""):
            break
    tl.close()
    return (response)

# Подключаюсь к своему серверу и к nginx, сравниваю ответы
def compare(path, details):
    originalResponse = telnet("localhost", 80, path)
    localResponse = telnet("localhost", 8080, path)
    if (localResponse != originalResponse):
        print(f"{colorama.Fore.RED}Test #{compare.testNum} failed{colorama.Style.RESET_ALL}")
        if (details == 1):
            print(f"{colorama.Fore.BLUE}Original response:\n{colorama.Style.RESET_ALL}", originalResponse)
            print(f"{colorama.Fore.BLUE}Local response:\n{colorama.Style.RESET_ALL}", localResponse)
    else:
        print(f"{colorama.Fore.RED}Test #{compare.testNum} correct{colorama.Style.RESET_ALL}")
    compare.testNum = 2

# 0 - не выводить запросы, 1 - выводить
details = 1
# Список потоков
threads = []
# Количество потоков
thread_num = 1

# Создаю потоки, добавляю в список и запускаю
for i in range(thread_num):
    compare.testNum = 1
    t = threading.Thread(target=compare, args=("/", details,))
    threads.append(t)
    t.start()

# Ожидаю завершения всех потоков
for thread in threads:
    thread.join()