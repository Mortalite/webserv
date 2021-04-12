import threading
import colorama
from telnetlib import Telnet

def print_red(input):
    print(f"{colorama.Fore.RED}{input}{colorama.Style.RESET_ALL}")

def print_blue(input):
    print(f"{colorama.Fore.BLUE}{input}{colorama.Style.RESET_ALL}")

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
    originalResponse = telnet("localhost", 8080, path)
    localResponse = telnet("localhost", 9000, path)
    if (localResponse != originalResponse):
        print(f"{colorama.Fore.RED}Test #{compare.testNum} failed{colorama.Style.RESET_ALL}")
    else:
        print(f"{colorama.Fore.RED}Test #{compare.testNum} correct{colorama.Style.RESET_ALL}")
    if (details == 1):
        print(print_red("Original response:\n"), originalResponse)
        print(print_blue("Local response:\n"), localResponse)
    compare.testNum = 2

# 0 - не выводить запросы, 1 - выводить
details = 1
# Список потоков
threads = []
# Количество потоков
thread_num = 1

# Создаю потоки, добавляю в список и запускаю
for i in range(thread_num):
    compare.testNum = 3
    t = threading.Thread(target=compare, args=("/", details,))
    threads.append(t)
    t.start()

# Ожидаю завершения всех потоков
for thread in threads:
    thread.join()